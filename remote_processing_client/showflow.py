#!/usr/bin/env python

'''
showflow.py - Optical-flow demonstrations

    To test:

      % python showflow.py               # video from webcam
      % python showflow.py -f FILENAME   # video from file
      % python showflow.py -c CAMERA     # specific camera number
      % python showflow.py -s N          # scale-down factor for flow image
      % python showflow.py -m M          # move step in pixels

    Adapted from 
 
    https://code.ros.org/trac/opencv/browser/trunk/opencv/samples/python/fback.py?rev=2271

    Copyright (C) 2014 Simon D. Levy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as 
    published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
'''

import cv2
import numpy as np

import time
import math
import optparse

import urllib


from optical_flow import OpticalFlowCalculator

if __name__=="__main__":

    parser = optparse.OptionParser()

    parser.add_option("-f", "--file",  dest="filename", help="Read from video file", metavar="FILE")
    parser.add_option("-s", "--scaledown", dest="scaledown", help="Fractional image scaling", metavar="SCALEDOWN")
    parser.add_option("-c", "--camera", dest="camera", help="Camera number", metavar="CAMERA")
    parser.add_option("-m", "--movestep", dest="movestep", help="Move step (pixels)", metavar="MOVESTEP")

    (options, _) = parser.parse_args()

    camno = int(options.camera) if options.camera else 0

    #cap = cv2.VideoCapture(camno if not options.filename else options.filename)

    #width    = int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
    #height   = int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))

    width    = 640
    height   = 480

    scaledown = int(options.scaledown) if options.scaledown else 1

    movestep = int(options.movestep) if options.movestep else 16

    #flow = OpticalFlowCalculator(width, height, window_name='Optical Flow', scaledown=1, move_step=16)
    flow = OpticalFlowCalculator(width, height, window_name='Optical Flow', scaledown=scaledown, move_step=16)

    start_sec = time.time()
    count = 0


    stream=urllib.urlopen('http://@192.168.1.8:5000/video_feed.mjpg')
    bytes=''
    
    while True:
        bytes+=stream.read(1024)
        a = bytes.find('\xff\xd8')
        b = bytes.find('\xff\xd9')

        if a!=-1 and b!=-1:
            jpg = bytes[a:b+2]
            bytes= bytes[b+2:]
            frame = cv2.imdecode(np.fromstring(jpg, dtype=np.uint8),cv2.CV_LOAD_IMAGE_COLOR)
            
            result = flow.processFrame(frame)

            elapsed_sec = time.time() - start_sec

    print('%dx%d image: %d frames in %3.3f sec = %3.3f frames / sec' % 
             (width/scaledown, height/scaledown, count, elapsed_sec, count/elapsed_sec))
