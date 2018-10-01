import sys
import asahmi
import numpy as np
import time
import serial

if __name__=='__main__':
    hh = asahmi.HmiHandler('COM5', timeout = 100,baudrate=38400)
    hh.start()
    time.sleep(1)
    while 1:
        time.sleep(1)
        print('!')
        getsome=hh.get()
        print(getsome)
        for x in range(17):
            print(getsome[1][x])
