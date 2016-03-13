#!/usr/bin/env python


###############################################################################
#
#   Project : Video Streaming with Flask
#   Author  : Peter Guan, Xu Guo
#   Date    : 11/11/2015
#
#   Description:
#   Modified to support streaming out with webcams, and not just raw JPEGs.
#   Most of the code credits to Miguel Grinberg.
#   Credits : http://blog.miguelgrinberg.com/post/video-streaming-with-flask
#             http://www.chioka.in/
#
#
###############################################################################
from __future__ import print_function
from flask import Flask, render_template, Response, send_from_directory
from camera import VideoCamera
import smbus
import datetime
import time

app = Flask(__name__, static_url_path='')


def gen(camera):
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')

@app.route('/video_feed.mjpg')
def video_feed():
    return Response(gen(VideoCamera()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='192.168.1.8', debug=True)
