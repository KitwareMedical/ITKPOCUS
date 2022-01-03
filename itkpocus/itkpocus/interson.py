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
    
    if len(zerorow) == 0: # fills the whole image vertically
        topbound=0
        bottombound=npimg.shape[0]-1
    else:
        topbound = np.max(zerorow[zerorow < midrow])
        bottombound = np.min(zerorow[midrow < zerorow])
    
    
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

def load_and_preprocess_video(fp, version=None):
    '''
    Loads and preprocesses a Interson video.
    
    Parameters
    ----------
    fp : str
        filepath to video (e.g. .mp4)
    version : optional
        Reserved for future use.
    
    Returns
    -------
    img : itk.Image[itk.F,3]
        Floating point image with physical spacing set (3rd dimension is framerate), origin is at upper-left of image, and intensities are between 0.0 and 1.0
    meta : dict
        Meta data (includes spacing and crop)
    '''
    return preprocess_video(skvideo.io.vread(fp), framerate=get_framerate(skvideo.io.ffprobe(fp)))