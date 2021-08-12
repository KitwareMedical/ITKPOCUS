import itk
from scipy.signal import find_peaks
from skimage.morphology import disk
from skimage.morphology import dilation
from skimage.feature import match_template
import skvideo.io
from itkpocus.util import get_framerate
import skimage.filters
import numpy as np
import os

def load_and_preprocess_video(fp, version=None):
    '''
    Loads and preprocesses a Sonosite video.
    
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
    
    npvid_rgb = skvideo.io.vread(fp)
    return preprocess_video(npvid_rgb, framerate=get_framerate(skvideo.io.ffprobe(fp)))

def load_and_preprocess_image(fp, version=None):
    '''
    Loads and preprocesses a Sonosite image.
    
    Parameters
    ----------
    fp : str
        filepath to image
    version : optional
        Reserved for future use.
    
    Returns
    -------
    img : itk.Image[itk.F,2]
        Floating point image with physical spacing set (3rd dimension is framerate), origin is at upper-left of image, and intensities are between 0.0 and 1.0
    meta : dict
        Meta data (includes spacing and crop)
    '''
    npimg_rgb = itk.array_from_image(itk.imread(fp))
    return preprocess_image(npimg_rgb)

def _find_spacing(npimg):
    '''
    Finds the white ticks on the right of the image and calculates their spacing.
    
    Parameters
    -----------
    npimg (numpy array): 0-255 numpy array MxN
    
    Returns
    -------
    
    spacing : float
        mm per pixel
    '''
    rulerpos = [npimg.shape[1]-6, npimg.shape[1]]
    rulerimg = npimg[:,rulerpos[0]:rulerpos[1]].copy()

    ruler1d = np.max(rulerimg, axis=1)
    peaks, properties = find_peaks(ruler1d, height=[240,255], distance=5)
    rulerdiff = peaks[1:-1] - peaks[:-2]
    if np.std(rulerdiff) >= 2.0:
        raise ValueError("Error detecting ruler, peaks not equally-spaced {}".format(peaks))
    pixelsize = 5.0 / np.mean(rulerdiff) # in mm, 0.5 cm ruler breaks

    return pixelsize

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

    # try to detect hud elements by their color (ultrasound should be grey) and pure whiteness
    npcolor = np.logical_not(np.logical_and(npimgrgb[:,:,0] == npimgrgb[:,:,1], npimgrgb[:,:,1] == npimgrgb[:,:,2]))
    npwhite = npimg > 235
    npblack = npimg < 10 # tries to get rid of black noise
    nphud = np.logical_or(npcolor, npwhite, npblack)
    nphud2 = dilation(nphud, disk(4))
    npmasked = npimg.copy()
    npmasked[nphud2] = 0

    # now we have a cropped image, need to get rid of the annotation marks
    nphud3 = dilation(nphud, disk(2))

    npmed = skimage.filters.median(npimg, disk(4))
    npnorm = npimg.copy()
    npnorm[nphud3] = npmed[nphud3]
    
    return npnorm, npmasked


def _crop_image(npimg, crop):
    '''
    Crops an ndarray.
    
    Parameters
    ----------
    npimg : ndarray
        MxN
    crop : ndarray 
        2x2 [[ymin, ymax], [xmin, xmax]] where y=row and x=column
    
    Returns
    -------
    ndarray
    '''
    return npimg[crop[0,0]:crop[0,1]+1, crop[1,0]:crop[1,1]+1]

def _find_crop(npnorm, npmasked):
    '''
    A bunch of funkiness with removing the SonoSite overlay.  Overlay elements can overlap the ultrasound image
    and jpeg compression can add noise.  This works by a fixed crop in the y-direction and then a search on the 
    overlay-removed image to find the x-direction boundaries.
    
    Returns
    -------
    ndarray
        2x2 [[ymin, ymax], [xmin, xmax]] where y=row, x=column
    '''
    
    def array_crop(arr, threshold):
        c1 = 0
        c2 = len(arr)-1
        while (c1 <= c2 and arr[c1] < threshold):
            c1 += 1
        while (c2 > c1 and arr[c2] < threshold):
            c2 -= 1
        return np.array([c1, c2])
    
    # oy, hard-code cropping, then looking for image intensity in the x-axis
    # zooming seems to stretch the image in x, but y is always maxed
    # confounding is grey-scale overlay elements and jpeg compression artifacts
    crop1 = np.array([[50, 620], [180, 830]])
    npcrop1 = _crop_image(npmasked, crop1)
    xintensity = np.sum(npcrop1, axis=0)
    yintensity = np.sum(npcrop1, axis=1)
    xcrop = array_crop(xintensity, 2000)
    crop2 = np.array([crop1[0,:], [crop1[1,0] + xcrop[0], crop1[1,1]+xcrop[1]-npcrop1.shape[1]]])
    
    return crop2

def preprocess_image(npimg, version=None):
    '''
    Loads and preprocesses a Sonosite image.
    
    Parameters
    ----------
    npimg : ndarray
        MxNx3 image array
    version : optional
        Reserved for future use.

    Returns
    -------
    img : itk.Image[itk.F,2]
        Floating point image with physical spacing set, origin is at upper-left of image, and intensities are between 0.0 and 1.0
    meta : dict
        Meta data (includes spacing and crop)
    '''

    npimg_bw = npimg[:,:,0].squeeze()
    
    spacing = _find_spacing(npimg_bw)
    npnorm, npmasked = _normalize(npimg_bw, npimg)
    crop = _find_crop(npnorm, npmasked)
    npnormcrop = _crop_image(npnorm, crop) # this is the us image for analysis
    
    img = itk.image_from_array((npnormcrop / 255.0).astype('float32'))
    img.SetSpacing(np.array([spacing, spacing]))
    return img, {'spacing' : np.array([spacing, spacing]), 'crop' : crop}

def preprocess_video(npvid, framerate=1, version=None):
    '''
    Loads and preprocesses a Sonosite video.
    
    Parameters
    ----------
    ndarray : ndarray
        FxMxNxRGB 
    version : optional
        Reserved for future use.
    
    Returns
    -------
    img : itk.Image[itk.F,3]
        Floating point image with physical spacing set (3rd dimension is framerate), origin is at upper-left of image, and intensities are between 0.0 and 1.0
    meta : dict
        Meta data (includes spacing and crop)
    '''
    frm_cnt = npvid.shape[0]
    frm1rgb = npvid[0,:,:,:].squeeze()
    frm1 = frm1rgb[:,:,0].squeeze()
    spacing = _find_spacing(frm1)
    tmpnorm, tmpmasked = _normalize(frm1, frm1rgb)
    crop = _find_crop(tmpnorm, tmpmasked)
    tmpnormcrop = _crop_image(tmpnorm, crop)
    npnormvid = np.zeros([frm_cnt, tmpnormcrop.shape[0], tmpnormcrop.shape[1]])
    
    for i in range(frm_cnt):
        frmrgb = npvid[i,:,:,:].squeeze()
        frm = frmrgb[:,:,0].squeeze() # just pick a channel for greyscale
        frmnorm, frmmasked = _normalize(frm, frmrgb)
        npnormvid[i,:,:] = _crop_image(frmnorm, crop)
    
    img = itk.image_from_array((npnormvid / 255.0).astype('float32'))
    tmp = np.array([spacing, spacing, framerate])
    img.SetSpacing(tmp)
    return (npnormvid / 255.0).astype('float32'), {'spacing' : tmp, 'crop' : crop}
