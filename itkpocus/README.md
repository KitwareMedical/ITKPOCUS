# ITKPOCUS (ITK Point-of-Care Ultrasound) Python Library

## Documentation
1. Example Jupyter Notebooks
2. Readthedocs

## Installation
1. Install <https://www.ffmpeg.org/>
2. Add ffmpeg binary directory to your PATH
    1. Note: errors in this step will result in exceptions from _skvideo_
3. Activate your virtual environment
4. `pip install itk-pocus`

## Usage
The scripts provided convert manufacturer video files to ITK Image objects.  They may also remove overlays from the ultrasound image and set the physical dimension of the image by processing the overlay ruler (when applicable).

```
import itkpocus.clarius as clarius
import matplotlib.pyplot as plt
import itk

img_fp=PATH_TO_FILE
video_fp=PATH_TO_FILE

img, meta = load_and_preprocess_image(img_fp)
plt.imshow(img)
print(img, meta)

vid, vid_meta = load_and_preprocess_video(video_fp)
plt.imshow(itk.array_from_image(vid)[0,:,:]) # plot first frame
print(vid, vid_meta)
```