## Video Streaming with Flask Example

### Install Flask and Check Flask Version
1. check flask version
```shell
python -c "import flask; print flask.__version__"
```
2. install flask
```shell
pip install Flask
```

### Run this sample
1. modify the last line of main.py and main2.py, update ip address in ``` app.run(host='10.1.0.233', debug=True) ``` to local machine ip address.
2. run main.py. use ``` python main.py ```.
3. run main2.py. use ``` python main2.py ```.
4. visit the page [http://your_ip_address:5000](http://your_ip_address:5000), run this demo.