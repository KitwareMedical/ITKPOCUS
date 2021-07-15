# Traumatic Brain Injury Toolkit (tbitk) Python Library

## Installation
We recommend using a Python virtual environment

1. Install <https://www.ffmpeg.org/>
2. Add ffmpeg binary directory to your PATH
3. Activate your virtual environment, install Python dependencies, install tbitk
``` 
# activate your virtual environment, e.g., source venv-3.6.7/Scripts/Activate
cd TraumaticBrainInjury/tbitk
pip install requirements.txt
pip install -e tbitk
```

## Usage
The scripts provided convert manufacturer video files to ITK Image objects.  They may also remove overlays from the ultrasound image and set the physical dimension of the image by processing the overlay ruler (when applicable).

```
import tbitk
import matplotlib.pyplot as plt
from tbitk.butterfly import ffprobe_count_frames, vread_workaround
from tbitk.util import get_framerate

data_path=PATH_TO_FILE

# clarius
npvid = vread(data_path)
vidmeta = skvideo.io.ffprobe(data_path)
img, spacing, crop = tbitk.clarius.preprocess_video(npvid, framerate=get_framerate(vidmeta))
plt.imshow(npvid[0]) # plot the first frame

# butterfly
vidmeta = ffprobe_count_frames(data_path)
npvid = vread_workaround(data_path, vidmeta)
img, spacing, crop = tbitk.butterfly.preprocess_video(npvid, framerate=get_framerate(vidmeta))
plt.imshow(npvid[0]) # plot the first frame

# sonoque
import tbitk.sonoque
x, spacing, crop = tbitk.sonoque.load_and_preprocess_video(data_path)
plt.imshow(npvid[0]) # plot the first frame
```