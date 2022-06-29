import itk
import itkpocus.util as util
import numpy as np

def preprocess_image(img_raw, version=None, probe_type='linear'):
    '''
    Loads and preprocessed Sonivate image as recorded using 3D Slicer OpenIGTLinkRF.  Crops
    the input image and corrects the spacing.
    
    Parameters
    ----------
    img_raw : itk.Image[,2] 
        
    version : None
        reserved for future use
    probe_type : 'linear' ('phased' reserved for future use)
        Which of the Sonivate probes (it's a dual probe) is used
    Returns
    -------
    img : itk.Image[itk.F,3]
    meta : dict
        Meta data dictionary
    '''
    if probe_type != 'linear':
        raise NotImplemented('Only probe_type=\'linear\' is supported')
    
    crop = np.array([[0, img_raw.shape[2]-1], [133, 378]])
    
    npimg = util.crop(itk.array_from_image(img_raw).squeeze(), crop) / 255.0
    img = itk.image_from_array(npimg)
    
    img.SetSpacing([img_raw.GetSpacing()[0], img_raw.GetSpacing()[1]])
    return img, { 'spacing' : [ img.GetSpacing()[0], img.GetSpacing()[1] ], 'crop' : crop }

def preprocess_video(img_raw, version=None, probe_type='linear' ):
    '''
    Loads and preprocessed Sonivate video as recorded using itkpocus.openigtlink.  Crops
    the input video and corrects the spacing.
    
    Parameters
    ----------
    img_raw : itk.Image[,3]
        
    version : None
        reserved for future use
    probe_type : 'linear' ('phased' reserved for future use)
        Which of the Sonivate probes (it's a dual probe) is used
    Returns
    -------
    img : itk.Image[itk.F,3]
    meta : dict
        Meta data dictionary
    '''
    if probe_type != 'linear':
        raise NotImplemented('Only probe_type=\'linear\' is supported')
    
    crop = np.array([[0, img_raw.shape[2]-1], [133, 378]])
    
    npimg = util.crop(img_raw, crop) / 255.0
    img = itk.image_from_array(npimg)
    img.SetSpacing([0.0776016, 0.0776016, 1/24.0])
    
    return img, { 'spacing' : [ img.GetSpacing()[0], img.GetSpacing()[1], img.GetSpacing()[2] ], 'crop' : crop }

def load_and_preprocess_image(fp, version=None, probe_type='linear'):
    '''
    Loads and preprocessed Sonivate '.nrrd' image as recorded using 3D Slicer OpenIGTLinkRF.  Crops
    the input image and corrects the spacing.
    
    Parameters
    ----------
    fp : str 
        filepath
    version : None
        reserved for future use
    probe_type : 'linear' ('phased' reserved for future use)
        Which of the Sonivate probes (it's a dual probe) is used
    Returns
    -------
    img : itk.Image[itk.F,3]
    meta : dict
        Meta data dictionary
    '''
    img_raw = itk.imread(fp)
    return preprocess_image(img_raw, version, probe_type)

def load_and_preprocess_video(fp, version=None, probe_type='linear'):
    '''
    Loads and preprocessed Sonivate '.mha' as recorded using itkpocus.openigtlink.  Crops
    the input video and corrects the spacing.
    
    Parameters
    ----------
    fp : str 
        filepath
    version : None
        reserved for future use
    probe_type : 'linear' ('phased' reserved for future use)
        Which of the Sonivate probes (it's a dual probe) is used
    Returns
    -------
    img : itk.Image[itk.F,3]
    meta : dict
        Meta data dictionary
    '''
    img_raw = itk.imread(fp)
    return preprocess_video(img_raw, version, probe_type)
