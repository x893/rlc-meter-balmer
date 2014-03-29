# -*- coding: UTF-8 -*-
import array
import math
import cmath
import datetime
import struct
import sys
import json
import smath

import matplotlib
import matplotlib.pyplot as plt
import numpy as np

toVolts = 3.3/4095.0

def formatR(R):
	RA = math.fabs(R)
	if RA<1e-2:
		return '{:3.2f} mOm'.format(R*1e3)
	if RA<1:
		return '{:3.1f} mOm'.format(R*1e3)
	if RA<1e3:
		return '{:3.1f} Om'.format(R)
	if RA<1e6:
		return '{:3.1f} KOm'.format(R*1e-3)
	return '{:3.1f} MOm'.format(R*1e-6)

def printC(C):
	if C>=1:
		print "C=", C, "F"
	elif C>=1e-6:
		print "C=", C*1e6, "mkF"
	elif C>=1e-9:
		print "C=", C*1e9, "nF"
	else:
		print "C=", C*1e12, "pF"


def readJson(filename):
	with open(filename, "rb") as file:
		data = json.load(file)
	return data

def makeTimeList(readableData, xmin, xstep):
	xlist = []
	for i in xrange(0, len(readableData)):
		xlist.append(xmin+i*xstep)
	return xlist

def plot(fileName):
	fig, ax = plt.subplots()
	data = readJson(fileName)

	ax.set_xlabel(data['xlabel'])
	ax.set_ylabel(data['ylabel'])
	ax.set_title(data['title'])
	ax.plot (data['datax'], data['datay'], '-')

	# !!! Покажем окно с нарисованным графиком
	plt.show()

def timePerSample(jout):
	'''
		return time in seconds, per one sample
	'''
	jattr = jout["attr"]
	return float(jattr["clock"])/(jattr["period"]/float(jattr["ncycle"]))

def averagePeriod(data, ncycle):
	adata = [0.0]*ncycle
	num = len(data)/ncycle
	for i in xrange(0, len(data)):
		adata[i%ncycle] += data[i]

	for i in xrange(0, ncycle):
		adata[i] /= num
	return adata

def plotRaw(fileName, IV, average = False):
	fig, ax = plt.subplots()
	jout = readJson(fileName)
	jattr = jout["attr"]
	ncycle = jattr['ncycle']

	ax.set_xlabel("Time")

	if IV=='I':
		ylabel = "Current"
	else:
		ylabel = "Voltage"

	ax.set_ylabel(ylabel)
	per_second = timePerSample(jout)
	dx = 1.0/per_second
	#ax.set_title(data['title'])
	ydata = jout['data'][IV]
	if average:
		ydata = averagePeriod(ydata, ncycle)

	timeList = makeTimeList(ydata, 0, dx)

	ax.plot (timeList, ydata, '-')

	# !!! Покажем окно с нарисованным графиком
	plt.show()

def calcFast(period, clock, ncycle, sdata):
	(amplitude, fi) = smath.calcFi(sdata["sin"], sdata["cos"])
	#(a,f) = cmath.polar(complex(sdata["sin"], sdata["cos"]))
	#print "fi=", fi, "f=", f
	#print "am=", amplitude, "a=", a
	return {"amplitude": amplitude, "fi": fi}



def calculateJson(jout, gain_corrector = None):
	jattr = jout["attr"]
	period = jattr["period"]
	clock = jattr["clock"]
	ncycle = jattr["ncycle"]

	resistor = jattr["resistor"]

	F = clock/period #frequency, herz

	if gain_corrector:
		(ampV, fiV, ampI, fiI)=gain_corrector.calc(period=period, clock=clock, ncycle=ncycle, jout=jout)
	else:
		if 'summary' in jout:
			resultV = calcFast(period=period, clock=clock, ncycle=ncycle, sdata=jout['summary']['V'])
			resultI = calcFast(period=period, clock=clock, ncycle=ncycle, sdata=jout['summary']['I'])
		else:
			resultV = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=jout['data']['V'])
			resultI = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=jout['data']['I'])
		gain_V = jattr["gain_V"]
		gain_I = jattr["gain_I"]
		ampV = resultV['amplitude']
		ampI = resultI['amplitude']
		fiV = resultV['fi']
		fiI = resultI['fi']
		ampV *= toVolts/gain_V
		ampI *= toVolts/gain_I
		print "F=", F, "gain_V=", gain_V, "gain_I=", gain_I

	if fiV<0:
		fiV+=math.pi*2
	if fiI<0:
		fiI+=math.pi*2

	#dfi = resultV['fi']-resultI['fi']
	dfi = fiV-fiI

	if dfi>math.pi:
		dfi -= math.pi*2
	if dfi<-math.pi:
		dfi += math.pi*2


	current = ampI/resistor # current in Ampers

	cRe = math.cos(dfi)
	cIm = math.sin(dfi)

	resistanceComplex = ampV/current
	Rre = resistanceComplex*cRe
	Rim = resistanceComplex*cIm

	return {
		"ampV": ampV,
		"ampI": ampI,
		"cRe": cRe,
		"cIm": cIm,
		"Rre": Rre,
		"Rim": Rim,
		"F": F,
		"dfi": dfi,
		"current": current,
		"resistance": resistanceComplex,
		"period": period,
		"fiV": fiV,
		"fiI": fiI
	}

class GainCorrector:
	'''
	Корректирует неточность усиления из-за переключения диапазонов усиления
	'''
	def __init__(self):
		self.load()
		pass
	def load(self):
		self.jsonsI = []
		self.jsonsV = []
		pr = "cor/K0_";
		for i in xrange(7):
			self.append(self.jsonsI, readJson("cor/KI0_"+str(i)+".json"))
			self.append(self.jsonsV, readJson("cor/KV0_"+str(i)+".json"))

		pass

	def append(self, jsons, json_data):
		data = {}
		jfreq = json_data['freq']
		for jout in jfreq:
			period = jout['attr']['period']
			data[period] = jout
		jsons.append(data)
		pass

	def calc(self, period, clock, ncycle, jout):
		zV = self.calcX('V', self.jsonsV, jout, index=jout['attr']['gain_index_V'], period=period)
		zI = self.calcX('I', self.jsonsI, jout, index=jout['attr']['gain_index_I'], period=period)
		(ampV, fiV) = cmath.polar(zV)
		(ampI, fiI) = cmath.polar(zI)
		return (ampV, fiV, ampI, fiI)

	def calcX(self, IV, jsons, jout, index, period):
		gi = 'gain_'+IV
		zMeasure = self.calcZ(jout['summary'][IV])
		if index>=len(jsons):
			index = len(jsons)-1

		jout0 = jsons[0][period]
		joutX = jsons[index][period]
		z0 = self.calcZ(jout0['summary'][IV]) 
		zX = self.calcZ(joutX['summary'][IV])
		z0 *= toVolts/jout0['attr'][gi]
		zX *= toVolts/joutX['attr'][gi]
		zMeasure *= toVolts/jout['attr'][gi]
		if index!=0:
			print "i"+IV+'=', index, "z0=", z0, "zX=", zX
		return zMeasure*z0/zX
		#return zX

	def calcZ(self, sdata):
		return complex(sdata["sin"], sdata["cos"])
		#return math.sqrt(sdata["sin"]*sdata["sin"]+sdata["cos"]*sdata["cos"])

class Corrector2x:
	def __init__(self, diapazon, gain_corrector = None):
		self.gain_corrector = gain_corrector
		self.load(diapazon)
		pass
	def load(self, diapazon):
		if diapazon==0:
			fname0 = 'cor/D0_100Om.json'
			fname1 = 'cor/D0_1KOm.json'
		elif diapazon==1:
			fname0 = 'cor/D1_1KOm.json'
			fname1 = 'cor/D1_10KOm.json'
		elif diapazon==2:
			fname0 = 'cor/D2_10KOm.json'
			fname1 = 'cor/D2_100KOm.json'

		json_min = readJson(fname0)
		json_max = readJson(fname1)

		data = {}

		jfreq_min = json_min['freq']
		for jf in jfreq_min:
			res = calculateJson(jf, self.gain_corrector)
			data[res['period']] = { 'min': res }

		jfreq_max = json_max['freq']
		for jf in jfreq_max:
			res = calculateJson(jf, self.gain_corrector)
			data[res['period']]['max'] = res

		self.data = data
		self.Rmin = json_min['R']
		self.Rmax = json_max['R']
		self.C = 1.2e-12
		pass

	def correct(self, Rre, Rim, period, F):
		d = self.data[period]
		Z1 = complex(self.Rmin, 0)
		Z2 = complex(self.Rmax, 0)
		Zm1 = complex(d['min']['Rre'] , d['min']['Rim'])
		Zm2 = complex(d['max']['Rre'] , d['max']['Rim'])
		A = (Z2-Z1)/(Zm2-Zm1)
		B = (Z1*Zm2-Z2*Zm1)/(Zm2-Zm1)
		Zxm = complex(Rre , Rim)
		Zx = A*Zxm+B
		return Zx

class CorrectorOpen:
	def __init__(self, gain_corrector = None):
		self.gain_corrector = gain_corrector
		self.load()
		pass
	def load(self):
		json_open = readJson("cor/K_open.json")
		json_load = readJson("cor/D3_100KOm.json")

		data = {}

		jfreq_open = json_open['freq']
		for jf in jfreq_open:
			res = calculateJson(jf)
			data[res['period']] = { 'open': res }

		jfreq_load = json_load['freq']
		for jf in jfreq_load:
			res = calculateJson(jf)
			data[res['period']]['load'] = res

		self.data = data
		self.R = json_load['R']
		self.C = 1.2e-12
		pass

	def correct(self, Rre, Rim, period, F):
		d = self.data[period]
		Zom = complex(d['open']['Rre'] , d['open']['Rim'])
		Zstdm = complex(d['load']['Rre'] , d['load']['Rim'])
		Ystd = complex(1.0/self.R, 2*math.pi*F*self.C)
		Zstd = 1/Ystd
		#Zstd = complex(self.R, 0)
		Zxm = complex(Rre , Rim)
		Zx = Zstd*(1/Zstdm-1/Zom)*Zxm/(1-Zxm/Zom)
		if period==96:
			print "Zsm=", Zsm
			print "Zstdm=", Zstdm
			print "Zstd=", Zstd
			print "Zxm=", Zxm
			print "Zx=", Zx
		return Zx

class CorrectorShort:
	def __init__(self, gain_corrector = None):
		self.gain_corrector = gain_corrector
		self.load()
		pass
	def load(self):
		json_short = readJson("cor/K_short.json")
		json_load = readJson("cor/D0_100Om.json")

		data = {}

		jfreq_short = json_short['freq']
		for jf in jfreq_short:
			res = calculateJson(jf)
			data[res['period']] = { 'short': res }

		jfreq_load = json_load['freq']
		for jf in jfreq_load:
			res = calculateJson(jf)
			data[res['period']]['load'] = res

		self.data = data
		self.R = json_load['R']
		pass

	def correct(self, Rre, Rim, period, F):
		d = self.data[period]
		Zsm = complex(d['short']['Rre'] , d['short']['Rim'])
		Zstdm = complex(d['load']['Rre'] , d['load']['Rim'])
		Zstd = complex(self.R, 0)
		Zxm = complex(Rre , Rim)
		Zx = Zstd/(Zstdm-Zsm)*(Zxm-Zsm)
		if period==720000:
			print "Zsm=", Zsm
			print "Zstdm=", Zstdm
			print "Zstd=", Zstd
			print "Zxm=", Zxm
			print "Zx=", Zx
		return Zx

class Corrector:
	def __init__(self, gain_corrector = None):
		self.gain_corrector = gain_corrector
		self.load()
		pass
	def load(self):
		self.corr_short = CorrectorShort(self.gain_corrector)
		self.corr = []
		self.corr.append(Corrector2x(0, self.gain_corrector))
		self.corr.append(Corrector2x(1, self.gain_corrector))
		self.corr.append(Corrector2x(2, self.gain_corrector))
		self.corr.append(CorrectorOpen(self.gain_corrector))
		pass
	def correct(self, Rre, Rim, period, F, resistor_index):
		if abs(complex(Rre, Rim))<100:
			return self.corr_short.correct(Rre, Rim, period, F)
		return self.corr[resistor_index].correct(Rre, Rim, period, F)


'''
class Corrector:	
	def __init__(self, gain_corrector = None):
		self.gain_corrector = gain_corrector
		self.load()
		pass

	def load(self):		
		#self.load0()
		#self.load1()
		self.load2x()
		#self.load3()
		pass

	def correct(self, Rre, Rim, period, F):
		#return self.correct0(Rre, Rim, period, F)
		#return self.correct1(Rre, Rim, period, F)
		return self.correct2x(Rre, Rim, period, F)
		#return self.correct3(Rre, Rim, period, F)
		#return self.correctParallelCapacitor(Rre, Rim, period, F)

	def load0(self):
		json_short = readJson("cor/freq_short.json")
		json_load = readJson("cor/freq_1Om.json")

		data = {}

		jfreq_short = json_short['freq']
		for jf in jfreq_short:
			res = calculateJson(jf)
			data[res['period']] = { 'short': res }

		jfreq_load = json_load['freq']
		for jf in jfreq_load:
			res = calculateJson(jf)
			data[res['period']]['load'] = res

		self.data = data
		self.R = json_load['R']

		pass

	def load3(self):
		json_open = readJson("cor/freq_open.json")
		json_load = readJson("cor/freq_100KOm.json")

		data = {}

		jfreq_open = json_open['freq']
		for jf in jfreq_open:
			res = calculateJson(jf)
			data[res['period']] = { 'open': res }

		jfreq_load = json_load['freq']
		for jf in jfreq_load:
			res = calculateJson(jf)
			data[res['period']]['load'] = res

		self.data = data
		self.R = json_load['R']
		self.C = 1.2e-12

		pass

	def load1(self):
		json_Z0 = readJson("cor/1_1KOm.json")
		json_Z1 = readJson("cor/1_2200Om.json")
		json_Z2 = readJson("cor/1_10KOm.json")
		#json_Z0 = readJson("cor/freq_1MOm.json")
		#json_Z1 = readJson("cor/freq_2700KOm.json")
		#json_Z2 = readJson("cor/freq_10MOm.json")

		Z0dut = complex(json_Z0['R'], 0)
		Z1dut = complex(json_Z1['R'], 0)
		Z2dut = complex(json_Z2['R'], 0)

		data = {}

		jfreq_Z0 = json_Z0['freq']
		jfreq_Z1 = json_Z1['freq']
		jfreq_Z2 = json_Z2['freq']
		for i in xrange(0, len(jfreq_Z0)):
			res0 = calculateJson(jfreq_Z0[i], self.gain_corrector)
			res1 = calculateJson(jfreq_Z1[i], self.gain_corrector)
			res2 = calculateJson(jfreq_Z2[i], self.gain_corrector)
			Z0m = complex(res0['Rre'], res0['Rim'])
			Z1m = complex(res1['Rre'], res1['Rim'])
			Z2m = complex(res2['Rre'], res2['Rim'])

			ABCD = self.calcABCD(Z0dut, Z0m, Z1dut, Z1m, Z2dut, Z2m)

			data[res0['period']] = ABCD

		self.data = data

		pass

	def load2x(self):
		json_min = readJson("cor/1_1KOm.json")
		json_max = readJson("cor/1_10KOm.json")
		#json_min = readJson("cor/0_100Om.json")
		#json_max = readJson("cor/0_1KOm.json")

		data = {}

		jfreq_min = json_min['freq']
		for jf in jfreq_min:
			res = calculateJson(jf, self.gain_corrector)
			data[res['period']] = { 'min': res }

		jfreq_max = json_max['freq']
		for jf in jfreq_max:
			res = calculateJson(jf, self.gain_corrector)
			data[res['period']]['max'] = res

		self.data = data
		self.Rmin = json_min['R']
		self.Rmax = json_max['R']
		self.C = 1.2e-12

		pass
	def correct2x(self, Rre, Rim, period, F):
		d = self.data[period]
		Z1 = complex(self.Rmin, 0)
		Z2 = complex(self.Rmax, 0)
		Zm1 = complex(d['min']['Rre'] , d['min']['Rim'])
		Zm2 = complex(d['max']['Rre'] , d['max']['Rim'])
		A = (Z2-Z1)/(Zm2-Zm1)
		B = (Z1*Zm2-Z2*Zm1)/(Zm2-Zm1)
		Zxm = complex(Rre , Rim)
		Zx = A*Zxm+B
		return Zx

	def correct0(self, Rre, Rim, period, F):
		d = self.data[period]
		Zsm = complex(d['short']['Rre'] , d['short']['Rim'])
		Zstdm = complex(d['load']['Rre'] , d['load']['Rim'])
		Zstd = complex(self.R, 0)
		Zxm = complex(Rre , Rim)
		Zx = Zstd/(Zstdm-Zsm)*(Zxm-Zsm)
		if period==720000:
			print "Zsm=", Zsm
			print "Zstdm=", Zstdm
			print "Zstd=", Zstd
			print "Zxm=", Zxm
			print "Zx=", Zx
		return Zx

	def correct3(self, Rre, Rim, period, F):
		d = self.data[period]
		Zom = complex(d['open']['Rre'] , d['open']['Rim'])
		Zstdm = complex(d['load']['Rre'] , d['load']['Rim'])
		Ystd = complex(1.0/self.R, 2*math.pi*F*self.C)
		Zstd = 1/Ystd
		#Zstd = complex(self.R, 0)
		Zxm = complex(Rre , Rim)
		Zx = Zstd*(1/Zstdm-1/Zom)*Zxm/(1-Zxm/Zom)
		if period==96:
			print "Zsm=", Zsm
			print "Zstdm=", Zstdm
			print "Zstd=", Zstd
			print "Zxm=", Zxm
			print "Zx=", Zx
		return Zx

	# Калибровка по трем элементам 
	# Z2 == Zdut
	# Z1 == ZDut measured
	# Z1 = (A*Z2+B)/(C*Z2+D)
	# A = 1
	def calcLine(self, Zm, Zdut):
		Z1 = Zdut
		Z2 = Zm
		#        Bre, Bim,           Cre,                           Cim,                       Dre,      Dim
		return ([ -1,  0, Z1.real*Z2.real-Z1.imag*Z2.imag, -Z1.imag*Z2.real-Z1.real*Z2.imag, Z1.real, -Z1.imag],
		        [  0, -1, Z1.real*Z2.imag+Z1.imag*Z2.real, -Z1.imag*Z2.imag+Z1.real*Z2.real, Z1.imag,  Z1.real],
		        Z2.real,
		        Z2.imag)

	def calcABCD(self, Z0dut, Z0m, Z1dut, Z1m, Z2dut, Z2m):
		# M * (A,B,C) = H
		# M - matrix
		# H - complex vector
		# A - complex number

		(M0, M1, H0, H1) = self.calcLine(Z0m, Z0dut)
		(M2, M3, H2, H3) = self.calcLine(Z1m, Z1dut)
		(M4, M5, H4, H5) = self.calcLine(Z2m, Z2dut)

		#solve
		a = np.array([M0, M1, M2, M3, M4, M5])
		b = np.array([H0, H1, H2, H3, H4, H5])
		x = np.linalg.solve(a, b)
		A = complex(1,0)
		B = complex(x[0], x[1])
		C = complex(x[2], x[3])
		D = complex(x[4], x[5])

		#print np.allclose(np.dot(a, x), b)
		return (A,B,C,D)

	def correct1(self, Rre, Rim, period, F):
		(A, B, C, D) = self.data[period]
		Zxm = complex(Rre , Rim)
		Zx = (A*Zxm+B)/(C*Zxm+D)
		if period==72000:
			print "Zxm=", Zxm
			print "Zx=", Zx
		return Zx

	def correctParallelCapacitor(self, Rre, Rim, period, F):
		C = 1.59e-12
		#C = 0e-12
		#C = -6e-12
		L = 100e-9
		Rshort = 0.007
		Yc = complex(0, 2*math.pi*F*C)
		Zxm = complex(Rre , Rim)
		Zs = complex(Rshort, 2*math.pi*F*L)
		Zx = (Zxm-Zs)/(1-(Zxm-Zs)*Yc)
		return Zx
'''

def calculate(fileName):
	jout = readJson(fileName)
	res = calculateJson(jout)

	F = res['F']
	Rre = res['Rre']
	Rim = res['Rim']
	print "F=", F
	print "dfi=", res['dfi']
	print "ampV=", res['ampV'], "V"
	print "ampI=", res['current'], "A"
	print "resistance=", res['resistance'], "Om"
	print "Rre=", Rre, "Om"
	print "Rim=", Rim, "Om"

	print "cRe=", res['cRe']
	print "cIm=", res['cIm']

	if Rim<0:
		# capacitor
		Rim = -Rim
		C = 1/(2*math.pi*F*Rim)
		print "ESR=", Rre, " Om"
		printC(C)
		pass
	else:
		# inductance
		L = Rim/(2*math.pi*F)
		print "R=", Rre, " Om"
		#print "L=", L*1e6-0.137, " mkH"
		print "L=", L*1e6, " mkH"
		pass

	return 'data' in jout

def plotIVInternal(ax, fileName, average = False):
	jout = readJson(fileName)
	jattr = jout["attr"]
	ncycle = jattr['ncycle']

	ax.set_xlabel("Voltage")
	ax.set_ylabel("Current")
	#ax.set_title(data['title'])
	xdata = jout['data']['V']
	ydata = jout['data']['I']

	if average:
		xdata = averagePeriod(xdata, ncycle)
		ydata = averagePeriod(ydata, ncycle)
		xdata.append(xdata[0])
		ydata.append(ydata[0])

	ax.plot (xdata, ydata, '-')
	#ax.plot (xdata, ydata, '.')
	pass

def plotIV(fileName, average = False):
	is_data = calculate(fileName)
	if not is_data:
		return
	fig, ax = plt.subplots()
	plotIVInternal(ax, fileName, average)
	plt.show()
	pass

def plotIV_2():
	average = True
	fig, ax = plt.subplots()
	plotIVInternal(ax, "0pF_100KHz.json", average)
	plotIVInternal(ax, "1_5pF_100KHz.json", average)
	plt.show()
	pass

def plotFreq(fileName):
	jout = readJson(fileName)
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

	corr = Corrector()

	for jf in jfreq:
		res = calculateJson(jf)
		F = res['F']
		f_data.append(F)

		re_data.append(math.fabs(res['Rre']))
		#im_data.append(math.sqrt(res['Rre']**2+res['Rim']**2))
		#re_data.append(res['Rre'])
		im_data.append(math.fabs(res['Rim']))
		#im_data.append(res['Rim'])
		re_error.append(jf['summary']['V']['square_error'])
		im_error.append(jf['summary']['I']['square_error'])

		gain_I = jf['attr']["gain_I"]
		gain_V = jf['attr']["gain_V"]
		#re_error.append(math.sqrt(jf['summary']['V']['sin']**2+jf['summary']['V']['cos']**2)/gain_V)
		#im_error.append(math.sqrt(jf['summary']['I']['sin']**2+jf['summary']['I']['cos']**2)/gain_I)

		im_sin.append(jf['summary']['I']['sin']/gain_I)
		im_cos.append(jf['summary']['I']['cos']/gain_I)

		#dfi_data.append(res['dfi']*1e6/F)
		dfi_data.append(res['dfi'])

		if True:
			Zx = complex(res['Rre'], res['Rim'])
			#Zx = corr.correct(res['Rre'], res['Rim'], res['period'], F)
			re_corr.append(Zx.real)
			im_corr.append(math.fabs(Zx.imag))
			
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
		if False:
			#Zx = complex(res['Rre'], res['Rim'])
			Zx = corr.correct(res['Rre'], res['Rim'], res['period'], F)
			Yx = 1/Zx

			if Yx.real < 1e-8:
				re_corr.append(1e8)
			else:
				re_corr.append(1/Yx.real)

			im_max = 1e8
			if math.fabs(Yx.imag)*im_max>1:
				im_corr.append(1/Yx.imag)
			else:
				if Yx.imag>0:
					im_corr.append(im_max)
				else:
					im_corr.append(-im_max)

			C = Yx.imag/(2*math.pi*F)
			C = min(C, 1e-6)
			C = max(C, -1e-6)
			arr_C.append(C*1e12)

			if Yx.imag<0:
				L = -1/(2*math.pi*F*Yx.imag)
			else:
				L = 0
			arr_L.append(L*1e6)



	fig, ax = plt.subplots()
	#ax.set_title("1 uF 160 V")
	ax.set_xscale('log')
	#ax.set_yscale('log')
	ax.set_xlabel("Hz")

	ax.set_ylabel("Om")
	#ax.plot (f_data, re_data, '-', color="red")
	#ax.plot (f_data, im_data, '-', color="blue")
	#ax.plot (f_data, dfi_data, '-', color="green")

	#ax.plot (f_data, re_error, '.', color="red")
	#ax.plot (f_data, im_error, '.-', color="blue")

	#ax.plot (f_data, im_sin, '.', color="red")
	#ax.plot (f_data, im_cos, '.-', color="blue")

	#ax.plot (f_data, re_corr, '.-', color="#00FF00")
	#ax.plot (f_data, im_corr, '.-', color="#555555")

	ax.set_ylabel("uH")
	ax.plot (f_data, arr_L, '-', color="red")

	#ax.set_ylabel("pF")
	#ax.plot (f_data, arr_C, '-', color="red")

	plt.show()
	pass

def plotDelta(fileName1, fileName2, K):
	jfreq1 = readJson(fileName1)['freq']
	jfreq2 = readJson(fileName2)['freq']
	f_data = []
	ampV = []
	ampI = []
	dfi_data = []
	for i in xrange(0, len(jfreq1)):
		jf1 = jfreq1[i]
		jf2 = jfreq2[i]		
		res1 = calculateJson(jf1)
		res2 = calculateJson(jf2)
		F = res1['F']
		f_data.append(F)
		ampV.append(res2['ampV']*K/res1['ampV']-1.0)
		ampI.append(res2['ampI']*K/res1['ampI']-1.0)
		dfi_data.append(res2['dfi']-res1['dfi'])
		pass

	fig, ax = plt.subplots()
	ax.set_xscale('log')
	ax.set_xlabel("Hz")
	ax.set_ylabel("V")

	ax.plot (f_data, ampV, '-', color="red")
	ax.plot (f_data, ampI, '-', color="blue")
	ax.plot (f_data, dfi_data, '-', color="green")

	plt.show()
	pass

def main():
	if len(sys.argv)>=2:
		fileName = sys.argv[1]

	#plotDelta("0_1200.json", "4_150.json", 8)
	#plotDelta("0_1200.json", "1_600.json", 2)
	#return

	if fileName[0]=='f':
		plotFreq(fileName)
	else:
		#plot(fileName)
		#plotRaw(fileName, "V", average=False)
		plotIV(fileName, average=True)
		#plotIV_2()

if __name__ == "__main__":
	main()