# -*- coding: UTF-8 -*-
# balmer@inbox.ru 2014 RLC Meter
import sys, os, csv
from PyQt4 import QtCore, QtGui

import matplotlib
import time
import datetime
import usb_commands
import threading

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

        header_label = QtGui.QLabel(
            u'Измеритель комплексного сопротивления'
            )

        vbox.addWidget(header_label)

        scan_button = QtGui.QPushButton(u'Просканировать диапазон')
        scan_button.clicked.connect(self.OnScan)
        vbox.addWidget(scan_button)

        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)
        pass

    def OnScan(self):
        form = FormScan(self)
        if not usb_commands.inited():
            return

        form.show()
        pass

class FormScan(QtGui.QMainWindow):
    self_ptr = None
    scan_freq = None
    endThread = False

    def __init__(self, parent=None):
        super(FormScan, self).__init__(parent)
        self.setWindowTitle(TITLE)

        if not usb_commands.initDevice():
            QtGui.QMessageBox.about(self, TITLE, u"Устройство не найдено.")
            return
        self.CreateMainFrame()

        FormScan.self_ptr = self
        FormScan.scan_freq = usb_commands.ScanFreq()
        FormScan.scan_freq.init()

        FormScan.progress_bar.setRange(0, FormScan.scan_freq.count())
        FormScan.progress_bar.setValue(0)

        FormScan.endThread = False
        self.th = threading.Thread(target=FormScan.UsbThread, args=[])
        self.th.start()
        pass

    def CreateMainFrame(self):
        self.main_frame = QtGui.QWidget()
        vbox = QtGui.QVBoxLayout()

        header_label = QtGui.QLabel(u'Сканирование диапазона.')

        vbox.addWidget(header_label)

        FormScan.progress_bar = QtGui.QProgressBar()

        vbox.addWidget(self.progress_bar)

        self.main_frame.setLayout(vbox)
        self.setCentralWidget(self.main_frame)
        pass

    def closeEvent(self, event):
        print("FormScan: close event")
        FormScan.endThread = True
        event.accept()

        pass

    @staticmethod
    def UsbThread():
        s = FormScan.self_ptr
        while s.scan_freq.next():
            if FormScan.endThread:
                return
            s.progress_bar.setValue(s.scan_freq.current())
            pass
            
        s.progress_bar.setValue(s.scan_freq.current())
        s.scan_freq.save()
        pass


def main():
    app = QtGui.QApplication(sys.argv)
    form = FormMain()
    form.show()
    app.exec_()


if __name__ == "__main__":
    main()
    
