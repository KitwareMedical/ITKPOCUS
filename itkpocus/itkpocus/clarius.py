import numpy as np
import skvideo
import skvideo.io
from skimage.transform import SimilarityTransform
from scipy.signal import find_peaks
from itkpocus.util import get_framerate
import itk

'''
Preprocessing and device-specific IO for the Clarius HD C7.
'''

def find_spacing_and_crop(npimg):
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
    col_peaks, col_props = find_peaks(colsum, prominence=20)
    ruler_col = col_peaks[-1]
    ruler_peaks, ruler_props = find_peaks(npimg[:,ruler_col,0])
    # TODO: stress test this with max zoomed video, not sure if the ruler would switch to 0.5 cm spacing or < 4 circles would be on the video
    spacing = 10.0 / np.max(ruler_peaks[1:] - ruler_peaks[0:len(ruler_peaks)-1]) # 10 mm per ruler spacing, we get the max peak distance because half-circles on the top and bottom shift the peaks
    
    # find left/right bounds
    # just going assume the ultrasound is in the middle of the image and work outwards to find the letterboxing
    midcol = npimg.shape[1]/2.0 # not worried if non-integer
    zerocol = np.argwhere(colsum == 0)
    leftbound = np.max(zerocol[zerocol < midcol])+1
    rightbound = np.min(zerocol[midcol < zerocol])-1
    
    midrow = npimg.shape[0]/2.0
    rowsum = np.sum(npimg[:,leftbound:rightbound+1,0], axis=1)
    zerorow = np.argwhere(rowsum == 0)
    topbound = np.max(zerorow[zerorow < midrow])
    bottombound = np.min(zerorow[midrow < zerorow])
    
    return spacing, np.array([[topbound, bottombound], [leftbound, rightbound]])

def preprocess_image(npimg):
    '''
    Calculate physical spacing of image from identified ruler within image and crops to B-mode only.
    
    Parameters
    ----------
    npimg : ndararay
        MxNx3
    
    Returns
    -------
    itkImage[itk.F,2]
        cropped image scaled to 0.0 to 1.0 (MxN) with physical spacing
    '''
    spacing, crop = find_spacing_and_crop(npimg)
    img = itk.image_from_array((npimg[crop[0,0]:crop[0,1]+1, crop[1,0]:crop[1,1]+1, 0] / 255.0).astype('float32'))
    img.SetSpacing([spacing, spacing])
    return img, { 'spacing' : [spacing, spacing], 'crop' : crop }

def preprocess_video(npvid, framerate=1):
    '''
    npvid : ndarray
        video TxMxNx3
    framerate : float
        framerate (e.g. extracted from ffprobe)
    '''
    npmean = np.mean(npvid, axis=0)
    spacing, crop = find_spacing_and_crop(npmean)
    vid = itk.image_from_array((npvid[:,crop[0,0]:crop[0,1]+1, crop[1,0]:crop[1,1]+1,0] / 255.0).astype('float32').squeeze())
    vid.SetSpacing([spacing, spacing, framerate])
    
    return vid, { 'spacing' : [spacing, spacing, framerate], 'crop' : crop }
    
def load_and_preprocess_image(fp):
    '''
    Loads and preprocesses a Clarius image.
    
    Parameters
    ----------
    fp : str
        filepath to image
    version : optional
        Reserved for future use.
    
    Returns
    -------
    img : itk.Image[itk.F,2]
        Floating point image with physical spacing set, origin is at upper-left of image, and intensities are between 0.0 and 1.0
    meta : dict
        Meta data (includes spacing and crop)
    '''
    return preprocess_image(itk.imread(fp))

def load_and_preprocess_video(fp):
    '''
    Loads and preprocesses a Clarius video.
    
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
    return preprocess_video(skvideo.io.vread(fp))