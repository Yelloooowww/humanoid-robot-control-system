import sys
# import asahmi
# import numpy as np
import time
import serial
# import functools
from pyqtapp.ui_mainwindow import Ui_MainWindow
from pyqtapp.ui_remote_control_dialog import Ui_Dialog
from PyQt5.QtWidgets import QDialog,QMainWindow, QApplication,QLabel,QWidget,QVBoxLayout
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
    AAACCCKKK= pyqtSignal(bytes)
    def __init__(self,  parent=None):
        super().__init__(parent)
    def run(self):
        print('WaitForAck')
        get=ser.read(1)
        print('get ack=',get)
        self.AAACCCKKK.emit(get) #收到M128端的回應後發射訊號

class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.setupUi(self)
        self.RC_control=RemoteControl()
        self.RC_control.YouPress.connect(self.SimulateRemoteControl)
        self.ack=GetAck()
        self.ack.AAACCCKKK.connect(self.SomethingAfterAck)
        self.dia_waiting=QDialog()    #等待ack時叫使用者不要亂動的對話窗
        self.dia_waiting.setWindowTitle("Waiting")
        self.delay=0.05 #通訊速率從這裡改
        # self.flag=0 #總是會有需要旗標的時候@@
        self.now_angle_data=[35,35,45,40,67,40,60,49,42,35,49,0,75,41,40,40,68]
        self.accumulate_angle_data=[]
        self.num_of_active=0
        self.setting()
        self.vision_effect()
        self.control_object()
        self.total_table_set()

        self.spinBox_11.setEnabled(False) #紅色4號殘障了QQ
        self.horizontalSlider_11.setEnabled(False) #紅色4號殘障了QQ

    def setting(self):     #通訊埠設定
        items=("COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","COM10","COM11","COM12","COM13","COM14","COM15","COM16","COM17","COM18","COM19","COM20","COM21","COM22","COM23","COM24","COM25")
        item,ok=QInputDialog.getItem(self,"選擇序列埠","Select Your COM",items,0,False)
        ser.port = str(item)
        ser.open()
        time.sleep(5)

    def encoder_and_send_pac(self,data): #通訊封包
        pac=[]
        # pac+=[171,171,171]  #封包頭
        # pac += [len(data)]   #資料長度
        pac += data
        check= [(sum(data))%2] #基偶檢查
        # pac += check
        for x in pac:
            ser.write(bytes([x]))
            time.sleep(self.delay)

    def total_table_set(self):  #總表設定
        self.tableWidget.setHorizontalHeaderLabels(['定格1','定格2','定格3','定格4','定格5','定格6','定格7','定格8','定格9','定格10'])
        self.tableWidget.setVerticalHeaderLabels(['ID_0','ID_1','ID_2','ID_3','ID_4','ID_5','ID_6','ID_7','ID_8','ID_9','ID_10','ID_11','ID_12','ID_13','ID_14','ID_15','ID_16'])
        self.tableWidget.resizeColumnsToContents()  #調整格子寬度 與顯示內容配合
        self.tableWidget.resizeRowsToContents()  #調整格子長度 與顯示內容配合

    def total_table_update(self):  #總表更新
        self.lcdNumber.display(self.num_of_active) #LCD顯示值更新
        for x in range(170):
            if x < len(self.accumulate_angle_data):
                itemmm=QTableWidgetItem(str(self.accumulate_angle_data[x]))
                itemmm.setTextAlignment(Qt.AlignVCenter | Qt.AlignHCenter)
                self.tableWidget.setItem(x%17,int(x/17),itemmm)
            else:
                itemmm=QTableWidgetItem(' ')
                itemmm.setTextAlignment(Qt.AlignVCenter | Qt.AlignHCenter)
                self.tableWidget.setItem(x%17,int(x/17),itemmm)





    def vision_effect(self):   #設置一些圖片而已
        self.setWindowTitle('KHR-3HV')
        self.setWindowIcon(QIcon('.\pyqtapp\icon.ico'))
        self.label.setPixmap(QPixmap(".\pyqtapp\_background.jpg"))

    def control_object(self):     #按鈕們
        self.pushButton.clicked.connect(self.play)
        self.pushButton_2.clicked.connect(self.del_all)
        self.pushButton_3.clicked.connect(self.del_lastest_angle)
        self.pushButton_4.clicked.connect(self.save_to_PC)
        self.pushButton_5.clicked.connect(self.save_to_SDC)
        self.pushButton_6.clicked.connect(self.open_file)
        self.pushButton_7.clicked.connect(lambda:self.RC_control.show())
        self.pushButton_8.clicked.connect(self.note_now_angle)
        self.pushButton_9.clicked.connect(self.to_the_best_position)
        self.spinBox.valueChanged['int'].connect(self.ID_0)
        self.spinBox_1.valueChanged['int'].connect(self.ID_1)
        self.spinBox_2.valueChanged['int'].connect(self.ID_2)
        self.spinBox_3.valueChanged['int'].connect(self.ID_3)
        self.spinBox_4.valueChanged['int'].connect(self.ID_4)
        self.spinBox_5.valueChanged['int'].connect(self.ID_5)
        self.spinBox_6.valueChanged['int'].connect(self.ID_6)
        self.spinBox_7.valueChanged['int'].connect(self.ID_7)
        self.spinBox_8.valueChanged['int'].connect(self.ID_8)
        self.spinBox_9.valueChanged['int'].connect(self.ID_9)
        self.spinBox_10.valueChanged['int'].connect(self.ID_10)
        # self.spinBox_11.valueChanged['int'].connect(self.ID_11)  #紅色4號殘障了QQ
        self.spinBox_12.valueChanged['int'].connect(self.ID_12)
        self.spinBox_13.valueChanged['int'].connect(self.ID_13)
        self.spinBox_14.valueChanged['int'].connect(self.ID_14)
        self.spinBox_15.valueChanged['int'].connect(self.ID_15)
        self.spinBox_16.valueChanged['int'].connect(self.ID_16)

    def SimulateRemoteControl(self,value):   #遙控器送訊的地方
        print('YouPress=',value)
        self.ack.start()
        self.dia_waiting.show()
        self.encoder_and_send_pac([131,value])

    def ID_0(self):
        self.encoder_and_send_pac([128,0,self.spinBox.value()])
        self.now_angle_data[0]=self.spinBox.value()
    def ID_1(self):
        self.encoder_and_send_pac([128,1,self.spinBox_1.value()])
        self.now_angle_data[1]=self.spinBox_1.value()
    def ID_2(self):
        self.encoder_and_send_pac([128,2,self.spinBox_2.value()])
        self.now_angle_data[2]=self.spinBox_2.value()
    def ID_3(self):
        self.encoder_and_send_pac([128,3,self.spinBox_3.value()])
        self.now_angle_data[3]=self.spinBox_3.value()
    def ID_4(self):
        self.encoder_and_send_pac([128,4,self.spinBox_4.value()])
        self.now_angle_data[4]=self.spinBox_4.value()
    def ID_5(self):
        self.encoder_and_send_pac([128,5,self.spinBox_5.value()])
        self.now_angle_data[5]=self.spinBox_5.value()
    def ID_6(self):
        self.encoder_and_send_pac([128,6,self.spinBox_6.value()])
        self.now_angle_data[6]=self.spinBox_6.value()
    def ID_7(self):
        self.encoder_and_send_pac([128,7,self.spinBox_7.value()])
        self.now_angle_data[7]=self.spinBox_7.value()
    def ID_8(self):
        self.encoder_and_send_pac([128,8,self.spinBox_8.value()])
        self.now_angle_data[8]=self.spinBox_8.value()
    def ID_9(self):
        self.encoder_and_send_pac([128,9,self.spinBox_9.value()])
        self.now_angle_data[9]=self.spinBox_9.value()
    def ID_10(self):
        self.encoder_and_send_pac([128,10,self.spinBox_10.value()])
        self.now_angle_data[10]=self.spinBox_10.value()
    # def ID_11(self):     #紅色4號殘障了QQ
    #     self.encoder_and_send_pac([128,11,self.spinBox_11.value()])
    #     self.now_angle_data[11]=self.spinBox_11.value()
    def ID_12(self):
        self.encoder_and_send_pac([128,12,self.spinBox_12.value()])
        self.now_angle_data[12]=self.spinBox_12.value()
    def ID_13(self):
        self.encoder_and_send_pac([128,13,self.spinBox_13.value()])
        self.now_angle_data[13]=self.spinBox_13.value()
    def ID_14(self):
        self.encoder_and_send_pac([128,14,self.spinBox_14.value()])
        self.now_angle_data[14]=self.spinBox_14.value()
    def ID_15(self):
        self.encoder_and_send_pac([128,15,self.spinBox_15.value()])
        self.now_angle_data[15]=self.spinBox_15.value()
    def ID_16(self):
        self.encoder_and_send_pac([128,16,self.spinBox_16.value()])
        self.now_angle_data[16]=self.spinBox_16.value()


    def to_the_best_position(self):  #強迫轉到最佳位置
        self.now_angle_data=[75,86,95,47,73,75,90,83,75,64,55,0,77,75,60,67,75]
        # for x in range(17):
        #     if x != 11:
        #         self.encoder_and_send_pac([128,x,self.now_angle_data[x]])
        self.spinBox.setValue(75)
        self.spinBox_1.setValue(86)
        self.spinBox_2.setValue(95)
        self.spinBox_3.setValue(47)
        self.spinBox_4.setValue(73)
        self.spinBox_5.setValue(75)
        self.spinBox_6.setValue(90)
        self.spinBox_7.setValue(83)
        self.spinBox_8.setValue(75)
        self.spinBox_9.setValue(64)
        self.spinBox_10.setValue(55)
        # self.spinBox_11.setValue(103)    #紅色4號殘障了QQ
        self.spinBox_12.setValue(77)
        self.spinBox_13.setValue(75)
        self.spinBox_14.setValue(60)
        self.spinBox_15.setValue(67)
        self.spinBox_16.setValue(75)

    def note_now_angle(self):  #更新accumulate_angle_data
        if self.num_of_active <10:
            self.accumulate_angle_data += self.now_angle_data
            after_len=len(self.accumulate_angle_data)
            print('After len=',after_len)
            self.num_of_active +=1
            print('num=',self.num_of_active)
            self.total_table_update()  #更新總表
        else:
            QMessageBox.about(self,"Too long","動作串長度不可以超過10個")

    def del_lastest_angle(self):  #類似DEL鍵的功能
        if self.num_of_active >0:
            before_len=len(self.accumulate_angle_data)
            print('Before len=',before_len)
            self.accumulate_angle_data = self.accumulate_angle_data[0:(before_len-17)]
            after_len=len(self.accumulate_angle_data)
            print('After len=',after_len)
            self.num_of_active -=1
            print('num=',self.num_of_active)
            self.total_table_update()  #更新總表

            if self.num_of_active >1:
                #轉回上一個位置
                tmp=len(self.accumulate_angle_data)-17
                print('tmp=',tmp)
                self.spinBox.setValue(self.accumulate_angle_data[tmp])
                self.spinBox_1.setValue(self.accumulate_angle_data[tmp+1])
                self.spinBox_2.setValue(self.accumulate_angle_data[tmp+2])
                self.spinBox_3.setValue(self.accumulate_angle_data[tmp+3])
                self.spinBox_4.setValue(self.accumulate_angle_data[tmp+4])
                self.spinBox_5.setValue(self.accumulate_angle_data[tmp+5])
                self.spinBox_6.setValue(self.accumulate_angle_data[tmp+6])
                self.spinBox_7.setValue(self.accumulate_angle_data[tmp+7])
                self.spinBox_8.setValue(self.accumulate_angle_data[tmp+8])
                self.spinBox_9.setValue(self.accumulate_angle_data[tmp+9])
                self.spinBox_10.setValue(self.accumulate_angle_data[tmp+10])
                # self.spinBox_11.setValue(self.accumulate_angle_data[tmp+11])    #紅色4號殘障了QQ
                self.spinBox_12.setValue(self.accumulate_angle_data[tmp+12])
                self.spinBox_13.setValue(self.accumulate_angle_data[tmp+13])
                self.spinBox_14.setValue(self.accumulate_angle_data[tmp+14])
                self.spinBox_15.setValue(self.accumulate_angle_data[tmp+15])
                self.spinBox_16.setValue(self.accumulate_angle_data[tmp+16])

        else:
            QMessageBox.about(self,"Too Short","目前已經無暫存之動作串")

    def del_all(self):  #清空accumulate_angle_data  #類似AC鍵的功能
        if self.num_of_active >0:
            self.accumulate_angle_data = []
            self.num_of_active =0
            self.total_table_update()  #更新總表
            QMessageBox.about(self,"Clear","動作串已清空")
        else:
            QMessageBox.about(self,"No Thing","本來就沒有暫存之動作串")


    def save_to_PC(self):       #生成.txt
        if self.accumulate_angle_data !=[]:
            fileName, _ =QFileDialog.getSaveFileName(self,"QFileDialog.getSaveFileName()","(*.txt)")
            if fileName:
                print(fileName)
                f = open(fileName, 'w', encoding = 'UTF-8')
                data=self.accumulate_angle_data
                for x in data:
                    if x==0:
                        f.write('00')  #一定要兩位數才行嗎@@?
                        f.write('\n')
                    else:
                        f.write(hex(x)[2:])  #16進位去掉0x
                        f.write('\n')
                f.close()
            print('動作串已儲存')
            QMessageBox.about(self,"Save Success","動作串已儲存至電腦")
        else:
            print('目前無佔存之動作串')
            QMessageBox.about(self,"No Thing","尚無暫存之動作串")

    def SomethingAfterAck(self,value):
        self.dia_waiting.accept()  #將叫使用者不要亂動的對話窗關起來
        print('value=',value)
        if value== b'\x84':
            QMessageBox.about(self,"Save Success","動作串已儲存至SD卡")
        if value== b'\x83':
            QMessageBox.about(self,"Done","SD卡中的套裝動作已播放完成")
        if value== b'\x82':
            QMessageBox.about(self,"Done","播放完成")
        if value== b'\xff':
            QMessageBox.about(self,"Error","很抱歉出現異常錯誤")

    def save_to_SDC(self):
        if self.accumulate_angle_data !=[]:
            items=("1","2","3","4","5","6","7","8")
            item,ok=QInputDialog.getItem(self,"儲存至SD卡","選擇編號1~8",items,0,False)
            if ok and item:
                print('select=',item)
            data=[132,int(item),len(self.accumulate_angle_data)]
            data += self.accumulate_angle_data
            self.ack.start()
            self.dia_waiting.show()
            self.encoder_and_send_pac(data)
        else:
            print('目前無佔存之動作串')
            QMessageBox.about(self,"No Thing","尚無暫存之動作串")

    def play(self):   #播放模式130
        if self.accumulate_angle_data !=[]:
            data=[130,len(self.accumulate_angle_data)]
            data += self.accumulate_angle_data
            self.ack.start()
            self.dia_waiting.show()
            self.encoder_and_send_pac(data)
        else:
            print('目前無佔存之動作串')
            QMessageBox.about(self,"No Thing","尚無暫存之動作串")

    def open_file(self):    #匯入先前檔案
        self.accumulate_angle_data=[]  #暫存動作先歸零
        self.num_of_active=0  #也先歸零
        dlg=QFileDialog()
        if dlg.exec_():
            filenames=dlg.selectedFiles()
            f=open(filenames[0],'r')
            while True:
                line = f.readline()
                if not line: break
                self.accumulate_angle_data += [int(line,16)]  # hex->int
                self.num_of_active= int(len(self.accumulate_angle_data)/17)
                self.total_table_update()
            f.close()

            #詢問要不要轉到資料最後的位置
            reply=QMessageBox.question(self,"Ask","請問要不要轉到最後一個位置",QMessageBox.Yes|QMessageBox.No)
            print('Reply=',reply)
            if reply==16384:  #User Press 'Yes'
                #轉到最後一個位置
                tmp=len(self.accumulate_angle_data)-17
                print('tmp=',tmp)
                self.spinBox.setValue(self.accumulate_angle_data[tmp])
                self.spinBox_1.setValue(self.accumulate_angle_data[tmp+1])
                self.spinBox_2.setValue(self.accumulate_angle_data[tmp+2])
                self.spinBox_3.setValue(self.accumulate_angle_data[tmp+3])
                self.spinBox_4.setValue(self.accumulate_angle_data[tmp+4])
                self.spinBox_5.setValue(self.accumulate_angle_data[tmp+5])
                self.spinBox_6.setValue(self.accumulate_angle_data[tmp+6])
                self.spinBox_7.setValue(self.accumulate_angle_data[tmp+7])
                self.spinBox_8.setValue(self.accumulate_angle_data[tmp+8])
                self.spinBox_9.setValue(self.accumulate_angle_data[tmp+9])
                self.spinBox_10.setValue(self.accumulate_angle_data[tmp+10])
                # self.spinBox_11.setValue(self.accumulate_angle_data[tmp+11])    #紅色4號殘障了QQ
                self.spinBox_12.setValue(self.accumulate_angle_data[tmp+12])
                self.spinBox_13.setValue(self.accumulate_angle_data[tmp+13])
                self.spinBox_14.setValue(self.accumulate_angle_data[tmp+14])
                self.spinBox_15.setValue(self.accumulate_angle_data[tmp+15])
                self.spinBox_16.setValue(self.accumulate_angle_data[tmp+16])
                self.now_angle_data=self.accumulate_angle_data[tmp:]











if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
