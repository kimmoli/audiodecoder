audiodecoder test.mp3 | gst-launch-0.10 -v fdsrc ! wavparse ! audioconvert ! alsasink
