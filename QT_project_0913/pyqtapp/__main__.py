import sys
import asahmi
import numpy as np
import time
import serial


from PyQt5.QtWidgets import QMainWindow, QApplication,QLabel,QWidget,QVBoxLayout
# from .ui_mainwindow import Ui_MainWindow
from .ui_mainwindow_before import Ui_MainWindow
from PyQt5.QtGui import QIcon,QPixmap,QPalette
from PyQt5.QtCore import Qt
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *




class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.ser = serial.Serial()
        self.ser.baudrate = 9600
        self.setting()

        self.now_angle_data=[]
        self.accumulate_angle_data=[]
        self.send_matrix=[]
        self.delay=0.005
        self.tmp=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        self.setupUi(self)
        self.vision_effect()
        self.control_object()


        self.Mode=128
        self.ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        print('預設:電腦教學')
        self.textBrowser.append('預設:電腦教學')
        # self.hh.eventGetArray.wait()
        # print("Get Ack")


    def vision_effect(self):
        self.setWindowTitle('KHR-3HV')
        self.setWindowIcon(QIcon('.\pyqtapp\icon.ico'))
        win=QMainWindow()
        palette=QPalette()
        win.setPalette(palette)
        self.resize(900,900)
        palette1 = QPalette()
        palette1.setBrush(self.backgroundRole(), QBrush(QPixmap(".\pyqtapp\_background.jpg")))   # 设置背景图片
        self.setPalette(palette1)


    def control_object(self):
        self.pushButton.clicked.connect(self.note_now_angle)
        self.pushButton_2.clicked.connect(self.test_actions)
        self.pushButton_3.clicked.connect(self.del_actions)
        self.pushButton_4.clicked.connect(self.save_dialog)
        self.pushButton_5.clicked.connect(self.get_file_dailog)
        self.pushButton_6.clicked.connect(self.setting)
        self.pushButton_7.clicked.connect(self.close_window)
        self.radioButton.setChecked(True)
        self.radioButton.toggled.connect(self.choose_mode)

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

    def setting(self):
        items=("COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","COM10","COM11","COM12","COM13","COM14","COM15","COM16","COM17","COM18","COM19","COM20","COM21","COM22","COM23","COM24","COM25")
        item,ok=QInputDialog.getItem(self,"選擇序列埠","Select Your COM",items,0,False)

        self.ser.port = str(item)
        self.ser.open()
        time.sleep(5)

    def test_actions(self):
        if self.accumulate_angle_data !=[]:
            self.Mode= 2+128
            self.ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            print('切換至:播放模式')
            self.textBrowser.append('切換至:播放模式')
            # self.hh.eventGetArray.wait()
            # print("Get Ack")


            for x in self.accumulate_angle_data:
                self.ser.write(bytes([x]))
                time.sleep(self.delay)
            print('測試動作串')
            self.textBrowser.append('測試動作串')
            # if 播放完成:
            #     self.Mode= 0+128
            #     .....
            #     self.textBrowser.append('播放完成 切換回預設電腦教學模式')
            #     self.hh.eventGetArray.wait()
            #     print("Get Ack")

        else:
            print('目前無佔存之動作')
            self.textBrowser.append('目前無佔存之動作')
            self.Mode= 0+128
            self.ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            self.textBrowser.append('切換回預設:電腦教學模式')
            # self.hh.eventGetArray.wait()
            # print("Get Ack")


    def get_file_dailog(self):
        self.Mode= 2+128
        self.ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        print('切換至:播放模式')
        self.textBrowser.append('切換至:播放模式')
        # self.hh.eventGetArray.wait()
        # print("Get Ack")

        dlg=QFileDialog()
        if dlg.exec_():
            filenames=dlg.selectedFiles()
            f=open(filenames[0],'r')
            while True:
                line = f.readline()
                if not line: break
                self.send_matrix += [int(line)]
            f.close()
            time.sleep(0.005)
            for x in self.send_matrix:
                self.ser.write(bytes([x]))
                time.sleep(self.delay)
            print('播放儲存之動作串')
            self.textBrowser.append('播放儲存之動作串')

        # if 播放完成:
        #     self.Mode=0
        #    ......
        #     self.textBrowser.append('播放完成 切換回預設電腦教學模式')
        #     self.hh.eventGetArray.wait()
        #     print("Get Ack")


    def save_dialog(self):
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
        self.ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        self.textBrowser.append('切換回預設電腦教學模式')
        # self.hh.eventGetArray.wait()
        # print("Get Ack")


    def closeEvent(self, event):
        self.Mode= 3+128
        self.ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        print('切換至:關機模式')
        # self.hh.eventGetArray.wait()
        # print("Get Ack")

    def close_window(self):
        sender=self.sender()
        self.Mode= 3+128
        self.ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        print('切換至:關機模式')
        self.textBrowser.append('切換至:關機模式')
        # self.hh.eventGetArray.wait()
        # print("Get Ack")
        self.ser.close()
        app.quit()
        app.exec_()


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


    def choose_mode(self):
        if self.radioButton.isChecked() :
            self.Mode= 0+128
            self.ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            print('切換至:電腦教學模式')
            self.textBrowser.append('切換至:電腦教學模式')
            # self.hh.eventGetArray.wait()
            # print("Get Ack")

        else:
            self.Mode= 1+128
            self.ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            print('切換至:手動教學模式')
            self.textBrowser.append('切換至:手動教學模式(先不要測這個)')
            # self.hh.eventGetArray.wait()
            # print("Get Ack")
            self.now_angle_func()






    def now_angle_func(self):
        if self.Mode==129:
            time.sleep(0.001)
            # while self.radioButton_2.isChecked():
            #     print('!')
            # self.hh.get()
            # print('!')
            # getsome=self.hh.get()
            # self.now_angle_data=self.hh.getsome[1]
            # self.spinBox.setValue(self.now_angle_data[0])
            # self.spinBox_1.setValue(self.now_angle_data[1])
            # self.spinBox_2.setValue(self.now_angle_data[2])
            # self.spinBox_3.setValue(self.now_angle_data[3])
            # self.spinBox_4.setValue(self.now_angle_data[4])
            # self.spinBox_5.setValue(self.now_angle_data[5])
            # self.spinBox_6.setValue(self.now_angle_data[6])
            # self.spinBox_7.setValue(self.now_angle_data[7])
            # self.spinBox_8.setValue(self.now_angle_data[8])
            # self.spinBox_9.setValue(self.now_angle_data[9])
            # self.spinBox_10.setValue(self.now_angle_data[10])
            # self.spinBox_11.setValue(self.now_angle_data[11])
            # self.spinBox_12.setValue(self.now_angle_data[12])
            # self.spinBox_13.setValue(self.now_angle_data[13])
            # self.spinBox_14.setValue(self.now_angle_data[14])
            # self.spinBox_15.setValue(self.now_angle_data[15])
            # self.spinBox_16.setValue(self.now_angle_data[16])



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
                    self.ser.write(bytes([x]))
                    time.sleep(self.delay)
                    self.ser.write(bytes([self.now_angle_data[x]]))
                    time.sleep(self.delay)

                    self.tmp[x] = self.now_angle_data[x]








if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
