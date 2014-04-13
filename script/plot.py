# -*- coding: UTF-8 -*-
# balmer@inbox.ru 2014 RLC Meter
import sys, os, csv
from PyQt4 import QtCore, QtGui
import threading

import matplotlib
import time
import math
import cmath
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar
from matplotlib.figure import Figure
import jplot
import usb_commands


class FormDrawData(QtGui.QMainWindow):
	def __init__(self, title, parent=None):
		super(FormDrawData, self).__init__(parent)
		self.setWindowTitle(title)

		self.gtype = 'ReImCorrect'
		self.serial = True

		self.createMainFrame()
		pass

	def createMainFrame(self):
		self.main_frame = QtGui.QWidget()

		plot_frame = QtGui.QWidget()

		self.dpi = 100
		self.fig = Figure((6.0, 4.0), dpi=self.dpi)
		self.canvas = FigureCanvas(self.fig)
		self.canvas.setParent(self.main_frame)

		self.axes = self.fig.add_subplot(111)
		self.mpl_toolbar = NavigationToolbar(self.canvas, self.main_frame)

		left_vbox = QtGui.QVBoxLayout()
		left_vbox.addWidget(self.canvas)
		left_vbox.addWidget(self.mpl_toolbar)

		right_vbox = QtGui.QVBoxLayout()

		self.gtype_combo_box = QtGui.QComboBox()
		self.gtype_combo_box.addItem(u'Re+Im', QtCore.QVariant('ReImCorrect'))
		self.gtype_combo_box.addItem(u'Re', QtCore.QVariant('ReCorrect'))
		self.gtype_combo_box.addItem(u'Im', QtCore.QVariant('ImCorrect'))
		self.gtype_combo_box.addItem(u'C', QtCore.QVariant('C'))
		self.gtype_combo_box.addItem(u'L', QtCore.QVariant('L'))
		self.gtype_combo_box.addItem(u'Error', QtCore.QVariant('error'))
		self.gtype_combo_box.addItem(u'Q (corrected)', QtCore.QVariant('dfic'))
		self.gtype_combo_box.addItem(u'Q (uncorrected)', QtCore.QVariant('dfi'))
		self.gtype_combo_box.addItem(u'Re+Im (Raw)', QtCore.QVariant('ReImRaw'))
		self.gtype_combo_box.addItem(u'Re+Im (OnlyGainCorrector)', QtCore.QVariant('ReImRawG'))
		
		self.gtype_combo_box.currentIndexChanged.connect(self.OnSelectGraph)
		right_vbox.addWidget(self.gtype_combo_box)

		self.serial_combo_box = QtGui.QComboBox()
		self.serial_combo_box.addItem(u'Serial')
		self.serial_combo_box.addItem(u'Parralel')
		self.serial_combo_box.currentIndexChanged.connect(self.OnSerial)
		right_vbox.addWidget(self.serial_combo_box)

		right_vbox.addStretch(1)

		hbox = QtGui.QHBoxLayout()
		hbox.addLayout(left_vbox, stretch=1)
		hbox.addLayout(right_vbox)
		self.main_frame.setLayout(hbox)

		self.setCentralWidget(self.main_frame)

	def setData(self, filename):
		self.filename = filename
		self.updateFigure(filename)

	def updateFigure(self, filename):
		self.axes.clear()
		self.axes.grid(True)

		#xlist = [1,2,3,4,5]
		#ylist = [5,3,7,9,12]
		#ax1 = self.axes
		#ax1.plot(xlist, ylist, 'r')
		self.plotFreq(filename)

		self.fig.autofmt_xdate()
		self.fig.canvas.draw()
		pass

	def readPhase(self):
		jphase = jplot.readJson('cor/phase.json')
		set_phase = {}
		for p in jphase:
			set_phase[p['period']] = p
		return set_phase

	def OnSelectGraph(self, index):
		self.gtype = self.gtype_combo_box.itemData(index).toString()
		self.updateFigure(self.filename)
		pass

	def OnSerial(self, index):
		if index==0:
			self.serial = True
		else:
			self.serial = False
		self.updateFigure(self.filename)
		pass

	def plotFreq(self, fileName):
		gtype = self.gtype
		ax = self.axes
		jout = jplot.readJson(fileName)
		set_phase = self.readPhase()
		jfreq = jout['freq']

		f_data = []
		re_data = []
		im_data = []
		dfi_data = []
		re_error = []
		im_error = []
		re_corr = []
		im_corr = []
		arr_L = []
		arr_C = []

		im_sin = []
		im_cos = []
		fiV_data = []
		fiI_data = []
		ampV = []
		ampI = []

		if gtype=="ReImRaw":
			corr_gain = None
		else:
			corr_gain = jplot.GainCorrector()
		corr = jplot.Corrector(corr_gain)

		for jf in jfreq:
			res = corr.calculateJson(jf)
			F = res['F']
			Zx = res['Zx']
			f_data.append(F)

			ampV.append(res['ampV'])
			ampI.append(res['ampI'])
			#re_data.append(math.fabs(res['R'].real))
			#im_data.append(abs(res['R']))
			re_data.append(res['R'].real)
			#im_data.append(math.fabs(res['R'].imag))
			im_data.append(res['R'].imag)
			re_error.append(jf['summary']['V']['square_error'])
			im_error.append(jf['summary']['I']['square_error'])

			gain_I = jf['attr']["gain_I"]
			gain_V = jf['attr']["gain_V"]
			#re_error.append(math.sqrt(jf['summary']['V']['sin']**2+jf['summary']['V']['cos']**2)/gain_V)
			#im_error.append(math.sqrt(jf['summary']['I']['sin']**2+jf['summary']['I']['cos']**2)/gain_I)

			im_sin.append(jf['summary']['I']['sin']/gain_I)
			im_cos.append(jf['summary']['I']['cos']/gain_I)

			#dfi_data.append(res['dfi']*1e6/F)
			if gtype=='dfic':
				dfi_data.append(cmath.phase(Zx)*180/math.pi)
			if gtype=='dfi':
				dfi_data.append(res['dfi']*180/math.pi)
			
			fiV_data.append(res['fiV'])
			fiI_data.append(res['fiI'])

			if self.serial:
				re_corr.append(Zx.real)
				#re_corr.append(res['ampV']/res['ampI'])
				#im_corr.append(math.fabs(Zx.imag))
				im_corr.append(Zx.imag)
				
				if Zx.imag>0:
					L = Zx.imag/(2*math.pi*F)
				else:
					L = 0

				if Zx.imag<-1e-10:
					C = -1/(2*math.pi*F*Zx.imag)
					#C = min(C, 1e-6)
				else:
					C = 0
				arr_L.append(L*1e6)
				arr_C.append(C*1e12)

			if not self.serial: #parrallel
				Yx = 1/Zx

				im_max = 1e10
				if Yx.real < 1/im_max:
					re_corr.append(im_max)
				else:
					re_corr.append(1/Yx.real)

				if math.fabs(Yx.imag)*im_max>1:
					im_corr.append(1/Yx.imag)
				else:
					if Yx.imag>0:
						im_corr.append(im_max)
					else:
						im_corr.append(-im_max)

				C = Yx.imag/(2*math.pi*F)
				#C = 1.0/(2*math.pi*F*res["resistance"])
				C = min(C, 1e-6)
				C = max(C, -1e-6)
				arr_C.append(C*1e12)

				if Yx.imag<0:
					L = -1/(2*math.pi*F*Yx.imag)
				else:
					L = 0
				arr_L.append(L*1e6)



		#ax.set_title("1 uF 160 V")
		ax.set_xscale('log')
		#ax.set_yscale('log')
		ax.set_xlabel("Hz")

		#ax.set_ylabel("Om")
		#ax.plot (f_data, ampV, '-', color="red")
		#ax.plot (f_data, ampI, '-', color="blue")
		#ax.plot (f_data, re_data, '-', color="red")
		#ax.plot (f_data, im_data, '-', color="blue")
		#ax.plot (f_data, fiV_data, '-', color="red")
		#ax.plot (f_data, fiI_data, '-', color="blue")
		#ax.plot (f_data, im_sin, '.', color="red")
		#ax.plot (f_data, im_cos, '.-', color="blue")

		if gtype=="dfi" or gtype=="dfic":
			ax.plot (f_data, dfi_data, '-', color="green")

		if gtype=="error":
			ax.plot (f_data, re_error, '.', color="red")
			ax.plot (f_data, im_error, '.-', color="blue")

		if gtype=="ReImCorrect" or gtype=="ReCorrect" or gtype=="ImCorrect":
			ax.set_ylabel("Om")
			if gtype=="ReImCorrect" or gtype=="ReCorrect":
				ax.plot (f_data, re_corr, '-', color="red")
			if gtype=="ReImCorrect" or gtype=="ImCorrect":
				ax.plot (f_data, im_corr, '-', color="blue")

		if gtype=="ReImRaw" or gtype=="ReImRawG":
			ax.set_ylabel("Om")
			ax.plot (f_data, re_data, '-', color="red")
			#ax.plot (f_data, im_data, '-', color="blue")

		if gtype=="C":
			ax.set_ylabel("pF")
			ax.plot (f_data, arr_C, '-', color="red")

		if gtype=="L":
			ax.set_ylabel("uH")
			ax.plot (f_data, arr_L, '-', color="red")
		pass

class FormMeasure(QtGui.QMainWindow):
	def __init__(self, title, parent=None):
		super(FormMeasure, self).__init__(parent)
		self.setWindowTitle(title)
		self.serial = True
		self.end_thread = False
		self.createMainFrame()

		self.th = threading.Thread(target=self.UsbThread)
		self.th.start()
		self.corr_gain = jplot.GainCorrector()
		self.corr = jplot.Corrector(self.corr_gain)
		pass

	def createMainFrame(self):
		self.main_frame = QtGui.QWidget()
		vbox = QtGui.QVBoxLayout()

		self.periods = usb_commands.periodAll()
		self.period = self.periods[0]

		self.freq_slider = QtGui.QSlider(QtCore.Qt.Horizontal)
		self.freq_slider.setMinimumWidth(256)
		self.freq_slider.valueChanged.connect(self.OnSliderValueChanged)
		self.freq_slider.sliderReleased.connect(self.OnSliderReleased)
		self.freq_slider.setRange (0, len(self.periods)-1)
		vbox.addWidget(self.freq_slider)

		self.freq_label = QtGui.QLabel(u'info');
		vbox.addWidget(self.freq_label);
		self.OnSliderValueChanged(self.freq_slider.value())


		self.serial_combo_box = QtGui.QComboBox()
		self.serial_combo_box.addItem(u'Serial')
		self.serial_combo_box.addItem(u'Parralel')
		self.serial_combo_box.currentIndexChanged.connect(self.OnSerial)
		vbox.addWidget(self.serial_combo_box)

		self.info_label = QtGui.QLabel(u'info');
		vbox.addWidget(self.info_label);

		button_close = QtGui.QPushButton(u'Закончить.')
		button_close.clicked.connect(self.close)
		vbox.addWidget(button_close)

		self.main_frame.setLayout(vbox)
		self.setCentralWidget(self.main_frame)
		pass

	def OnSerial(self, index):
		if index==0:
			self.serial = True
		else:
			self.serial = False
		pass

	def OnFrequency(self, index):
		self.period = self.freq_combo_box.itemData(index).toString()
		pass

	def OnSliderValueChanged (self, index):
		self.freq_slider.setValue(index)
		period = self.periods[index]
		F = usb_commands.periodToFreqency(period)
		self.freq_label.setText(str(int(F))+' Hz');
		pass

	def OnSliderReleased(self):
		index = self.freq_slider.value()
		self.period = self.periods[index]
		pass

	def closeEvent(self, event):
		print "closeEvent"
		self.end_thread = True
		event.accept()
		pass

	def UsbThread(self):
		i = 0
		while not self.end_thread:
			jf = usb_commands.oneFreq(self.period)
			res = self.corr.calculateJson(jf)
			if self.end_thread:
				return
			self.SetInfo(res)
		pass

	def SetInfo(self, res):
		(L, C, isC) = jplot.calculateLC(res, self.serial)
		Zx = res['Zx']
		#uncorrected
		#txt = "Rre=" + jplot.formatR(res['R'].real)
		#txt += "\nRim=" + jplot.formatR(res['R'].imag)
		if self.serial:
			txt = "Rre=" + jplot.formatR(Zx.real)
			txt += "\nRim=" + jplot.formatR(Zx.imag)
		else:
			#parallel
			Yx = 1/Zx
			txt = "Rre=" + jplot.formatR(1/Yx.real)
			txt += "\nRim=" + jplot.formatR(-1/Yx.imag)


		if isC:
			txt += "\nC=" + jplot.formatC(C)
		else:
			txt += "\nL=" + jplot.formatL(L)
		self.info_label.setText(txt)
		pass
