import skvideo.io.ffprobe
import skvideo.utils
import sys
from skvideo.io import vread
from scipy.signal import find_peaks
from pathlib import Path
import numpy as np
import tbitk.util

'''
Preprocessing and device specific IO for the Butterfly iQ.

'''

def ffprobe_count_frames(filename):
    """get metadata by using ffprobe

    Checks the output of ffprobe on the desired video
    file. MetaData is then parsed into a dictionary.

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
    ffprobe_count_frames_dict = ffprobe_count_frames_dict if ffprobe_count_frames_dict is not None else ffprobe_count_frames(fp)
    return vread(fp, outputdict={'-vframes' : ffprobe_count_frames_dict['video']['@nb_read_frames']})

def calc_spacing(npvid):
    '''
    Parameters
    ----------
    npvid : video or image, if image, H, W, RGB, if video, F, H, W, RGB
    '''
    if len(npvid.shape) == 4: # it's a video
        npimg = npvid[0,:,:,:].squeeze()
    else: # it's an image
        npimg = npvid

    ruler_peaks, ruler_props = find_peaks(npimg[:,-4,0].squeeze(), threshold=10, distance=10)
    ruler_dist = np.median(ruler_peaks[1:] - ruler_peaks[:-1])
    spacing = 2 / ruler_dist
    return spacing

def calc_crop(npvid, black_threshold_x=10, black_threshold_y=10):
    '''
    Calculates and returns 
    
    Returns
    -------
    [[start_row, end_row], [start_col, end_col]]
    '''
    if len(npvid.shape) == 4:
        npimg = npvid[0,:,:,0].squeeze()
    else:
        npimg = npvid[:,:,0].squeeze()
        
    cr, cc = (np.array(npimg.shape) / 2.0).astype('int')
    #cr = tmp[0]
    #cc = tmp[1]
    
    sr = cr
    er = cr
    row_baseline = len(np.argwhere(npimg[cr,:]))
    #pdb.set_trace()
    while sr > 0 and len(np.argwhere(npimg[sr,:])) - row_baseline < black_threshold_y:
        sr -= 1
    while er < npimg.shape[0]-1 and len(np.argwhere(npimg[er,:])) - row_baseline < black_threshold_y:
        er += 1
    
    
    sc = cc
    ec = cc
    col_baseline = len(np.argwhere(npimg[cc,:]))
    while sc > 0 and len(np.argwhere(npimg[sc,:])) - col_baseline < black_threshold_x:
        sc -= 1
    while ec < npimg.shape[1]-1 and len(np.argwhere(npimg[ec,:])) - col_baseline < black_threshold_x:
        ec += 1
    
    return np.array([[sr, er], [sc, ec]], dtype='int')

def preprocess_video(npvid, framerate=1):
    '''
    
    Returns
    -------
    img, spacing, crop
    '''
    spacing = calc_spacing(npvid)
    spacing = np.array([spacing, spacing, framerate])
    crop = calc_crop(npvid)
    img = tbitk.util.image_from_array(npvid[:,crop[0,0]:crop[0,1], crop[1,0]:crop[1,1],0].astype('float32') / 255.0, spacing=spacing)
    return img, spacing, crop

def preprocess_image(npimg):
    '''
    Returns
    -------
    img, spacing, crop
    '''
    spacing = calc_spacing(npimg)
    spacing = np.array([spacing, spacing])
    crop = calc_crop(npimg)
    img = tbitk.util.image_from_array(npimg[crop[0,0]:crop[0,1], crop[1,0]:crop[1,1],0].astype('float32') / 255.0, spacing=spacing)
    return img, spacing, crop
