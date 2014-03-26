# -*- coding: UTF-8 -*-
# balmer@inbox.ru 2014 RLC Meter
import sys, os, csv
from PyQt4 import QtCore, QtGui

import matplotlib
import time
import datetime
import threading
import json

import usb_commands
from jplot import calculateJson
from jplot import formatR
from jplot import readJson
import plot

TITLE = 'RLC Meter "Balmer 303" (R) 2014'


class FormMain(QtGui.QMainWindow):

    def __init__(self, parent=None):
        super(FormMain, self).__init__(parent)

        self.setWindowTitle(TITLE)
        self.CreateMainFrame()
        pass        
        
    def CreateMainFrame(self):
        self.main_frame = QtGui.QWidget()
        vbox = QtGui.QVBoxLayout()

        header_label = QtGui.QLabel(u'Измеритель комплексного сопротивления')

        vbox.addWidget(header_label)

        scan_button = QtGui.QPushButton(u'Просканировать диапазон')
        scan_button.clicked.connect(self.OnScan)
        vbox.addWidget(scan_button)

        graph_button = QtGui.QPushButton(u'Просмотреть последний график')
        graph_button.clicked.connect(self.OnGraph)
        vbox.addWidget(graph_button)

        graph_button = QtGui.QPushButton(u'Просмотреть график...')
        graph_button.clicked.connect(self.OnGraphOpen)
        vbox.addWidget(graph_button)

        cal_button = QtGui.QPushButton(u'Калибровка')
        cal_button.clicked.connect(self.OnCalibration)
        vbox.addWidget(cal_button)

        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)
        pass

    def initDevice(self):
        if not usb_commands.initDevice():
            QtGui.QMessageBox.about(self, TITLE, u"Устройство не найдено.")
            return False
        return True

    def OnScan(self):
        if not self.initDevice():
            return
        form = FormScan(self)
        form.startDefault()
        form.show()
        pass

    def OnGraph(self):
        form = plot.FormDrawData(TITLE, self)
        form.setData('freq.json')
        form.show()
        pass

    def OnGraphOpen(self):
        fileName = QtGui.QFileDialog.getOpenFileName(filter='freq json (*.json)', caption=TITLE+' - Open freq.json')
        form = plot.FormDrawData(TITLE, self)
        form.setData(fileName)
        form.show()
        pass

    def OnCalibration(self):
        if not self.initDevice():
            return
        form = FormCalibrationResistor(self)
        form.show()
        pass



class FormScan(QtGui.QMainWindow):
    self_ptr = None
    scan_freq = None
    endThread = False

    def __init__(self, parent=None):
        super(FormScan, self).__init__(parent)
        self.setWindowTitle(TITLE)
        self.CreateMainFrame()
        pass

    def startDefault(self):
        FormScan.self_ptr = self
        self.scan_freq = usb_commands.ScanFreq()
        self.scan_freq.init()

        self.progress_bar.setRange(0, self.scan_freq.count())
        self.progress_bar.setValue(0)

        FormScan.endThread = False
        self.th = threading.Thread(target=FormScan.UsbThread, args=[])
        self.th.start()
        pass

    def startCalibrate(self):
        FormScan.self_ptr = self

        FormScan.endThread = False
        self.th = threading.Thread(target=FormScan.CalibrateThreadV, args=[])
        self.th.start()
        pass

    def CreateMainFrame(self):
        self.main_frame = QtGui.QWidget()
        vbox = QtGui.QVBoxLayout()

        self.header_label = QtGui.QLabel(u'Сканирование диапазона.')
        vbox.addWidget(self.header_label)

        self.progress_bar = QtGui.QProgressBar()

        vbox.addWidget(self.progress_bar)

        self.info_label = QtGui.QLabel(u'info');
        vbox.addWidget(self.info_label);


        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)
        pass

    def closeEvent(self, event):
        FormScan.endThread = True
        event.accept()
        pass

    @staticmethod
    def UsbThread():
        s = FormScan.self_ptr
        while s.scan_freq.next():
            if FormScan.endThread:
                return
            FormScan.SetInfo()
            pass

        FormScan.SetInfo()
        s.scan_freq.save()
        pass

    @staticmethod
    def CalibrateThreadI():
        '''
        Калибровка коэффициэнтов усиления тока сопротивлением 1 КОм
        '''
        s = FormScan.self_ptr

        resistorIndex = 0
        prefix = 'cor/KI'+str(resistorIndex)

        for index in xrange(7):
            fileName = prefix+'_'+str(index)+'.json'
            s.header_label.setText(fileName)
            s.scan_freq = usb_commands.ScanFreq()
            s.scan_freq.init(resistorIndex=resistorIndex, VIndex=0, IIndex=index, fileName=fileName)

            s.progress_bar.setRange(0, s.scan_freq.count())
            s.progress_bar.setValue(0)

            while s.scan_freq.next():
                if FormScan.endThread:
                    return
                FormScan.SetInfo()
                pass

            FormScan.SetInfo()
            s.scan_freq.save()
        pass

    @staticmethod
    def CalibrateThreadV():
        '''
        Калибровка коэффициэнтов усиления напряжения сопротивлением 10 Ом
        '''
        s = FormScan.self_ptr

        resistorIndex = 0
        prefix = 'cor/KV'+str(resistorIndex)

        for index in xrange(7):
            fileName = prefix+'_'+str(index)+'.json'
            s.header_label.setText(fileName)
            s.scan_freq = usb_commands.ScanFreq()
            s.scan_freq.init(resistorIndex=resistorIndex, VIndex=index, IIndex=0, fileName=fileName)

            s.progress_bar.setRange(0, s.scan_freq.count())
            s.progress_bar.setValue(0)

            while s.scan_freq.next():
                if FormScan.endThread:
                    return
                FormScan.SetInfo()
                pass

            FormScan.SetInfo()
            s.scan_freq.save()
        pass

    @staticmethod
    def SetInfo():
        s = FormScan.self_ptr
        s.progress_bar.setValue(s.scan_freq.current())
        jout = s.scan_freq.jfreq[-1]
        data = calculateJson(jout)
        info = ''
        info += 'F=' + str(int(data['F']))
        info += '\n' + 'R='+usb_commands.getResistorValueStr(usb_commands.resistorIdx)
        info += '\n' + 'KU='+str(usb_commands.getGainValueV(usb_commands.gainVoltageIdx))+'x'
        info += ' KI='+str(usb_commands.getGainValueI(usb_commands.gainCurrentIdx))+'x'
        info += '\n' + 'Rre='+str(formatR(data['Rre']))
        info += '\n' + 'Rim='+str(formatR(data['Rim']))

        s.info_label.setText(info)
        pass

class FormCalibrationResistor(QtGui.QMainWindow):

    def __init__(self, parent=None):
        super(FormCalibrationResistor, self).__init__(parent)

        self.diapazon = []
        self.diapazon.append({'diapazon':0, 'value':1e2})
        self.diapazon.append({'diapazon':1, 'value':1e3})
        self.diapazon.append({'diapazon':2, 'value':1e4})
        self.diapazon.append({'diapazon':3, 'value':1e5})
        self.labels  = [None]*4
        self.edits  = [None]*4

        self.setWindowTitle(TITLE)
        self.CreateMainFrame()
        pass

    def CreateMainFrame(self):
        self.main_frame = QtGui.QWidget()
        vbox = QtGui.QVBoxLayout()

        header_label = QtGui.QLabel(u'Калибровка')

        vbox.addWidget(header_label)
        self.AddLine(vbox, u'100 Ом', self.On100_Om, 0)
        self.AddLine(vbox, u'1 KОм', self.On1_KOm, 1)
        self.AddLine(vbox, u'10 KОм', self.On10_KOm, 2)
        self.AddLine(vbox, u'100 KОм', self.On100_KOm, 3)

        button_save = QtGui.QPushButton(u'Записать')
        button_save.clicked.connect(self.OnSave)
        vbox.addWidget(button_save)

        button_save = QtGui.QPushButton(u'Калиб!')
        button_save.clicked.connect(self.OnCalibrateK)
        vbox.addWidget(button_save)

        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)
        pass

    def AddLine(self, vbox, name, func, interval):
        hbox = QtGui.QHBoxLayout()

        label1 = QtGui.QLabel(u'Точное значение сопротивления ' + name + '=')
        hbox.addWidget(label1)
        edit = QtGui.QLineEdit()
        validator = QtGui.QDoubleValidator()
        validator.setRange(90, 250)
        edit.setValidator(validator)
        text = self.diapazon[interval]['value']
        edit.setText(str(text))
        hbox.addWidget(edit)
        self.edits[interval] = edit

        button = QtGui.QPushButton(name)
        button.clicked.connect(func)
        hbox.addWidget(button)

        label = QtGui.QLabel(u'Не пройден')
        self.labels[interval] = label
        label.setStyleSheet("QLabel { color : red; }");
        hbox.addWidget(label)

        vbox.addLayout(hbox)
        pass

    def On100_Om(self):
        self.OnProcess(0)
        pass

    def On1_KOm(self):
        self.OnProcess(1)
        pass

    def On10_KOm(self):
        self.OnProcess(2)
        pass

    def On100_KOm(self):
        self.OnProcess(3)
        pass

    def OnProcess(self, interval):
        print "OnProcess=", interval
        self.diapazon[interval]['Rreal'] = float(self.edits[interval].text())
        self.process(interval, lowPass = False)
        self.process(interval, lowPass = True)
        pass 
    def process(self, interval, lowPass):
        print "lowPass = ", lowPass
        if lowPass:
            freq = 1000
        else:
            freq = 3000
        period = usb_commands.periodByFreq(freq)
        jout = usb_commands.oneFreq(period, lowPass=lowPass)
        result = calculateJson(jout, correctR=False)
        Rre = result['Rre']
        Rim = result['Rim']
        print 'Rre=', Rre
        print 'Rim=', Rim
        print 'result=', result

        if lowPass:
            label = self.labels[interval]
            label.setText(formatR(Rre))
            label.setStyleSheet("QLabel { color : green; }");

        if lowPass:
            self.diapazon[interval]['Rlow'] = Rre
        else:
            self.diapazon[interval]['Rhigh'] = Rre
        pass

    def OnSave(self):
        print "OnSave"
        f = open('cor/res.json', 'w')
        f.write(json.dumps(self.diapazon))
        f.close()
        pass
    def OnCalibrateK(self):
        form = FormScan(self)
        form.startCalibrate()
        form.show()
        pass

def makePhase():
    '''
    Вычисляем коэффициэнты для коррекции фазы по open/short файлам.
    '''
    freq_open = readJson('cor/freq_open.json')
    freq_short = readJson('cor/freq_short.json')
    jfreq_open = freq_open['freq']
    jfreq_short = freq_short['freq']
    jout = []

    for idx in xrange(len(jfreq_open)):
        res_open = calculateJson(jfreq_open[idx], correctR=False)
        res_short = calculateJson(jfreq_short[idx], correctR=False)
        assert res_open['F']==res_short['F']
        d = {
            'period' : jfreq_open[idx]['attr']['period'],
            'fiV' : res_open['fiV'],
            'fiI' : res_short['fiI']
            }
        jout.append(d)

    f = open('cor/phase.json', 'w')
    f.write(json.dumps(jout))
    f.close()
    pass

def main():
    #makePhase()
    #return
    app = QtGui.QApplication(sys.argv)
    form = FormMain()
    form.show()
    app.exec_()


if __name__ == "__main__":
    main()
    
