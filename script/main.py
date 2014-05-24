# -*- coding: UTF-8 -*-
# balmer@inbox.ru 2014 RLC Meter
import sys, os, csv
from PyQt4 import QtCore, QtGui

import matplotlib
import time
import datetime
import threading
import json
import os.path

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

        scan_button = QtGui.QPushButton(u'Измерить')
        scan_button.clicked.connect(self.OnMeasure)
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

    def OnMeasure(self):
        if not self.initDevice():
            return
        form = plot.FormMeasure(TITLE, self)
        form.show()
        pass

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

def getCorrName(resistorIndex, Rname):
    return 'cor/D'+str(resistorIndex)+'_'+Rname+'.json'



class FormScan(QtGui.QMainWindow):

    def __init__(self, parent=None):
        super(FormScan, self).__init__(parent)
        self.setWindowModality(QtCore.Qt.WindowModal)
        self.end_thread = False

        self.setWindowTitle(TITLE)
        self.CreateMainFrame()
        pass

    def startDefault(self, parent_self=None, filename='freq.json'):
        self.scan_freq = usb_commands.ScanFreq()
        self.scan_freq.init(fileName=filename)

        self.progress_bar.setRange(0, self.scan_freq.count())
        self.progress_bar.setValue(0)

        self.th = threading.Thread(target=FormScan.UsbThread, args=[self, parent_self])
        self.th.start()
        pass

    def startCalibrateIV(self, parent_self=None, calibrateV=True):
        if calibrateV:
            self.th = threading.Thread(target=FormScan.CalibrateThreadV, args=[self, parent_self])
        else:
            self.th = threading.Thread(target=FormScan.CalibrateThreadI, args=[self, parent_self])
        self.th.start()
        pass

    def startCalibrateR(self, parent_self, R, resistorIndexes, Rname):
        '''
        resistorIndexes - диапазон на котором производится измерение
        Rindex - индекс резистора для getCorrName
        '''
        self.th = threading.Thread(target=FormScan.CalibrateThreadR, args=[self, parent_self, R, resistorIndexes, Rname])
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

        button_close = QtGui.QPushButton(u'Отменить.')
        button_close.clicked.connect(self.close)
        vbox.addWidget(button_close)

        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)
        pass

    def closeEvent(self, event):
        self.end_thread = True
        event.accept()
        pass

    @staticmethod
    def UsbThread(self_ptr, parent_self):
        s = self_ptr
        while s.scan_freq.next():
            if s.end_thread:
                return
            s.SetInfo()
            pass

        s.SetInfo()
        s.scan_freq.save()
        s.close()
        if parent_self:
            parent_self.OnCompleteProcess()
        pass

    @staticmethod
    def CalibrateThreadI(self_ptr, parent_self):
        '''
        Калибровка коэффициэнтов усиления тока сопротивлением 1 КОм
        '''
        s = self_ptr

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
                if s.end_thread:
                    return
                s.SetInfo()
                pass

            s.SetInfo()
            s.scan_freq.save()
        s.close()
        if parent_self:
            parent_self.OnCompleteProcess()
        pass

    @staticmethod
    def CalibrateThreadV(self_ptr, parent_self):
        '''
        Калибровка коэффициэнтов усиления напряжения сопротивлением 10 Ом
        '''
        s = self_ptr

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
                if s.end_thread:
                    return
                s.SetInfo()
                pass

            s.SetInfo()
            s.scan_freq.save()
        s.close()
        if parent_self:
            parent_self.OnCompleteProcess()
        pass

    @staticmethod
    def CalibrateThreadR(self_ptr, parent_self, R, resistorIndexes, Rname):
        s = self_ptr

        for resistorIndex in resistorIndexes:
            fileName = getCorrName(resistorIndex, Rname)
            s.header_label.setText(fileName)
            s.scan_freq = usb_commands.ScanFreq()
            s.scan_freq.init(resistorIndex=resistorIndex, fileName=fileName)

            s.progress_bar.setRange(0, s.scan_freq.count())
            s.progress_bar.setValue(0)

            while s.scan_freq.next():
                if s.end_thread:
                    return
                s.SetInfo()
                pass

            s.SetInfo()
            s.scan_freq.jout['R'] = R
            s.scan_freq.save()

        s.close()
        if parent_self:
            parent_self.OnCompleteProcess()
        pass

    def SetInfo(self):
        s = self
        s.progress_bar.setValue(s.scan_freq.current())
        jout = s.scan_freq.jfreq[-1]
        data = calculateJson(jout)
        info = ''
        info += 'F=' + str(int(data['F']))
        info += '\n' + 'R='+usb_commands.getResistorValueStr(usb_commands.resistorIdx)
        info += '\n' + 'KU='+str(usb_commands.getGainValueV(usb_commands.gainVoltageIdx))+'x'
        info += ' KI='+str(usb_commands.getGainValueI(usb_commands.gainCurrentIdx))+'x'
        info += '\n' + 'Rre='+str(formatR(data['R'].real))
        info += '\n' + 'Rim='+str(formatR(data['R'].imag))

        s.info_label.setText(info)
        pass

class FormCalibrationResistor(QtGui.QMainWindow):

    def __init__(self, parent=None):
        super(FormCalibrationResistor, self).__init__(parent)

        self.setWindowModality(QtCore.Qt.WindowModal)

        self.diapazon = []
        self.diapazon.append({'diapazon':0, 'value':1e2})
        self.diapazon.append({'diapazon':1, 'value':1e3})
        self.diapazon.append({'diapazon':2, 'value':1e4})
        self.diapazon.append({'diapazon':3, 'value':1e5})
        self.labels  = [None]*4
        self.edits  = [None]*4
        self.labelOS = {}

        self.setWindowTitle(TITLE)
        self.CreateMainFrame()
        self.checkComplete()
        pass

    def CreateMainFrame(self):
        self.main_frame = QtGui.QWidget()
        vbox = QtGui.QVBoxLayout()

        header_label = QtGui.QLabel(u'Калибровка')

        vbox.addWidget(header_label)
        self.AddLine1Om(vbox)
        self.AddLine(vbox, u'100 Ом', self.On100_Om, 0)
        self.AddLine(vbox, u'1 KОм', self.On1_KOm, 1)
        self.AddLine(vbox, u'10 KОм', self.On10_KOm, 2)
        self.AddLine(vbox, u'100 KОм', self.On100_KOm, 3)

        self.AddLineV(vbox)
        self.AddLineI(vbox)
        self.AddLineOpenShort(vbox, u'Замкнутые щупы', 'short')
        self.AddLineOpenShort(vbox, u'Открытые щупы', 'open')

        button_close = QtGui.QPushButton(u'Закрыть')
        button_close.clicked.connect(self.close)
        vbox.addWidget(button_close)

        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)
        pass

    def AddLine(self, vbox, name, func, interval, overrideValue = None):
        hbox = QtGui.QHBoxLayout()

        label1 = QtGui.QLabel(u'Точное значение сопротивления ' + name + '=')
        hbox.addWidget(label1)
        edit = QtGui.QLineEdit()
        #validator = QtGui.QDoubleValidator()
        #validator.setRange(90, 250)
        #edit.setValidator(validator)
        if overrideValue:
            text = overrideValue
        else:
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

    def AddLine1Om(self, vbox):
        name = u'1 Ом'
        value = 1.0
        func = self.On1_Om

        hbox = QtGui.QHBoxLayout()

        label1 = QtGui.QLabel(u'Точное значение сопротивления ' + name + '=')
        hbox.addWidget(label1)
        edit = QtGui.QLineEdit()
        edit.setText(str(value))
        hbox.addWidget(edit)
        self.edit1Om = edit

        button = QtGui.QPushButton(name)
        button.clicked.connect(func)
        hbox.addWidget(button)

        label = QtGui.QLabel(u'Не пройден')
        self.label1Om = label
        label.setStyleSheet("QLabel { color : red; }");
        hbox.addWidget(label)

        vbox.addLayout(hbox)
        pass

    def AddLineV(self, vbox):
        hbox = QtGui.QHBoxLayout()
        label1 = QtGui.QLabel(u'Подключите сопротивление примерно 10 Ом')
        hbox.addWidget(label1)
        button = QtGui.QPushButton(u'Пуск.')
        button.clicked.connect(self.OnCalibrateV)
        hbox.addWidget(button)
        label = QtGui.QLabel(u'XXX')
        self.labelV = label
        hbox.addWidget(label)
        vbox.addLayout(hbox)
        pass

    def AddLineI(self, vbox):
        hbox = QtGui.QHBoxLayout()
        label1 = QtGui.QLabel(u'Подключите сопротивление примерно 1 КОм')
        hbox.addWidget(label1)
        button = QtGui.QPushButton(u'Пуск.')
        button.clicked.connect(self.OnCalibrateI)
        hbox.addWidget(button)
        label = QtGui.QLabel(u'XXX')
        self.labelI = label
        hbox.addWidget(label)
        vbox.addLayout(hbox)
        pass

    def AddLineOpenShort(self, vbox, title, name):
        hbox = QtGui.QHBoxLayout()
        label1 = QtGui.QLabel(title)
        hbox.addWidget(label1)
        button = QtGui.QPushButton(u'Пуск.')
        button.clicked.connect(lambda: self.OnCalibrateOpenShort(name))
        hbox.addWidget(button)
        label = QtGui.QLabel(u'XXX')
        self.labelOS[name] = label
        hbox.addWidget(label)
        vbox.addLayout(hbox)
        pass

    def On100_Om(self):
        R = float(self.edits[0].text())
        self.process(R, [0], "100Om")
        pass

    def On1_Om(self):
        R = float(self.edit1Om.text())
        self.process(R, [0], "1Om")
        pass

    def On1_KOm(self):
        R = float(self.edits[1].text())
        self.process(R, [0, 1], "1KOm")
        pass

    def On10_KOm(self):
        R = float(self.edits[2].text())
        self.process(R, [1, 2], "10KOm")
        pass

    def On100_KOm(self):
        R = float(self.edits[3].text())
        self.process(R, [2, 3], "100KOm")
        pass

    def process(self, R, diapason, name):
        form = FormScan(self)
        form.startCalibrateR(self, R, diapason, name)
        form.show()
        pass

    def OnCompleteProcess(self):
        self.checkComplete()
        pass

    def OnCalibrateV(self):
        form = FormScan(self)
        form.startCalibrateIV(parent_self=self, calibrateV=True)
        form.show()
        pass

    def OnCalibrateI(self):
        form = FormScan(self)
        form.startCalibrateIV(parent_self=self, calibrateV=False)
        form.show()
        pass

    def OnCalibrateOpenShort(self, name):
        filename = 'cor/K_'+name+'.json'
        form = FormScan(self)
        form.startDefault(parent_self=self, filename=filename)
        form.show()
        pass

    def setComplete(self, label, ok):
        if ok:
            label.setText(u"Пройден.")
            label.setStyleSheet("QLabel { color : green; }");
        else:
            label.setText(u'Не пройден')
            label.setStyleSheet("QLabel { color : red; }");
        pass

    def checkCompleteOne(self, diapasons, Rname):
        ok = True
        for diapason in diapasons:
            filename = getCorrName(diapason, Rname)
            if not os.path.isfile(filename):
                ok = False
                break

        label = self.labels[diapasons[-1]]
        self.setComplete(label, ok)
        pass

    def checkComplete1Om(self):
        diapason = 0
        Rname = '1Om'
        filename = getCorrName(diapason, Rname)
        ok = os.path.isfile(filename)

        label = self.label1Om
        self.setComplete(label, ok)
        pass

    def checkCompleteIV(self, IV):
        kmul = [0,1,2,3,4,5,6]
        if IV=='I':
            label = self.labelI
        else:
            label = self.labelV

        ok = True
        for kindex in kmul:
            filename = 'cor/K'+IV+'0_'+str(kindex)+'.json'
            if not os.path.isfile(filename):
                ok = False
                break

        self.setComplete(label, ok)
        pass

    def checkCompleteOpenShort(self, name):
        ok = os.path.isfile('cor/K_'+name+'.json')
        label = self.labelOS[name]
        self.setComplete(label, ok)
        pass

    def checkComplete(self):
        self.checkComplete1Om()
        self.checkCompleteOne([0], "100Om")
        self.checkCompleteOne([0, 1], "1KOm")
        self.checkCompleteOne([1, 2], "10KOm")
        self.checkCompleteOne([2, 3], "100KOm")
        self.checkCompleteIV('I')
        self.checkCompleteIV('V')

        self.checkCompleteOpenShort('open')
        self.checkCompleteOpenShort('short')
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
    
