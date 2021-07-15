import skvideo.io.ffprobe
import skvideo.utils
import sys
from skvideo.io import vread
from scipy.signal import find_peaks
from pathlib import Path
import numpy as np
import itkpocus.util
import itk

'''
Preprocessing and device specific IO for the Butterfly iQ.
'''

def ffprobe_count_frames(filename):
    """
    Get metadata by using ffprobe

    Checks the output of ffprobe on the desired video file. MetaData is then parsed into a dictionary.

    Parameters
    ----------
    filename : string
        Path to the video file

    Returns
    -------
    metaDict : dict
       Dictionary containing all header-based information 
       about the passed-in source video.

    """
    # check if FFMPEG exists in the path
    try:
        command = [skvideo._FFMPEG_PATH + "/" + skvideo._FFPROBE_APPLICATION, "-v", "error", "-show_streams", "-count_frames", "-print_format", "xml", filename]
        #print(command)
        # simply get std output
        xml = skvideo.utils.check_output(command)
        #print(xml)
        d = skvideo.utils.xmltodictparser(xml)["ffprobe"]

        d = d["streams"]

        #import json
        #print json.dumps(d, indent = 4)
        #exit(0)

        # check type
        streamsbytype = {}
        if type(d["stream"]) is list:
            # go through streams
            for stream in d["stream"]:
                streamsbytype[stream["@codec_type"].lower()] = stream
        else:
            streamsbytype[d["stream"]["@codec_type"].lower()] = d["stream"]

        return streamsbytype
    except AttibuteError as e:
        print(e)
        return {}
    except:
        print("Unexpected error:", sys.exc_info()[0])
        return {}

def vread_workaround(fp, ffprobe_count_frames_dict=None):
    '''
    A workaround for Butterfly .mp4 files that don't correctly set the number of frames.  Forces vread to use the @nb_read_frames as the frame count.
    
    Parameters
    ----------
    fp : str
        File path to video file to read
    ffprobe_count_frames_dict : dict, optional
        A dict from ffprobe containing the key '@nb_read_frames'
        
    Returns
    -------
    ndarray
        Video array in F,H,W,RGB format
    '''
    ffprobe_count_frames_dict = ffprobe_count_frames_dict if ffprobe_count_frames_dict is not None else ffprobe_count_frames(fp)
    return vread(fp, outputdict={'-vframes' : ffprobe_count_frames_dict['video']['@nb_read_frames']})

def calc_spacing(npvid):
    '''
    Calculates the physical spacing of an image or video by interpeting the ruler overlay.
    
    Parameters
    ----------
    npvid : ndarray
        video or image, if image, H, W, RGB, if video, F, H, W, RGB
        
    Returns
    -------
    float
        
    '''
    if len(npvid.shape) == 4: # it's a video
        npimg = npvid[0,:,:,:].squeeze()
    else: # it's an image
        npimg = npvid

    ruler_peaks, ruler_props = find_peaks(npimg[:,-4,0].squeeze(), threshold=10, distance=10)
    ruler_dist = np.median(ruler_peaks[1:] - ruler_peaks[:-1])
    spacing = 2 / ruler_dist
    return spacing

def calc_crop(npvid):
    '''
    Calculates the crop region to isolate the ultrasound data.
    
    Parameters
    ----------
    npvid : ndarray
        video or image, if image, H, W, RGB, if video, F, H, W, RGB
        
    Returns
    -------
    ndarray
        2x2 array in format [[start_row, end_row], [start_column, end_column]]
    '''
    if len(npvid.shape) == 4:
        npimg = npvid[0,:,:,0].squeeze()
    else:
        npimg = npvid[:,:,0].squeeze()
    
    # center of image
    cr, cc = (np.array(npimg.shape) / 2.0).astype('int')
    
    sr = int(0.05 * npimg.shape[0])
    er = cr

    while er < npimg.shape[0]-1 and len(np.argwhere(npimg[er,:])) > npimg.shape[1] * 0.25:
        er += 1
    
    er -= 15 # get rid of angling shadowbox on butterfly
    
    sc = cc
    ec = cc
    col_baseline = len(np.argwhere(npimg[:, cc]))
    while sc > 0 and len(np.argwhere(npimg[sr:er, sc])) > npimg.shape[0] * 0.25:
        sc -= 1
    while ec < npimg.shape[1]-1 and len(np.argwhere(npimg[sr:er,ec])) > npimg.shape[0] * 0.25:
        ec += 1
    
    sc += 15 # get rid of angling shadowbox on butterfly
    ec -= 15 # get rid of angling shadowbox on butterfly
    
    return np.array([[sr, er], [sc, ec]], dtype='int')

def preprocess_video(npvid, framerate=1):
    '''
    Preprocesses an ndarray representing a video (FRCRGB)
    
    Parameters
    ----------
    npvid : ndarray
        FxRxCx3 (rgb), 0 to 255
    
    Returns
    -------
    img : itk.Image[itk.F,3]
    meta : dict
    '''
    spacing = calc_spacing(npvid)
    spacing = np.array([spacing, spacing, framerate])
    crop = calc_crop(npvid)
    img = itkpocus.util.image_from_array(npvid[:,crop[0,0]:crop[0,1], crop[1,0]:crop[1,1],0].astype('float32') / 255.0, spacing=spacing)
    return img, {'spacing' : spacing, 'crop' : crop}

def preprocess_image(npimg):
    '''
    
    Parameters
    ----------
    npimg : ndarray
        RxCx3 (rgb), 0 to 255
    
    Returns
    -------
    img : itk.Image[itk.F,2]
    meta : dict
    '''
    spacing = calc_spacing(npimg)
    spacing = np.array([spacing, spacing])
    crop = calc_crop(npimg)
    img = itkpocus.util.image_from_array(npimg[crop[0,0]:crop[0,1], crop[1,0]:crop[1,1],0].astype('float32') / 255.0, spacing=spacing)
    return img, {'spacing' : spacing, 'crop' : crop}

def load_and_preprocess_video(fp, version=None):
    '''
    Loads and preprocesses a Butterfly video.
    
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
    vidmeta = ffprobe_count_frames(fp)
    npvid_rgb = vread_workaround(fp, vidmeta)
    return preprocess_video(npvid_rgb, framerate=get_framerate(vidmeta))

def load_and_preprocess_image(fp, version=None):
    '''
    Loads and preprocesses a Butterfly image.
    
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
    npimg_rgb = itk.array_from_image(itk.imread(fp))
    return preprocess_image(npimg_rgb)
    