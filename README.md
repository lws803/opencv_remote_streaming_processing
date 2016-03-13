
### OpenCV web-based stream processing.

This is a proof of concept of remote video processing using OpenCV and a MJPG-WebCam streaming in pyhton.

Included sources:
	- server: MJPG Streaming server, based on Miguel Grinberg, Log0 and Peter Guan work.
	- client: Simple remote streaming viewer, based on several Zaw Lin's contributions to online discussion forums (http://stackoverflow.com/questions/21702477/how-to-parse-mjpeg-http-stream-from-ip-camera).
	- remote_processing_client: an adaptation of the Optical Flow algorithm developed by Simon D. Levy (https://github.com/simondlevy/OpenCV-Python-Hacks), where a MJPG stream is used instead of a local web camera.
	


hector.cadavid@escuelaing.edu.co
