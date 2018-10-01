import sys
import asahmi
import numpy as np
import time

from PyQt5.QtWidgets import QMainWindow, QApplication
from .ui_mainwindow import Ui_MainWindow

class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.setupUi(self)
        self.pushButton_A.clicked.connect(self.btnAfun)
        self.pushButton_B.clicked.connect(self.btnBfun)
        self.horizontalSlider.valueChanged['int'].connect(self.see)
    def see(self,value):
        hh = asahmi.HmiHandler('COM5', timeout = 1)
        hh.start()
        time.sleep(0.005)
        data= np.array([value],dtype=np.uint8)
        hh.putArray(data)
        print('value=%d' % value)



    def btnAfun(self):
        print('stdout btnA')
        self.textBrowser.append('HI~~~~~')

    def btnBfun(self):
        print('stdout btnB')
        self.textBrowser.append('Hello ^___^')




if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
