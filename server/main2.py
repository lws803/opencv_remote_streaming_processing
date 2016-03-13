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
from flask import Flask, render_template, Response, send_from_directory, make_response, request, current_app
import smbus
import datetime
import time
from functools import update_wrapper
from datetime import timedelta
from mpu6050 import MPU6050

# for RPI version 1, use "bus = smbus.SMBus(0)"
xbus = smbus.SMBus(1)
tmp_address = 0x68  # This is the address value read via the i2cdetect command
bus = smbus.SMBus(1)  # or bus = smbus.SMBus(1) for Revision 2 boards

# This is the address we setup in the Arduino Program
address = 0x04
flag = 0

serverCmd = {'forward': 1,
             'backward': 2,
             'turnleft': 3,
             'turnright': 4,
             'stop': 5,
             'request': 6
             }


def crossdomain(origin=None, methods=None, headers=None,
                max_age=21600, attach_to_all=True,
                automatic_options=True):
    if methods is not None:
        methods = ', '.join(sorted(x.upper() for x in methods))
    if headers is not None and not isinstance(headers, basestring):
        headers = ', '.join(x.upper() for x in headers)
    if not isinstance(origin, basestring):
        origin = ', '.join(origin)
    if isinstance(max_age, timedelta):
        max_age = max_age.total_seconds()

    def get_methods():
        if methods is not None:
            return methods

        options_resp = current_app.make_default_options_response()
        return options_resp.headers['allow']

    def decorator(f):
        def wrapped_function(*args, **kwargs):
            if automatic_options and request.method == 'OPTIONS':
                resp = current_app.make_default_options_response()
            else:
                resp = make_response(f(*args, **kwargs))
            if not attach_to_all and request.method != 'OPTIONS':
                return resp

            h = resp.headers

            h['Access-Control-Allow-Origin'] = origin
            h['Access-Control-Allow-Methods'] = get_methods()
            h['Access-Control-Max-Age'] = str(max_age)
            if headers is not None:
                h['Access-Control-Allow-Headers'] = headers
            return resp

        f.provide_automatic_options = False
        return update_wrapper(wrapped_function, f)
    return decorator


def writeNumber(value):
    global flag
    try:
        xbus.write_byte(address, value)
    except IOError:
        subprocess.call('i2cdetect', '-y', '1')
        flag = 1
    return -1


def readNumber():
    try:
        num = xbus.read_byte(address)
    except IOError:
        # :subprocess.call('i2cdetect', '-y', '1')
        print("io error")
        flag = 1
        return -1
    print("num is {0}".format(num))
    return num


app = Flask(__name__, static_url_path='')


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/css/<path:path>')
def send_js(path):
    return send_from_directory('css', path)


@app.route('/index2')
def index2():
    return render_template('index2.html')


def gen(camera):
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')


def processRes(response):
    if response is not None:
        print(response)
        return "True"
    return "False"


@app.route('/startup')
@crossdomain(origin='*')
def startup():
    # TODO call 0
    writeNumber(0)
    response = readNumber()
    return processRes(response)


@app.route('/stop')
@crossdomain(origin='*')
def stop():
    # TODO call 1
    writeNumber(1)
    response = readNumber()
    return processRes(response)


@app.route('/tinyleft')
@crossdomain(origin='*')
def tinyleft():
    # TODO call 8
    writeNumber(7)
    response = readNumber()
    return processRes(response)


@app.route('/tinyright')
@crossdomain(origin='*')
def tinyright():
    # TODO call 9
    print("tiny right")
    writeNumber(8)
    response = readNumber()
    return processRes(response)


@app.route('/left')
@crossdomain(origin='*')
def left():
    # TODO call 3
    writeNumber(2)
    response = readNumber()
    return processRes(response)


@app.route('/right')
@crossdomain(origin='*')
def right():
    # TODO call 4
    writeNumber(3)
    response = readNumber()
    return processRes(response)


@app.route('/speedup')
@crossdomain(origin='*')
def speedup():
    # TODO call 5
    writeNumber(4)
    response = readNumber()
    return processRes(response)


@app.route('/speeddown')
@crossdomain(origin='*')
def speeddown():
    # TODO call 6
    writeNumber(5)
    response = readNumber()
    return processRes(response)


@app.route('/backward')
@crossdomain(origin='*')
def backward():
    # TODO call 6
    writeNumber(6)
    response = readNumber()
    return processRes(response)


@app.route('/forward')
@crossdomain(origin='*')
def forward_world():
    print("request start")
    return "ok"

@app.route('/temp')
@crossdomain(origin='*')
def get_temp():
    mpu = MPU6050(bus, tmp_address, "MPU6050")
    mpu.read_all()
    temp = mpu.read_temp()
    print("temp is {0}".format(temp))
    return str(temp)


if __name__ == '__main__':
    app.run(host='10.1.0.233', port=5001, debug=True)
    # app.run(host='0.0.0.0', port=5001, debug=True)
