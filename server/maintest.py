#!/usr/bin/env python
from flask import Flask, render_template, Response, send_from_directory, make_response, request, current_app

app = Flask(__name__, static_url_path='')


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/startup')
def startup():
    # TODO call 0
    return "startup"


@app.route('/stop')
def stop():
    # TODO call 1
    return "stop"


@app.route('/tinyleft')
def tinyleft():
    # TODO call 8
    return "tinyleft"


@app.route('/tinyright')
def tinyright():
    # TODO call 9
    return "tinyright"


@app.route('/left')
def left():
    # TODO call 3
    return "left"


@app.route('/right')
def right():
    # TODO call 4
    return "right"


@app.route('/speedup')
def speedup():
    # TODO call 5
    return "speedup"


@app.route('/speeddown')
def speeddown():
    # TODO call 6
    return "speeddown"


@app.route('/backward')
def backward():
    # TODO call 7
    return "backward"


if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5001, debug=True)
    # app.run(host='0.0.0.0', port=5001, debug=True)
