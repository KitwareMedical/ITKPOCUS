import numpy as np
import skvideo
import skvideo.io
from skimage.transform import SimilarityTransform
from skimage.restoration import inpaint
from skimage.morphology import dilation, disk
from scipy.signal import find_peaks
from itkpocus.util import get_framerate
import itk

'''
Preprocessing and device-specific IO for the Clarius HD C7.
'''

def _get_rightmost_ruler_from_mask(mask):
    '''
    Finds the ruler as long as it is the rightmost thing on the mask.
    
    Parameters
    ----------
    mask : ndarray
        MxN where the ruler and other overlay elements are non-zero values
        
    Returns
    -------
    spacing : float
    ruler_col : int
    ruler_peaks : ndarray of int
    '''
    col_prominence=20 # strength of ruler signal column-wise
    row_prominence=1 # strength of each ruler tick row-wise
    row_width=5
    consistency_threshold = 10 # max number of pixels a ruler step can differ in
    outlier_threshold = 0.1
    tick_dist = 10.0 # physical distance between ticks in mm
    
    colsum = np.sum(mask, axis=0)
    col_peaks, col_props = find_peaks(colsum, prominence=col_prominence)
    ruler_col = col_peaks[-1]
    ruler_peaks, ruler_props = find_peaks(mask[:,ruler_col], prominence=row_prominence, width=row_width)
    
    diffs = np.abs(ruler_peaks[1:] - ruler_peaks[:-1])
    median_diff = np.median(diffs)
    outliers_perc =  len(np.argwhere(diffs - median_diff > consistency_threshold)) / len(diffs)
    if outliers_perc > outlier_threshold:
        raise ValueError('Max percentage of outlier ruler ticks greater than threshold (ruler likely not detected or obscured): {} > {}'.format(outliers_perc, outlier_threshold))
    
    # TODO: stress test this with max zoomed video, not sure if the ruler would switch to 0.5 cm spacing or < 4 circles would be on the video
    spacing = tick_dist / median_diff
    
    return spacing, ruler_col, ruler_peaks

def _inpaint(npimg, mask):
    '''
    Inpaint the locations in npimg corresponding to true values in mask.
    
    Parameters
    ----------
    npimg : ndarray
        MxN
    mask : ndarray
        MxN
        
    Returns
    -------
    ndarray
    '''
    return inpaint.inpaint_biharmonic(npimg, mask)

def _find_crop(npimg_clean):
    '''
    Parameters
    ----------
    npimg_clean : ndarray
        MxN with overlay elements removed/inpainted, intensities between 0 and 1.0
    
    Returns
    -------
    ndarray
        [[toprow, bottomrow], [leftcol, rightcol]]
    '''
    
    bg_cutoff = 10.0 / 255.0
    col_cutoff_perc = 0.1
    row_cutoff_perc = 0.1
    
    non_bg = npimg_clean > bg_cutoff
    
    colsum = np.sum(non_bg, axis=0)
    col_cutoff = col_cutoff_perc * np.max(colsum)
    
    fgcol = np.argwhere(colsum > col_cutoff)
    leftbound = np.min(fgcol)
    rightbound = np.max(fgcol)
    
    rowsum = np.sum(non_bg[:,leftbound:rightbound+1], axis=1)
    row_cutoff = row_cutoff_perc * np.max(rowsum)
    fgrow = np.argwhere(rowsum > row_cutoff)
    
    topbound = np.min(fgrow)
    bottombound = np.max(fgrow)
    
    return np.array([[topbound, bottombound], [leftbound, rightbound]])

def _get_overlay_mask(npimg, version=None, threshold=250):
    '''
    Identify the overlay and annotation elements by brightness (threshold) and color.
    
    Parameters
    ----------
    npimg : ndarray
        MxNx3
    version : None
        reserved for future use
    threshold : int
        brightness threshold for overlay elements, if too low, B-mode data may mistakenly masked
        
    Returns
    -------
    ndarray
        False is background and True is overlay
    '''
    mask = np.logical_not((npimg[:,:,0] == npimg[:,:,1]) & (npimg[:,:,1] == npimg[:,:,2])) | (npimg[:,:,0] >= threshold)
    mask = dilation(mask, disk(5))
    return mask

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
    mask = _get_overlay_mask(npimg)
    npimg_clean = _inpaint(npimg[:,:,0], mask)
    crop = _find_crop(npimg_clean)
    spacing, ruler_col, ruler_peaks = _get_rightmost_ruler_from_mask(mask)
    img = itk.image_from_array(npimg_clean[crop[0,0]:crop[0,1]+1, crop[1,0]:crop[1,1]+1])
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
    
    mask = _get_overlay_mask(npmean)
    spacing, ruler_col, ruler_peaks = _get_rightmost_ruler_from_mask(mask)
    
    npmean_clean = _inpaint(npmean[:,:,0], mask)
    crop = _find_crop(npmean_clean)
    mask_crop = mask[crop[0,0]:crop[0,1]+1, crop[1,0]:crop[1,1]+1]
    
    npvid2 = npvid[:,crop[0,0]:crop[0,1]+1, crop[1,0]:crop[1,1]+1,0] / 255.0
    for j in range(npvid2.shape[0]):
        npvid2[j] = _inpaint(npvid2[j], mask_crop)
        
    vid = itk.image_from_array(npvid2)
    vid.SetSpacing([spacing, spacing, framerate])
    
    return vid, { 'spacing' : [spacing, spacing, framerate], 'crop' : crop }
    
def load_and_preprocess_image(fp, version=None):
    '''
    Loads and preprocesses a Clarius image.
    
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
    return preprocess_video(skvideo.io.vread(fp), framerate=get_framerate(skvideo.io.ffprobe(fp)))