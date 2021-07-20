import itk
import pydicom
import numpy as np
from scipy.signal import find_peaks
from skimage.morphology import disk
from skimage.morphology import dilation
import skimage.filters
import itkpocus.util
import skvideo.io

'''Preprocessing and device-specific IO for the Sonoque.'''

def _find_spacing(npimg):
    '''
    Finds the spacing (pixel dimension in mm) of a Sonoque image by detecting the ruler ticks of the overlay.
    
    Parameters
    ----------
    npimg : ndarray
        single channel 0 to 255 (e.g. pydicom's pixel_array or a video frame)
    
    Returns
    -------
    spacing : float
        or None if the ruler cannot be detected
    '''
    tick_spacing = 5 # in mm
    error_threshold = 5 # in pixels
    ruler_size_threshold = 0.7 # percentage of vertical image
    ticks_offset = 3 # in pixels, right of long ruler line
    ruler_intensity_threshold = 80 # minimum brightness of ruler pixels/height in peak finding
    
    ruler_thresh = npimg.shape[0] * ruler_size_threshold
    col_count = np.sum(npimg > 0, axis=0)
    ruler_col = np.argwhere(col_count > ruler_thresh)[0] + ticks_offset
    ruler_ticks, _ = find_peaks(npimg[:, ruler_col].flatten(), height=ruler_intensity_threshold)
    ruler_diffs = ruler_ticks[1:] - ruler_ticks[:-1]
    
    if (np.min(ruler_diffs) - np.max(ruler_diffs)) > error_threshold :
        return None # bad

    spacing = tick_spacing / np.mean(ruler_diffs)
    return spacing

def _find_crop(npimg):
    '''
    Calculates a crop that contains only the ultrasound portion of the image (overlay text may still be on portion).
    
    Parameters
    ----------
    npimg : ndarray
        single channel 0 to 255 (e.g. pydicom's pixel_array or a video frame)
    
    Returns
    -------
    crop : ndarray
        (2x2 ndarray) [[topbound, bottombound], [leftbound, rightbound]]
    '''
    nonempty_threshold = 0.1 # percentage of nonzero pixels
    background_threshold = 10 # pixel intensity
    stuff_cols = np.sum(npimg > background_threshold, axis=0) > nonempty_threshold * npimg.shape[0]
    
    height_min_crop = 0.05
    width_min_crop = 0.19 # currently unused
    
    
    midcol = npimg.shape[1]/2.0 # not worried if non-integer
    zerocol = np.argwhere(stuff_cols == 0)
    leftbound = np.max( (np.max(zerocol[zerocol < midcol])+1, int(npimg.shape[1] * width_min_crop) ))
    rightbound = np.min( (np.min(zerocol[midcol < zerocol])-1, int(npimg.shape[1] * (1 - width_min_crop) )))
    
    midrow = npimg.shape[0]/2.0
    rowsum = np.sum(npimg[:,leftbound:rightbound+1], axis=1)
    zerorow = np.argwhere(rowsum == 0)
    topbound = np.max( (np.max(zerorow[zerorow < midrow]), int(height_min_crop * npimg.shape[0])) )
    bottombound = np.min( (np.min(zerorow[midrow < zerorow]), int(npimg.shape[0] * (1 - height_min_crop))) )
    
    return np.array([[topbound, bottombound], [leftbound, rightbound]])


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

def load_and_preprocess_image(fp, version=None):
    '''
    Loads Sonoque .dcm image.  Crops to ultrasound data (e.g. removes rulers) and uses a masked median filter to remove any overlayed text (by masking out bright white).
    
    Returns and itk.Image[itk.F,2] with intensities 0 to 1.0 and correct physical spacing.
    
    Parameters
    ----------
    fp : str
        filepath
    version : None
        reserved for future use
    
    Returns
    -------
    img : itk.Image[it.F,2]
    meta : dict
        Meta dictionary
    '''
    tmp = pydicom.dcmread(fp)
    spacing = ( tmp[0x0018, 0x6011][0][0x0018, 0x602c].value, tmp[0x0018, 0x6011][0][0x0018, 0x602e].value ) # expecting single item ultrasound series, get physicalX and Y tags
    npimg_rgb = tmp.pixel_array
    npimg = tmp.pixel_array[:,:,0]
    crop = _find_crop(npimg)
    npnorm, _ = _normalize(itkpocus.util.crop(npimg, crop), itkpocus.util.crop(npimg_rgb, crop, rgb=True))
    img = itk.image_from_array(npnorm / 255.0)
    img.SetSpacing(spacing)

    return img, {'spacing' : spacing, 'crop' : crop}

def load_and_preprocess_video(fp, version=None):
    '''
    Loads Sonoque .mov video.  Crops to ultrasound data (e.g. removes rulers) and uses a masked median filter to remove any overlayed text (by masking out bright white).
    
    Returns and itk.Image[itk.F,3] with intensities 0 to 1.0 and correct physical spacing.
    
    Parameters
    ----------
    fp : str 
        filepath
    version : None
        reserved for future use
    
    Returns
    -------
    img : itk.Image[itk.F,2]
    meta : dict
        Meta data dictionary
    '''
    npvid_rgb = skvideo.io.vread(fp)
    npvid = npvid_rgb[:,:,:,0]
    vidmeta = skvideo.io.ffprobe(fp)
    
    npfirst = npvid[0,:,:]
    spacing = _find_spacing(npfirst)
    spacing = [spacing, spacing, itkpocus.util.get_framerate(vidmeta)]
    crop = _find_crop(npfirst)
    
    npvid_rgb = itkpocus.util.crop(npvid_rgb, crop, rgb=True)
    npvid = itkpocus.util.crop(npvid, crop)
    for i in range(npvid.shape[0]):
        npvid[i,:,:], _ = _normalize(npvid[i,:,:], npvid_rgb[i,:,:])
        
    img = itk.image_from_array(npvid / 255.0)
    img.SetSpacing(spacing)

    return img, { 'spacing' : spacing, 'crop' : crop }