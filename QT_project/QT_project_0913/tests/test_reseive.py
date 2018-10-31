import serial
import time

ser = serial.Serial()
ser.baudrate = 9600
ser.port = 'COM2'
ser.open()
time.sleep(5)

delay=0.0005
while 1:
    rr=ser.read()
    print(rr)
