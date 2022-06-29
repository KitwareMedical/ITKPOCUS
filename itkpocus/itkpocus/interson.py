import numpy as np
import skvideo
import skvideo.io
from skimage.transform import SimilarityTransform
from scipy.signal import find_peaks
from itkpocus.util import get_framerate
import itk
from skimage.morphology import disk
from skimage.morphology import dilation
import skimage.filters
from struct import unpack, calcsize

'''
Preprocessing and device-specific IO for the Interson SP-L01.
'''

def _find_spacing_and_crop(npimg):
    '''
    Find the ruler in the image to calculate the physical dimensions and then crop to the B-mode data.
    
    Parameters
    ----------
    npimg : ndarray
        MxNx3
    
    Returns
    =======
    spacing
    crop : ndarray
        [[ymin,ymax],[xmin,xmax]]
    '''
    colsum = np.sum(npimg[:,:,0], axis=0)
    # find left/right bounds
    # just going assume the ultrasound is in the middle of the image and work outwards to find the letterboxing
    midcol = npimg.shape[1]/2.0 # not worried if non-integer
    zerocol = np.argwhere(colsum == 0)
    leftbound = np.max(zerocol[zerocol < midcol])+1
    rightbound = np.min(zerocol[midcol < zerocol])-1
    
    midrow = npimg.shape[0]/2.0
    rowsum = np.sum(npimg[:,leftbound:rightbound+1,0], axis=1)
    zerorow = np.argwhere(rowsum == 0)
   
    topbound = np.max(np.append(zerorow[zerorow < midrow], 0))
    bottombound = np.min(np.append(zerorow[midrow < zerorow],  npimg.shape[0]-1))   
    
    # interson ruler on left side and has text above it
    colsum2 = np.sum(npimg[topbound:(bottombound+1),:,0], axis=0)
    col_peaks, col_props = find_peaks(colsum2, prominence=20)
    ruler_col = col_peaks[0]
    ruler_peaks, ruler_props = find_peaks(npimg[topbound:(bottombound+1),ruler_col,0])
    # TODO: stress test this with max zoomed video, not sure if the ruler would switch to 0.5 cm spacing or < 4 circles would be on the video
    spacing = 1.0 / np.max(ruler_peaks[1:] - ruler_peaks[0:len(ruler_peaks)-1]) # 1.0 mm per ruler spacing, we get the max peak distance because half-circles on the top and bottom shift the peaks
    
    
    return spacing, np.array([[topbound, bottombound], [leftbound, rightbound]])

def _normalize(npimg, npimgrgb):
    '''
    A bunch of pixel-hacking to find the overlay elements in the image.  Returns the overlay (necessary) 
    for cropping later on and the image with the overlay elements in-filled (median filter at overlay pixels).

    Returns
    -------
    npnorm : ndarray
        MxN normalized image to be cropped later (median filtered hud elements)
    npmasked : ndarray
        MxN hud image that is input to cropping algorithm
    '''    
    white_threshold = 245
    black_threshold = 6
    dilation_radius = 3
    median_filter_radius = 9
    
    # try to detect hud elements by their color (ultrasound should be grey) and pure whiteness
    npcolor = np.logical_not(np.logical_and(npimgrgb[:,:,0] == npimgrgb[:,:,1], npimgrgb[:,:,1] == npimgrgb[:,:,2]))
    npwhite = npimg > white_threshold
    npblack = npimg < black_threshold # tries to get rid of black noise
    nphud = np.logical_or(npcolor, npwhite, npblack)
    nphud2 = dilation(nphud, disk(4))
    npmasked = npimg.copy()
    npmasked[nphud2] = 0

    # now we have a cropped image, need to get rid of the annotation marks
    nphud3 = dilation(nphud, disk(dilation_radius))
    #     nphud3 = nphud3[crop2[0,0]:crop2[0,1]+1, crop2[1,0]:crop2[1,1]+1]
    npmed = skimage.filters.median(npimg, disk(median_filter_radius))
    npnorm = npimg.copy()
    npnorm[nphud3] = npmed[nphud3]

    return npnorm, npmasked


def preprocess_image(npimg, version=None):
    '''
    Calculate physical spacing of image from identified ruler within image and crops to B-mode only.
    
    Parameters
    ----------
    npimg : ndararay
        MxNx3
    version : None
        reserved for future use
    
    Returns
    -------
    itkImage[itk.F,2]
        cropped image scaled to 0.0 to 1.0 (MxN) with physical spacing
    '''
    spacing, crop = _find_spacing_and_crop(npimg)
    npimgcrop = npimg[crop[0,0]:crop[0,1]+1, crop[1,0]:crop[1,1]+1]
    npimgnorm, _ = _normalize(npimgcrop[:,:,0], npimgcrop)
    img = itk.image_from_array(npimgnorm)
    img.SetSpacing([spacing, spacing])
    return img, { 'spacing' : [spacing, spacing], 'crop' : crop }

def preprocess_video(npvid, framerate=1, version=None):
    '''
    npvid : ndarray
        video TxMxNx3
    framerate : float
        framerate (e.g. extracted from ffprobe)
    version : None
        reserved for future use
    '''
    npmean = np.mean(npvid, axis=0)
    spacing, crop = _find_spacing_and_crop(npmean)
    vid = itk.image_from_array((npvid[:,crop[0,0]:crop[0,1]+1, crop[1,0]:crop[1,1]+1,0] / 255.0).astype('float32').squeeze())
    vid.SetSpacing([spacing, spacing, framerate])
    
    return vid, { 'spacing' : [spacing, spacing, framerate], 'crop' : crop }
    
def load_and_preprocess_image(fp, version=None):
    '''
    Loads and preprocesses a Interson image.
    
    Parameters
    ----------
    fp : str
        filepath to image
    version : None
        Reserved for future use.
    
    Returns
    -------
    img : itk.Image[itk.F,2]
        Floating point image with physical spacing set, origin is at upper-left of image, and intensities are between 0.0 and 1.0
    meta : dict
        Meta data (includes spacing and crop)
    '''
    return preprocess_image(itk.imread(fp))



class ProbeSettings:
    '''
    Struct-like class for handling probe settings meta data from the Interson .cine format.
    '''
    
    FORMAT_STR = 'iifiiiiif'
    
    def __init__(self, depth, frequency_idx, frequency, near_gain, mid_gain, far_gain, contrast, intensity, voltage):
        '''
        Order of parameters matters here as is used by unpack.  Do not change these parameters.
        '''
        self.depth = depth
        self.frequency_idx = frequency_idx
        self.frequency = frequency
        self.near_gain = near_gain
        self.mid_gain = mid_gain
        self.far_gain = far_gain
        self.contrast = contrast
        self.intensity = intensity
        self.voltage = voltage
        
    def __str__(self):
        return  str(self.__class__) + '\n' + '\n'.join((str(item) + ' = ' + str(self.__dict__[item]) for item in sorted(self.__dict__)))    
    
class DownloadSettings:
    '''
    Struct-like class for handling probe settings meta data from the Interson .cine format.
    '''
    
    FORMAT_STR = 'i???' + ProbeSettings.FORMAT_STR + 'i??ii?ifi??ihi?ii'
    def __init__(self, probe_id, bidir, rf_probe, b360_probe, depth, frequency_idx, frequency, near_gain, mid_gain, far_gain, contrast, intensity, voltage, y_display_offset, img_left_orientation, img_top_orientation, image_rotation, frame_scan_rate, averaging, image_sector, scan_radius, compound_angle, compound_enabled, doubler_enabled, steering_angle, cine_compression_ratio, number_of_cine_frames, cine_cfm_mode, cfm_vectors, cfm_samples):
        '''
        Order of parameters matters here as is used by unpack.  Do not change these parameters.
        '''
        self.probe_id = probe_id
        self.bidir = bidir
        self.rf_probe = rf_probe
        self.b360_probe = b360_probe
        self.probe_settings = ProbeSettings(depth, frequency_idx, frequency, near_gain, mid_gain, far_gain, contrast, intensity, voltage)
        self.y_display_offset = y_display_offset
        self.img_left_orientation = img_left_orientation
        self.img_top_orientation = img_top_orientation
        self.image_rotation = image_rotation
        self.frame_scan_rate = frame_scan_rate
        self.averaging = averaging
        self.image_sector = image_sector
        self.scan_radius = scan_radius
        self.compound_angle = compound_angle
        self.compound_enabled = compound_enabled
        self.doubler_enabled = doubler_enabled
        self.steering_angle = steering_angle
        self.cine_compression_ratio = cine_compression_ratio
        self.number_of_cine_frames = number_of_cine_frames
        self.cine_cfm_mode = cine_cfm_mode
        self.cmf_vectors = cfm_vectors
        self.cfm_samples = cfm_samples
    def __str__(self):
        return  str(self.__class__) + '\n' + '\n'.join((str(item) + ' = ' + str(self.__dict__[item]) for item in sorted(self.__dict__)))
    
def _load_video(fp):
    '''
    Loads an Interson .cine file.
    
    Parameters
    ----------
    fp : str
    
    Returns
    -------
    version : int
    download_settings : DownloadSettings
        Contains .cine file meta data
    tmp_buffer : bytearray
    length : int
        Length of cine_buffer
    cine_buffer : bytearray
    cfm_cine_length : int
        Length of cfm_cine_length
    cfm_cine_buffer : bytearray
    '''
    with open(fp, mode='rb') as f:
        version = unpack('i', f.read(calcsize('i')))
        download_settings = DownloadSettings(*unpack(DownloadSettings.FORMAT_STR, f.read(calcsize(DownloadSettings.FORMAT_STR))))
        # magic number from interson
        tmp_buffer = f.read(1164)
        length = 127 * 1024 * download_settings.number_of_cine_frames
        cine_buffer = bytearray(f.read(length))
        
        cfm_cine_length = None
        cfm_cine_buffer = None
        if download_settings.cine_cfm_mode:
            cfm_cine_length = download_settings.number_of_cine_frames * download_settings.cfm_vectors * download_settings.cfm_samples
            cfm_cine_buffer = f.read(cfm_cine_length)
    
    return version, download_settings, tmp_buffer, length, cine_buffer, cfm_cine_length, cfm_cine_buffer

def _convert_to_image(cine_buffer, number_of_cine_frames, depth, frame_scan_rate):
    '''
    Converts cine_buffer into an itk.Image that has been resampled to be isometric
    
    Parameters
    ----------
    cine_buffer : bytearray
        From _load_video
    number_of_cine_frames : int
        Number of video frames, i.e., download_setttings.number_of_cine_frames
    depth : int
        Image depth in mm, i.e., download_settings.probe_settings.depth
    frame_scan_rate : int
        Frames per second of video, i.e., download_settings.frame_scan_rate
        
    Returns
    -------
    itk.Image[itk.D,3]
        Original frame count and y pixel size preserved, x is resized to be isometric
        and there is cropping in the x dimension.
    '''
    # hard-coded x/y dims from interson
    size = [127, 1024, number_of_cine_frames]
    img = itk.Image[itk.UC, 3].New()
    img.SetRegions(itk.ImageRegion[3](size))
    img.Allocate()
    # 38mm for probe head size
    spacing = [38.0/size[0], depth/size[1], 1.0/frame_scan_rate]
    img.SetSpacing(spacing)
    p = 0
    idx = itk.Index[3]()
    idx_map = [2, 0, 1]
    for i in range(size[idx_map[0]]):
        idx.SetElement(idx_map[0], i)
        for j in range(size[idx_map[1]]):
            idx.SetElement(idx_map[1], j)
            for k in range(size[idx_map[2]]):
                idx.SetElement(idx_map[2], k)
                img.SetPixel(idx, cine_buffer[p])
                p += 1
    
    output_size = size
    output_size[0] = spacing[0] / spacing[1] * 112
    
    output_spacing = [spacing[1], spacing[1], spacing[2]]
    
    s = img.GetLargestPossibleRegion().GetSize()
    for i in range(len(s)):
        s[i] = int(output_size[i])
    
    # use this to crop the data
    # the ultrasound data doesn't fill the buffer
    translate_transform = itk.TranslationTransform[itk.D,3].New()
    p = translate_transform.GetParameters()
    p[0] = 0.5 # probe reading doesn't start until 0.5mm into the buffer
    translate_transform.SetParameters(p)

    interpolator = itk.LinearInterpolateImageFunction.New(img)
    resampled = itk.resample_image_filter(
        img,
        transform=translate_transform,
        interpolator=interpolator,
        size=s,
        output_spacing=np.array(output_spacing),
    )
    
    return resampled

def load_and_preprocess_video(fp, version=None):
    '''
    Loads and preprocesses a Interson video.
    
    Parameters
    ----------
    fp : str
        filepath to video (e.g. .mp4, .cine)
    version : optional
        Reserved for future use.
    
    Returns
    -------
    img : itk.Image[itk.F,3]
        Floating point image with physical spacing set (3rd dimension is framerate), origin is at upper-left of image, and intensities are between 0.0 and 1.0
    meta : dict
        Meta data (includes spacing and crop)
    '''
    if fp.endswith('.cine'): # interson .cine proprietary format
        version, download_settings, tmp_buffer, length, cine_buffer, cfm_cine_length, cfm_cine_buffer = _load_video(fp)
        img = _convert_to_image(cine_buffer, download_settings.number_of_cine_frames, download_settings.probe_settings.depth, download_settings.frame_scan_rate)
        size = img.GetLargestPossibleRegion().GetSize()
        meta_dict = { 
            'spacing' : np.array([
                img.GetSpacing()[0],
                img.GetSpacing()[1],
                img.GetSpacing()[2]]),
            'crop' : np.array([[0, 0], [size[0], size[1]]]) # there's no cropping (per se) as the .cine is just the raw data
        }
        return img, meta_dict
    else: # .mp4
        return preprocess_video(skvideo.io.vread(fp), framerate=get_framerate(skvideo.io.ffprobe(fp)))