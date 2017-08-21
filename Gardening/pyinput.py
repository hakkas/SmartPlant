#!/usr/bin/python

import serial
import time
from twython import Twython

from auth import (
    consumer_key,
    consumer_secret,
    access_token,
    access_token_secret
)

ser = serial.Serial('/dev/ttyACM0', 9600, 8, 'N', 1, timeout=5)

twitter = Twython(
    consumer_key,
    consumer_secret,
    access_token,
    access_token_secret
)

# message = "Hello world!"

while True:
    tijd = time.time()
    line = ser.readline().decode('UTF-8')
    line = line.rstrip() 
    if line != "": 
        print (line)
    if line != "" and not ("fail" in line):
        line += " Timestamp: " + str(tijd)
        twitter.update_status(status=line)
        print("Tweeted: %s" % line)
