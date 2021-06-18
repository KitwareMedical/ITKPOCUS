import itk
import numpy as np
from random import shuffle

def extract_slice(img, slice_, axis=2):
    '''
    Returns a 2D frame from a 3D image (assumes itk.F pixels)
    
    Adds frame to the index of the largest possible region according to the selected axis.  Direction submatrix is maintained
    in returned frame image.
    
    Parameters
    ----------
    img : 3D ITK image
    slice_ : the frame/slice to get
    axis : a binary array of size dim specifing which index to slice along, the default assumes typical z-axis index
    '''
    
    region = img.GetLargestPossibleRegion()
    size = list(region.GetSize())
    size[axis] = 0
    region.SetSize(size)
    index = list(region.GetIndex())
    index[axis] += slice_
    region.SetIndex(index)
    extractor = itk.ExtractImageFilter[itk.Image[itk.F,3], itk.Image[itk.F,2]].New(Input=img, ExtractionRegion=region)
    extractor.SetDirectionCollapseToSubmatrix()
    extractor.Update()
    return extractor.GetOutput()

def polgyon_from_array(pts):
    '''
    Return a PolygonSpatialObject from a list of points defining a closed polygon.
    
    Parameters
    ==========
    pts(nx2 nparray) : ordered list of polygon points, x0 != xn
    '''
    if pts is None:
        return None
    
    poly1=itk.PolygonSpatialObject[2].New(IsClosed=True)
    for i in range(pts.shape[0]):
        spt = itk.SpatialObjectPoint[2]()
        spt.SetPositionInObjectSpace(itk.Point[itk.D,2](pts[i,:]))
        poly1.AddPoint(spt)
    poly1.Update()
    return poly1

def image_from_array(array, spacing=None, direction=None, origin=None, reference_image=None):
    '''
    Augment ITK image_from_array so common parameters can be set in one line
    '''
    if reference_image is not None:
        spacing = reference_image.GetSpacing()
        direction = reference_image.GetDirection()
        origin = reference_image.GetOrigin()
    
    img = itk.image_from_array(array)
    if spacing is not None:
        img.SetSpacing(spacing)
    if direction is not None:
        img.SetDirection(direction)
    if origin is not None:
        img.SetOrigin(origin)
    
    return img

def array_from_image(img, return_meta=False):
    '''
    Augment ITK array_from_image so that common parameters are also returned
    
    Returns
    =======
    img --or-- (if return_meta)
    img, spacing, direction, origin
    '''
    # return array, spacing, direction, origin
    if return_meta:
        return itk.array_from_image(img), img.GetSpacing(), img.GetDirection(), img.GetOrigin()
    else:
        return itk.array_from_image(img)


def operate(img1, img2, foo):
    '''
    Convenience function for applying a function foo between two images.  foo is written on numpy arrays.  This function will output
    ITK arrays or numpy arrays to match the input.
    '''
    assert type(img1) == type(img2)
    tstr = str(type(img1))
    if tstr.contains('itkImage'):
        npimg1 = itk.array_from_image(img1)
        npimg2 = itk.array_from_image(img2)
    else:
        npimg1 = img1
        npimg2 = img2
        
    npimg3 = foo(npimg1, npimg2)
    if tstr.contains('itkImage'):
        ans = image_from_array(npimg3, img1.GetSpacing(), img1.GetDirection(), img1.GetOrigin())
    else:
        ans = npimg3
    
    return ans
        
def overwrite_mask(npimg1, npimg2, outside_value=0):
    '''
    Combine npimg1 and npimg2 while overwriting any overlap with npimg2's value.
    '''
    ans = npimg1.copy()
    idx = npimg2 != outside_value
    ans[idx] = npimg2[idx]
    return ans
    
def union_mask(npimg1, npimg2, outside_value=0, inside_value=1):
    '''
    Combine npimg1 and npimg2 and set the union to inside_value
    '''
    ans = np.ones(npimg1.shape) * outside_value
    idx = np.logical_or(npimg1 != outside_value, npimg2 != outside_value)
    ans[idx] = inside_value
    return ans

def array_from_spatial(obj, size, inside_value=1, outside_value=0):
    '''
    Return a Numpy array image of an ITKSpatialOjbect
    
    obj (ITKSpatialObject or list of ITKSpatialObject) : spatial object(s) to convert to mask image, should be in index (i.e. spacing = 1.0, origin = 0.0) coordinates
    size (nparray(2)) : size in x and y (columns, rows).  In the future may be None to self-calculate.
    reference_image (itk.Image) : sets the spacing, direction, origin, and size of the output image
    inside_value (real or list) : value to assign interior of each mask, if real, same value for each mask
    outside_value
    '''
    # the below is to allow obj to either be a single spatial object or a list
    # expand objects to lists
    if not type(obj) == list:
        obj = [obj]
    
    if not type(inside_value) == list:
        inside_value = [inside_value] * len(obj)
    
    npimg = np.ones(np.flip(size)) * outside_value
    for i in range(len(obj)):
        o = obj[i]
        if o is not None:
            filter = itk.SpatialObjectToImageFilter[itk.SpatialObject[2], itk.Image[itk.F,2]].New(Input=o, InsideValue=inside_value[i], OutsideValue=outside_value, Size=size.tolist())
            filter.Update()
            mask = filter.GetOutput()
            npimg = overwrite_mask(npimg, array_from_image(mask), outside_value=outside_value)

    return npimg

def image_from_spatial(obj, reference_image, inside_value=1, outside_value=0):
    '''
    Return a mask image from a single or list of ITKSpatialObjects.
    
    Parameters
    ==========
    
    obj (ITKSpatialObject or list of ITKSpatialObject) : spatial object(s) to convert to mask image, should be in index (i.e. spacing = 1.0, origin = 0.0) coordinates
    reference_image (itk.Image) : sets the spacing, direction, origin, and size of the output image
    inside_value (real or list) : value to assign interior of each mask, if real, same value for each mask
    outside_value
    '''
    npimg = array_from_spatial(obj, np.array(reference_image.GetLargestPossibleRegion().GetSize()), inside_value, outside_value)
    ans = image_from_array(npimg, reference_image.GetSpacing(), reference_image.GetDirection(), reference_image.GetOrigin())
    return ans

def get_framerate(meta_dict):
    '''
    Returns the framerate as a float from a meta_dict from ffprobe.
    
    Parameters
    ==========
    meta_dict (dict)
    '''
    arr = meta_dict['video']['@avg_frame_rate'].split('/')
    return float(arr[1]) / float(arr[0])
    

def get_slice(img, idx, axis, squeeze=True):
    '''
    Take an orthogonal slice from an image (must be axis-aligned, no interpolation).
    
    Parameters
    ==========
    img (itk.Image)
    idx (int) : index in axis to take slice
    axis (int) : axis of slice
    squeeze (bool) : whether to remove the specified axis from the output image
    
    Returns
    =======
    itk.Image : with dimensionality reduced by 1
    '''
    npimg, spacing, direction, origin = array_from_image(img, return_meta=True)
    npimg = npimg.take(indices=[idx], axis=axis)
    if squeeze: # remove the extra dimension of npimg and remove it from spacing/direction/origin
        npimg = npimg.squeeze(axis=axis)
        
        tmp = list(spacing)
        del(tmp[axis])
        spacing = np.array(tmp, dtype='float32').tolist()
        
        # this works for my case but probably needs a general sanity check
        tmp = itk.array_from_matrix(direction)
        tmp = np.delete(tmp, axis, 0) # delete row corresponding to axis
        tmp = np.delete(tmp, axis, 1) # delete column corresponding to axis
        direction = itk.matrix_from_array(tmp)
    
        tmp = list(origin)
        del(tmp[axis])
        origin = np.array(tmp, dtype='float32').tolist()
        
    return image_from_array(npimg, spacing=spacing, direction=direction, origin=origin)

def wrap_itk_index(x):
    '''
    TODO: see if this is necessary in newer ITK version
    '''
    idx = itk.Index[2]()
    idx.SetElement(0, int(x[0]))
    idx.SetElement(1, int(x[1]))
    return idx

def wrap_itk_point(x):
    '''
    TODO: see if this is necessary in newer ITK version
    '''
    # TODO, why itk.F?
    pt = itk.Point[itk.F,2]()
    pt.SetElement(0, x[0])
    pt.SetElement(1, x[1])
    return pt

def transform_to_physical(indices, image):
    '''
    Transform [y,x] indices to physical locations in an image.  Note, this is not the same as ITK's index scheme.
    '''
    start_index = np.asarray(image.GetLargestPossibleRegion().GetIndex())
    return np.apply_along_axis(lambda x: np.array(image.TransformIndexToPhysicalPoint(wrap_itk_index(x))), 1, np.fliplr(indices) + start_index[np.newaxis,:])

def transform_to_indices(pts, image):
    # TODO find usage of this and figure out if I want to get rid of it
    '''
    Transform ITK's physical locations to [y,x] indices.  Note, this is not the same as ITK's index scheme.
    '''
    start_index = np.asarray(image.GetLargestPossibleRegion().GetIndex())
    return np.fliplr(np.apply_along_axis(lambda x: np.array(image.TransformPhysicalPointToIndex(wrap_itk_point(x))), 1, pts) - start_index[np.newaxis,:])

def window_sample(x, spacing, num=None):
    '''
    Randomly sample an array of indices maintaining a minimum distance between samples.
    
    Parameters
    ==========
    x (ndarray of int) : an array of indices such as from np.argwhere
    spacing (int) : distance maintained between return indices, i.e., or all x,y in result, |x-y| > spacing
    num (int) : number of samples to return, or as many as possible if None
    
    Returns
    =======
    ndarray subsample of x
    '''
    num = num if num is not None else len(x)
    y = x.copy()
    shuffle(y)
    ans = list()
    while len(y) > 0 and len(ans) < num:
        tmp = y[0]
        ans.append(tmp)
        y = [r for r in y[1:] if r < tmp - spacing or r > tmp + spacing]
    
    return ans