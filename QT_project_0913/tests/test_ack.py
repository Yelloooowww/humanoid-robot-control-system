import sys
import asahmi
import numpy as np
import time
import serial



if __name__ == "__main__":

    hh = asahmi.HmiHandler(str('COM1'), timeout = 1,baudrate=9600)
    hh.start()
    time.sleep(1)
    data= np.array([1],dtype=np.uint8)
    hh.putArray(data)

    hh.eventGetString.wait()
    g=hh.get()
    print(g)



    # ser = serial.Serial()
    # ser.baudrate = 9600
    # ser.port = 'COM1'
    # ser.open()
    # time.sleep(1)
    #
    # ser.write(b'\x01')
    # time.sleep(delay)
