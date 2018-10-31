import sys
import asahmi
import numpy as np
import time
import serial


from PyQt5.QtWidgets import QMainWindow, QApplication,QLabel,QWidget,QVBoxLayout
from .ui_mainwindow import Ui_MainWindow
from PyQt5.QtGui import QIcon,QPixmap,QPalette
from PyQt5.QtCore import Qt
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import QThread, pyqtSignal

ser = serial.Serial()
ser.baudrate = 9600
ser.timeout=1000

class GETACK(QThread):      #這是ACK
    def __init__(self,  parent=None):
        super().__init__(parent)
    def __del__(self):
        print('WaitForAck')
        self.wait()
        print('GetAck')
    def run(self):
        get=ser.read(1)
        print(get)

class TeachingByHandMode(QThread):    #手動模式執行序
    def __init__(self, parent=None):
        super().__init__(parent)
    def __del__(self):
        print('DEL')
        self.wait()
        print('TERMINATE')
    def run(self):
        x=0
        while 1:
            print(x)
            x += 1
            time.sleep(1)




class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.setting()
        self.now_angle_data=[]
        self.accumulate_angle_data=[]
        self.send_matrix=[]
        self.delay=0.05
        self.tmp=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        self.setupUi(self)
        self.vision_effect()
        self.control_object()
        self.to_the_best_position()



    def vision_effect(self):  #設置一些圖片而已
        self.setWindowTitle('KHR-3HV')
        self.setWindowIcon(QIcon('.\pyqtapp\icon.ico'))
        win=QMainWindow()
        palette=QPalette()
        win.setPalette(palette)
        self.resize(620,620)
        palette1 = QPalette()
        palette1.setBrush(self.backgroundRole(), QBrush(QPixmap(".\pyqtapp\_background.jpg")))
        self.setPalette(palette1)


    def control_object(self):
        self.pushButton.clicked.connect(self.note_now_angle)
        self.pushButton_2.clicked.connect(self.test_actions)
        self.pushButton_3.clicked.connect(self.del_actions)
        self.pushButton_4.clicked.connect(self.save_dialog)
        self.pushButton_5.clicked.connect(self.get_file_dailog)
        self.pushButton_6.clicked.connect(self.setting)
        self.pushButton_8.clicked.connect(self.startPCMode)
        self.pushButton_9.clicked.connect(self.startHandMode)
        # self.pushButton_9.clicked.connect(lambda :TeachingByHandMode().start())
        # self.pushButton_8.clicked.connect(lambda :TeachingByHandMode().terminate())
        self.spinBox.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_1.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_2.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_3.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_4.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_5.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_6.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_7.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_8.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_9.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_10.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_11.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_12.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_13.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_14.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_15.valueChanged['int'].connect(self.now_angle_func)
        self.spinBox_16.valueChanged['int'].connect(self.now_angle_func)

    def setting(self):  #序列埠設定
        items=("COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","COM10","COM11","COM12","COM13","COM14","COM15","COM16","COM17","COM18","COM19","COM20","COM21","COM22","COM23","COM24","COM25")
        item,ok=QInputDialog.getItem(self,"選擇序列埠","Select Your COM",items,0,False)
        ser.port = str(item)
        ser.open()
        time.sleep(5)

    def test_actions(self):   #切換至播放模式 播放accumulate_angle_data
        if self.accumulate_angle_data !=[]:
            self.Mode= 2+128
            ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            GETACK().start()
            print('切換至:播放模式')
            self.textBrowser.append('切換至:播放模式')

            print('size=',len(self.accumulate_angle_data))
            ser.write(bytes([len(self.accumulate_angle_data)]))
            time.sleep(self.delay)
            for x in self.accumulate_angle_data:
                ser.write(bytes([x]))
                time.sleep(self.delay)
            print('測試動作串')
            self.textBrowser.append('測試動作串(播放中)')
            GETACK().start()
            self.textBrowser.append('播放完成')
            self.Mode= 128
            ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            GETACK().start()
            self.textBrowser.append('切換回預設電腦教學模式')

        else:
            print('目前無佔存之動作')
            self.textBrowser.append('目前無佔存之動作')
            self.Mode= 0+128
            ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            GETACK().start()
            self.textBrowser.append('切換回預設:電腦教學模式')


    def get_file_dailog(self):  #播放已儲存檔案
        self.Mode= 2+128
        ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        GETACK().start()
        print('切換至:播放模式')
        self.textBrowser.append('切換至:播放模式')

        dlg=QFileDialog()
        if dlg.exec_():
            filenames=dlg.selectedFiles()
            f=open(filenames[0],'r')
            while True:
                line = f.readline()
                if not line: break
                self.send_matrix += [int(line)]
            f.close()
            time.sleep(self.delay)
            print('size=',len(self.send_matrix))
            ser.write(bytes([len(self.send_matrix)]))
            time.sleep(self.delay)
            for x in self.send_matrix:
                ser.write(bytes([x]))
                time.sleep(self.delay)
            print('播放儲存之動作串')
            self.textBrowser.append('播放儲存之動作串(播放中)')
            GETACK().start()
            self.textBrowser.append('播放完成')
            self.Mode= 128
            ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            GETACK().start()
            self.textBrowser.append('切換回預設電腦教學模式')


    def save_dialog(self):    #儲存檔案(生成.txt)
        if self.accumulate_angle_data !=[]:
            fileName, _ =QFileDialog.getSaveFileName(self,"QFileDialog.getSaveFileName()",'D:\\',"(*.txt)")
            if fileName:
                print(fileName)
                f = open(fileName, 'w', encoding = 'UTF-8')
                data=self.accumulate_angle_data
                for x in data:
                    f.write(str(x))
                    f.write('\n')
                f.close()
            self.accumulate_angle_data=[]
            print('動作串已儲存')
            self.textBrowser.append('動作串已儲存')
        else:
            print('目前無佔存之動作串')
            self.textBrowser.append('目前無佔存之動作串')

        self.Mode= 0+128
        ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        GETACK().start()
        self.textBrowser.append('切換回預設電腦教學模式')


    def closeEvent(self, event):   #送出關機訊息
        self.Mode= 3+128
        ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        print('切換至:關機模式')



    def note_now_angle(self):
        self.accumulate_angle_data += self.now_angle_data
        for x in self.accumulate_angle_data:
            print(x)
        print('紀錄目前位置')
        self.textBrowser.append('紀錄目前位置')
    def del_actions(self):
        if self.accumulate_angle_data !=[]:
            self.accumulate_angle_data=[]
            print('刪除動作串')
            self.textBrowser.append('刪除動作串')
        else:
            self.textBrowser.append('目前無佔存之動作串')


    def startHandMode(self):
        self.Mode= 1+128
        ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        GETACK().start()
        print('切換至:手動教學模式')
        self.textBrowser.append('切換至:手動教學模式(先不要測這個)')
        # print('START')
        # TeachingByHandMode().start()
    def startPCMode(self):
        # TeachingByHandMode().terminate()
        # print('EEENNNDDD')
        self.Mode= 128
        ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        GETACK().start()
        print('切換至:電腦教學模式')
        self.textBrowser.append('切換至:電腦教學模式')




    def to_the_best_position(self):  #初始化動作(at電腦教學模式)
        time.sleep(3)
        self.Mode= 128
        ser.write(bytes([self.Mode]))
        time.sleep(1)
        GETACK().start()
        print('預設:電腦教學')
        self.textBrowser.append('預設:電腦教學')
        self.spinBox.setValue(75)
        self.spinBox_1.setValue(86)
        self.spinBox_2.setValue(95)
        self.spinBox_3.setValue(47)
        self.spinBox_4.setValue(73)
        self.spinBox_5.setValue(75)
        self.spinBox_6.setValue(90)
        self.spinBox_7.setValue(83)
        self.spinBox_8.setValue(75)
        self.spinBox_9.setValue(86)
        self.spinBox_10.setValue(95)
        self.spinBox_11.setValue(47)
        self.spinBox_12.setValue(73)
        self.spinBox_13.setValue(75)
        self.spinBox_14.setValue(90)
        self.spinBox_15.setValue(83)
        self.spinBox_16.setValue(75)


    def now_angle_func(self):  #電腦教學模式時 用來送出角度變化
        if self.Mode==128:
            a=self.horizontalSlider.value()
            b=self.horizontalSlider_1.value()
            c=self.horizontalSlider_2.value()
            d=self.horizontalSlider_3.value()
            e=self.horizontalSlider_4.value()
            f=self.horizontalSlider_5.value()
            g=self.horizontalSlider_6.value()
            h=self.horizontalSlider_7.value()
            i=self.horizontalSlider_8.value()
            j=self.horizontalSlider_9.value()
            k=self.horizontalSlider_10.value()
            l=self.horizontalSlider_11.value()
            m=self.horizontalSlider_12.value()
            n=self.horizontalSlider_13.value()
            o=self.horizontalSlider_14.value()
            p=self.horizontalSlider_15.value()
            q=self.horizontalSlider_16.value()
            self.now_angle_data=[a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q]
            for x in range(17):
                if(self.tmp[x] != self.now_angle_data[x]):
                    ser.write(bytes([x]))
                    time.sleep(self.delay)
                    ser.write(bytes([self.now_angle_data[x]]))
                    time.sleep(self.delay)
                    self.tmp[x] = self.now_angle_data[x]








if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())