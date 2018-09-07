import serial
import time

ser = serial.Serial()
ser.baudrate = 9600
ser.port = 'COM9'
ser.open()
time.sleep(5)

delay=0.0005
ser.write(b'\xAB')
time.sleep(delay)
ser.write(b'\xAB')
time.sleep(delay)
ser.write(b'\xAB')
time.sleep(delay)
ser.write(b'\x04')
time.sleep(delay)
ser.write(b'\x00')
time.sleep(delay)
ser.write(b'\x01')
time.sleep(delay)
ser.write(bytes([128]))
time.sleep(delay)
ser.write(bytes([128]))
time.sleep(delay)
