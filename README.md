
### OpenCV remote (web-based) stream processing.

This is a proof of concept of remote video processing using OpenCV and a MJPG-WebCam streaming in pyhton. The goal of this code is to delegate heavy CV processing of the video recorded by a SoC (System on a Chip) in a remote, more powerful host.

Included sources:

* server: MJPG Streaming server, based on Miguel Grinberg, Log0 and Peter Guan work.
* client: Simple remote streaming viewer, based on several Zaw Lin's contributions to online discussion forums (http://stackoverflow.com/questions/21702477/how-to-parse-mjpeg-http-stream-from-ip-camera).
* remote_processing_client: an adaptation of the Optical Flow algorithm developed by Simon D. Levy (https://github.com/simondlevy/OpenCV-Python-Hacks), where a MJPG stream is used instead of a local web camera.
	
How to use:

	In a machine with a WebCam (in my case, an Intel Edison board):
		- Enter the right ip-address in last lines of mjpgstreamserver.py file.
		- Execute the server (python mjpgstreamserver.py)
	
	In a remote machine (in my case, a laptop):
		- Modify the simple client or the 'processing' client by updating the URL of the video stream URL (using the IP address of the machine with the WebCam):   stream=urllib.urlopen('http://@192.168.1.8:5000/video_feed.mjpg')
		- Execute the client.
	
	

hector.cadavid@escuelaing.edu.co
