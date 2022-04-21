# import pyside6
import pyigtl
import itk
import argparse
import logging
from pathlib import Path
from time import sleep
import numpy as np

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='OpenIGTLink Server to record video')
    parser.add_argument('-o', '--output', help='Path to save video (.mha)', required=True)
    parser.add_argument('-d', '--device_name', help='Device name', required=True)
    parser.add_argument('-p', '--port', help='Port for server to listen on', type=int, default=18944)
    parser.add_argument('-m', '--max_frames', help='Max number of frames to record', type=int, default=-1)
    parser.add_argument('-t', '--video_timeout', help='Seconds to timeout', type=int, default=3)
    parser.add_argument('-v', '--verbose', help='Verbose', action='store_true')
    parser.add_argument('-f', '--force', help='Overwrite output if exists', action='store_true')
    # --output --port --max_frames --video_timeout --ui --force
    
    args = parser.parse_args()
    output = args.output
    device_name = args.device_name
    port = args.port
    max_frames = args.max_frames
    video_timeout = args.video_timeout
    force = args.force
    
    log_level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=log_level)
    
    p = Path(output)
    if p.exists() and not force:
        raise FileExistsError(str(p))
    

    server = pyigtl.OpenIGTLinkServer(port=port)
    logging.info(f'Listening on port: {port}')
    while not server.is_connected():
        sleep(0.1)
    
    logging.info('Client connected')
    
    frame_cnt = 0
    msg_images = []
    ijk_to_world_matrix = None
    while frame_cnt < max_frames or max_frames < 0:
        timeout = -1 if frame_cnt == 0 else video_timeout
        
        msg = server.wait_for_message(device_name, timeout)
        if msg is None:
            break
        
        frame_cnt += 1
        ijk_to_world_matrix = msg.ijk_to_world_matrix
        msg_images.append(msg.image)
        logging.debug(f'Frame {frame_cnt} received')
    
    if msg is None:
        logging.info(f'Client message timeout')
    
    vid = None
    if len(msg_images) > 0:
        logging.debug(f'Received {len(msg_images)} of shape: {msg_images[0].shape}\nand world coordinage: {ijk_to_world_matrix}')
        vid = itk.image_from_array(np.concatenate(msg_images))
        itk.imwrite(vid, str(p), compression=True)
        logging.info(f'Wrote video {{{vid.shape}}} to: {str(p)}')

