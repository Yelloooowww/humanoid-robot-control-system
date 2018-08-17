import sys
from PyQt5.QtWidgets import QMainWindow, QApplication
from .ui_mainwindow import Ui_MainWindow

class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.setupUi(self)
        self.pushButton_A.clicked.connect(self.btnAfun)
        self.pushButton_B.clicked.connect(self.btnBfun)
        self.horizontalSlider.valueChanged['int'].connect(self.see)
    def see(self,data):
        print('value=%d' % data)
        return data


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
