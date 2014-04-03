# -*- coding: UTF-8 -*-
# balmer@inbox.ru 2014 RLC Meter
import sys, os, csv
from PyQt4 import QtCore, QtGui

import matplotlib
import time
import math
import cmath
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar
from matplotlib.figure import Figure
import jplot


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
		self.gtype_combo_box.addItem(u'C', QtCore.QVariant('C'))
		self.gtype_combo_box.addItem(u'L', QtCore.QVariant('L'))
		self.gtype_combo_box.addItem(u'Error', QtCore.QVariant('error'))
		self.gtype_combo_box.addItem(u'Q (corrected)', QtCore.QVariant('dfic'))
		self.gtype_combo_box.addItem(u'Q (uncorrected)', QtCore.QVariant('dfi'))
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

		corr_gain = jplot.GainCorrector()
		corr = jplot.Corrector(corr_gain)

		for jf in jfreq:
			#res = jplot.calculateJson(jf, gain_corrector=corr_gain)
			(res, Zx) = corr.calculateJson(jf)
			F = res['F']
			f_data.append(F)

			ampV.append(res['ampV'])
			ampI.append(res['ampI'])
			#re_data.append(math.fabs(res['Rre']))
			#im_data.append(math.sqrt(res['Rre']**2+res['Rim']**2))
			re_data.append(res['Rre'])
			#im_data.append(math.fabs(res['Rim']))
			im_data.append(res['Rim'])
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
				#Zx = complex(res['Rre'], res['Rim'])
				#Zx = corr.correct(res['Rre'], res['Rim'], res['period'], F, jf['attr']['resistor_index'])
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
				#Zx = complex(res['Rre'], res['Rim'])
				#Zx = corr.correct(res['Rre'], res['Rim'], res['period'], F, jf['attr']['resistor_index'])
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
		if gtype=="dfi" or gtype=="dfic":
			ax.plot (f_data, dfi_data, '-', color="green")

		if gtype=="error":
			ax.plot (f_data, re_error, '.', color="red")
			ax.plot (f_data, im_error, '.-', color="blue")

		#ax.plot (f_data, im_sin, '.', color="red")
		#ax.plot (f_data, im_cos, '.-', color="blue")

		if gtype=="ReImCorrect":
			ax.set_ylabel("Om")
			ax.plot (f_data, re_corr, '-', color="#00FF00")
			ax.plot (f_data, im_corr, '-', color="#555555")

		if gtype=="C":
			ax.set_ylabel("pF")
			ax.plot (f_data, arr_C, '-', color="red")

		if gtype=="L":
			ax.set_ylabel("uH")
			ax.plot (f_data, arr_L, '-', color="red")


		pass
