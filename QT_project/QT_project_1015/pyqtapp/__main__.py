import sys
import asahmi
import numpy as np
import time
import serial
from PyQt5.QtWidgets import QDialog,QMainWindow, QApplication,QLabel,QWidget,QVBoxLayout
from .ui_mainwindow import Ui_MainWindow
from .ui_remote_control_dialog import Ui_Dialog
from PyQt5.QtGui import QIcon,QPixmap,QPalette
from PyQt5.QtCore import Qt
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import QThread, pyqtSignal


ser = serial.Serial()
ser.baudrate = 9600
ser.timeout=1000


class RemoteControl(QDialog, Ui_Dialog):    #遙控器對話窗
    YouPress=pyqtSignal(int)
    def __init__(self, parent=None):
        super(RemoteControl, self).__init__(parent)
        self.setupUi(self)
        self.setWindowTitle('Remote Control')
        self.setWindowIcon(QIcon('.\pyqtapp\icon.ico'))
        self.buttons()

    def buttons(self):
        self.pushButton.clicked.connect(lambda:self.YouPress.emit(int(1)))
        self.pushButton_2.clicked.connect(lambda:self.YouPress.emit(int(2)))
        self.pushButton_3.clicked.connect(lambda:self.YouPress.emit(int(3)))
        self.pushButton_4.clicked.connect(lambda:self.YouPress.emit(int(4)))
        self.pushButton_5.clicked.connect(lambda:self.YouPress.emit(int(5)))
        self.pushButton_6.clicked.connect(lambda:self.YouPress.emit(int(6)))
        self.pushButton_7.clicked.connect(lambda:self.YouPress.emit(int(7)))
        self.pushButton_8.clicked.connect(lambda:self.YouPress.emit(int(8)))


class GetAck(QThread):     #這是ack
    AAACCCKKK= pyqtSignal()
    def __init__(self,  parent=None):
        super().__init__(parent)
    def run(self):
        print('WaitForAck')
        # get=ser.read(1)
        # print(get)
        print('沒有ACK我也可以運作得很好 = =+')
        self.AAACCCKKK.emit() #收到M128端的回應後發射訊號

class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.RC_control=RemoteControl()
        self.RC_control.YouPress.connect(self.SimulateRemoteControl)
        self.ack=GetAck()
        self.ack.AAACCCKKK.connect(self.SomethingAfterAck)
        self.setting()
        self.Mode=0
        self.delay=0.05 #通訊速率從這裡改
        # self.flag=0 #總是會有需要旗標的時候@@
        self.now_angle_data=[]
        self.accumulate_angle_data=[]
        self.send_matrix=[]
        self.tmp=[35,35,45,40,67,40,60,49,42,35,45,40,67,40,60,49,42]
        self.setupUi(self)
        self.vision_effect()
        self.control_object()


        self.pushButton.setEnabled(False)
        self.pushButton_2.setEnabled(False)
        self.pushButton_3.setEnabled(False)
        self.pushButton_4.setEnabled(False)
        self.pushButton_5.setEnabled(False)
        self.pushButton_6.setEnabled(False)
        self.spinBox.setEnabled(False)
        self.spinBox_1.setEnabled(False)
        self.spinBox_2.setEnabled(False)
        self.spinBox_3.setEnabled(False)
        self.spinBox_4.setEnabled(False)
        self.spinBox_5.setEnabled(False)
        self.spinBox_6.setEnabled(False)
        self.spinBox_7.setEnabled(False)
        self.spinBox_8.setEnabled(False)
        self.spinBox_9.setEnabled(False)
        self.spinBox_10.setEnabled(False)
        self.spinBox_11.setEnabled(False)
        self.spinBox_12.setEnabled(False)
        self.spinBox_13.setEnabled(False)
        self.spinBox_14.setEnabled(False)
        self.spinBox_15.setEnabled(False)
        self.spinBox_16.setEnabled(False)
        self.radioButton.setEnabled(True)
        self.radioButton_2.setEnabled(False)
        self.radioButton_3.setEnabled(False)

    def vision_effect(self):   #設置一些圖片而已
        self.setWindowTitle('KHR-3HV')
        self.setWindowIcon(QIcon('.\pyqtapp\icon.ico'))
        win=QMainWindow()
        palette=QPalette()
        win.setPalette(palette)
        self.resize(620,620)
        palette1 = QPalette()
        palette1.setBrush(self.backgroundRole(), QBrush(QPixmap(".\pyqtapp\_background.jpg")))   # 设置背景图片
        self.setPalette(palette1)

    def control_object(self):     #按鈕拉條們
        self.pushButton.clicked.connect(self.note_now_angle)
        self.pushButton_2.clicked.connect(self.test_actions)
        self.pushButton_3.clicked.connect(self.del_actions)
        self.pushButton_4.clicked.connect(self.save_dialog)
        self.pushButton_5.clicked.connect(self.get_file_dailog)
        self.pushButton_6.clicked.connect(self.to_the_best_position)
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
        self.radioButton.toggled.connect(self.use_radiobutton_to_SetMode)
        # self.radioButton_2.toggled.connect(self.use_radiobutton_to_SetMode)
        # self.radioButton_3.toggled.connect(self.use_radiobutton_to_SetMode)

    def setting(self):     #通訊埠設定
        items=("COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","COM10","COM11","COM12","COM13","COM14","COM15","COM16","COM17","COM18","COM19","COM20","COM21","COM22","COM23","COM24","COM25")
        item,ok=QInputDialog.getItem(self,"選擇序列埠","Select Your COM",items,0,False)
        ser.port = str(item)
        ser.open()
        time.sleep(5)

    def SomethingAfterAck(self):    #收到Ack以後要做的事
        print('HEE')  #防呆裝置 禁致能按鈕們
        if  self.Mode==128:
            print('收到Ack! Mode==128')
            self.textBrowser.append('電腦教學模式')
            self.pushButton.setEnabled(True)
            self.pushButton_2.setEnabled(False)
            self.pushButton_3.setEnabled(False)
            self.pushButton_4.setEnabled(False)
            self.pushButton_5.setEnabled(False)
            self.pushButton_6.setEnabled(True)
            self.spinBox.setEnabled(True)
            self.spinBox_1.setEnabled(True)
            self.spinBox_2.setEnabled(True)
            self.spinBox_3.setEnabled(True)
            self.spinBox_4.setEnabled(True)
            self.spinBox_5.setEnabled(True)
            self.spinBox_6.setEnabled(True)
            self.spinBox_7.setEnabled(True)
            self.spinBox_8.setEnabled(True)
            self.spinBox_9.setEnabled(True)
            self.spinBox_10.setEnabled(True)
            self.spinBox_11.setEnabled(True)
            self.spinBox_12.setEnabled(True)
            self.spinBox_13.setEnabled(True)
            self.spinBox_14.setEnabled(True)
            self.spinBox_15.setEnabled(True)
            self.spinBox_16.setEnabled(True)
            self.radioButton.setEnabled(True)
            self.radioButton_2.setEnabled(True)
            self.radioButton_3.setEnabled(True)
        if  self.Mode==130:
            print('收到Ack!!! Mode==130')
            self.textBrowser.append('播放模式')
            self.pushButton.setEnabled(False)
            self.pushButton_2.setEnabled(True)
            self.pushButton_3.setEnabled(True)
            self.pushButton_4.setEnabled(True)
            self.pushButton_5.setEnabled(True)
            self.pushButton_6.setEnabled(False)
            self.spinBox.setEnabled(False)
            self.spinBox_1.setEnabled(False)
            self.spinBox_2.setEnabled(False)
            self.spinBox_3.setEnabled(False)
            self.spinBox_4.setEnabled(False)
            self.spinBox_5.setEnabled(False)
            self.spinBox_6.setEnabled(False)
            self.spinBox_7.setEnabled(False)
            self.spinBox_8.setEnabled(False)
            self.spinBox_9.setEnabled(False)
            self.spinBox_10.setEnabled(False)
            self.spinBox_11.setEnabled(False)
            self.spinBox_12.setEnabled(False)
            self.spinBox_13.setEnabled(False)
            self.spinBox_14.setEnabled(False)
            self.spinBox_15.setEnabled(False)
            self.spinBox_16.setEnabled(False)
            self.radioButton.setEnabled(True)
            self.radioButton_2.setEnabled(True)
            self.radioButton_3.setEnabled(False)
        if  self.Mode==131:
            print('收到Ack!!! Mode==131')
            self.textBrowser.append('模擬遙控器模式')
            self.pushButton.setEnabled(False)
            self.pushButton_2.setEnabled(False)
            self.pushButton_3.setEnabled(False)
            self.pushButton_4.setEnabled(False)
            self.pushButton_5.setEnabled(False)
            self.pushButton_6.setEnabled(False)
            self.spinBox.setEnabled(False)
            self.spinBox_1.setEnabled(False)
            self.spinBox_2.setEnabled(False)
            self.spinBox_3.setEnabled(False)
            self.spinBox_4.setEnabled(False)
            self.spinBox_5.setEnabled(False)
            self.spinBox_6.setEnabled(False)
            self.spinBox_7.setEnabled(False)
            self.spinBox_8.setEnabled(False)
            self.spinBox_9.setEnabled(False)
            self.spinBox_10.setEnabled(False)
            self.spinBox_11.setEnabled(False)
            self.spinBox_12.setEnabled(False)
            self.spinBox_13.setEnabled(False)
            self.spinBox_14.setEnabled(False)
            self.spinBox_15.setEnabled(False)
            self.spinBox_16.setEnabled(False)
            self.radioButton.setEnabled(True)
            self.radioButton_2.setEnabled(False)
            self.radioButton_3.setEnabled(True)
            self.RC_control.show()
            self.RC_control.exec_()

    def SimulateRemoteControl(self,value):   #遙控器模式下送訊的地方
        print('YouPress=',value)
        # ser.write(bytes([131]))
        # time.sleep(1)
        ser.write(bytes([value]))


    def use_radiobutton_to_SetMode(self):    #切換MODE唯一的方法!!!
        if self.radioButton.isChecked()==True:
            self.Mode=128
            ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            self.ack.start()
        if self.radioButton_2.isChecked()==True:
            self.Mode=130
            ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            self.ack.start()
        if self.radioButton_3.isChecked()==True:
            self.Mode=131
            ser.write(bytes([self.Mode]))
            time.sleep(self.delay)
            self.ack.start()



    def test_actions(self):#測試動作串
        if self.accumulate_angle_data !=[]:
            print('size=',len(self.accumulate_angle_data))
            self.textBrowser.append('測試動作串')
            ser.write(bytes([len(self.accumulate_angle_data)]))
            time.sleep(self.delay)
            for x in self.accumulate_angle_data:
                ser.write(bytes([x]))
                time.sleep(self.delay)
        else:
            print('目前無佔存之動作')
            self.textBrowser.append('目前無佔存之動作')

    def get_file_dailog(self): #播放檔案
        self.textBrowser.append('播放檔案')
        dlg=QFileDialog()
        if dlg.exec_():
            filenames=dlg.selectedFiles()
            f=open(filenames[0],'r')
            while True:
                line = f.readline()
                if not line: break
                self.send_matrix += [int(line,16)]  # hex->int
            f.close()
            time.sleep(self.delay)
            print('size=',len(self.send_matrix))
            ser.write(bytes([len(self.send_matrix)]))
            time.sleep(self.delay)
            for x in self.send_matrix:
                ser.write(bytes([x]))
                time.sleep(self.delay)

    def save_dialog(self):    #生成檔案(.txt)
        if self.accumulate_angle_data !=[]:
            fileName, _ =QFileDialog.getSaveFileName(self,"QFileDialog.getSaveFileName()","(*.txt)")
            if fileName:
                print(fileName)
                f = open(fileName, 'w', encoding = 'UTF-8')
                data=self.accumulate_angle_data
                for x in data:
                    f.write(hex(x)[2:])  #轉16進位
                    f.write('\n')
                f.close()
            self.accumulate_angle_data=[]
            print('動作串已儲存')
            self.textBrowser.append('動作串已儲存')
        else:
            print('目前無佔存之動作串')
            self.textBrowser.append('目前無佔存之動作串')


    def closeEvent(self, event):   #送出關機訊息
        self.Mode= 132
        ser.write(bytes([self.Mode]))
        time.sleep(self.delay)
        print('切換至:關機模式')

    def note_now_angle(self):  #更新accumulate_angle_data
        self.accumulate_angle_data += self.now_angle_data
        # for x in self.accumulate_angle_data:
        #     print(x)
        print('紀錄目前位置')
        self.textBrowser.append('紀錄目前位置')

    def del_actions(self):  #清空accumulate_angle_data
        if self.accumulate_angle_data !=[]:
            self.accumulate_angle_data=[]
            print('刪除動作串')
            self.textBrowser.append('刪除動作串')
        else:
            self.textBrowser.append('目前無佔存之動作串')

    def to_the_best_position(self):  #在電腦教學模式下強迫轉到最佳位置
        self.textBrowser.append('轉到最佳位置')
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

    def now_angle_func(self):  #電腦模式下 送出有改變的馬達ID &角度值
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
