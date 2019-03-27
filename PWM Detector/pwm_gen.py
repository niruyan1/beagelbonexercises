import Adafruit_BBIO.GPIO as GPIO
import Adafruit_BBIO.PWM as PWM
import datetime
import sys
from time import sleep
import struct

infile_path = "/dev/input/js0"
EVENT_SIZE = struct.calcsize("llHHI")
file = open(infile_path, "rb")
event = file.read(EVENT_SIZE)

pwmpin="P9_42";
dutycycle=50;
PWM.start(pwmpin,dutycycle,10);
while event:
    (tv_sec, tv_usec, type, code, value) = struct.unpack("llHHI", event)
    if value/1000000 == 302:
        print(struct.unpack("llHHI",event))
        if dutycycle>0:
            dutycycle=dutycycle-1;
    elif value/1000000 == 268:
        print (struct.unpack("llHHI",event))
        if dutycycle<100:
            dutycycle=dutycycle+1

    PWM.set_duty_cycle(pwmpin,dutycycle);
    event = file.read(EVENT_SIZE)
