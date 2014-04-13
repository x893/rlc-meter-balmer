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

def formatC(C):
	CA = math.fabs(C)
	if CA<1e-11:
		return '{:3.2f} pF'.format(C*1e12)
	if CA<1e-9:
		return '{:3.1f} pF'.format(C*1e12)
	if CA<1e-8:
		return '{:3.2f} nF'.format(C*1e9)
	if CA<1e-6:
		return '{:3.1f} nF'.format(C*1e9)
	if CA<1e-5:
		return '{:3.2f} mkF'.format(C*1e6)
	if CA<1e-3:
		return '{:3.1f} mkF'.format(C*1e6)
	return '{:3.0f} mkF'.format(C*1e6)

def formatL(L):
	LA = math.fabs(L)
	if LA<1e-8:
		return '{:3.2f} nH'.format(L*1e9)
	if LA<1e-6:
		return '{:3.1f} nH'.format(L*1e9)
	if LA<1e-5:
		return '{:3.2f} mkH'.format(L*1e6)
	if LA<1e-3:
		return '{:3.1f} mkH'.format(L*1e6)
	if LA<1e-2:
		return '{:3.2f} mH'.format(L*1e3)
	if LA<1:
		return '{:3.1f} mH'.format(L*1e3)
	if LA<1e1:
		return '{:3.2f} H'.format(L)
	return '{:3.1f} H'.format(L)

def printC(C):
	print "C=", formatC(C)

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
	return complex(sdata["sin"], sdata["cos"])

def calculateJson(jout, gain_corrector = None):
	jattr = jout["attr"]
	period = jattr["period"]
	clock = jattr["clock"]
	ncycle = jattr["ncycle"]

	resistor = jattr["resistor"]

	F = clock/float(period) #frequency, herz

	if gain_corrector:
		(ampV, fiV, ampI, fiI)=gain_corrector.calcPolar(period=period, clock=clock, ncycle=ncycle, jout=jout)
		(zV, zI)=gain_corrector.calcComplex(period=period, clock=clock, ncycle=ncycle, jout=jout)
	else:
		gain_V = jattr["gain_V"]
		gain_I = jattr["gain_I"]
		if 'summary' in jout:
			zV = calcFast(period=period, clock=clock, ncycle=ncycle, sdata=jout['summary']['V'])
			zI = calcFast(period=period, clock=clock, ncycle=ncycle, sdata=jout['summary']['I'])
			zV *= toVolts/gain_V
			zI *= toVolts/gain_I
			(ampV, fiV) = cmath.polar(zV)
			(ampI, fiI) = cmath.polar(zI)
		else:
			resultV = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=jout['data']['V'])
			resultI = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=jout['data']['I'])
			zV = cmath.rect(resultV['amplitude'], resultV['fi'])
			zI = cmath.rect(resultI['amplitude'], resultI['fi'])
			zV *= toVolts/gain_V
			zI *= toVolts/gain_I

	R = (zV/zI)*resistor

	return {
		"R": R,
		"F": F,
		"period": period,
		"fiV": cmath.polar(zV),
		"fiI": cmath.polar(zI)
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
			#data[period] = jout
			data[period] = {'I': self.calcZ(jout['summary']['I'])*toVolts/jout['attr']['gain_I'],
							'V': self.calcZ(jout['summary']['V'])*toVolts/jout['attr']['gain_V']}
		jsons.append(data)
		pass

	def calcComplex(self, period, clock, ncycle, jout):
		zV = self.calcX('V', self.jsonsV, jout, index=jout['attr']['gain_index_V'], period=period)
		zI = self.calcX('I', self.jsonsI, jout, index=jout['attr']['gain_index_I'], period=period)
		return (zV, zI)

	def calcPolar(self, period, clock, ncycle, jout):
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
		z0 = jout0[IV]
		zX = joutX[IV]

		zMeasure *= toVolts/jout['attr'][gi]
		#if index!=0:
		#	print "i"+IV+'=', index, "z0=", z0, "zX=", zX
		return zMeasure*z0/zX

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

	def correct(self, R, period, F):
		d = self.data[period]
		Z1 = complex(self.Rmin, 0)
		Z2 = complex(self.Rmax, 0)
		Zm1 = d['min']['R']
		Zm2 = d['max']['R']
		A = (Z2-Z1)/(Zm2-Zm1)
		B = (Z1*Zm2-Z2*Zm1)/(Zm2-Zm1)
		Zxm = R
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
			res = calculateJson(jf, self.gain_corrector)
			data[res['period']] = { 'open': res }

		jfreq_load = json_load['freq']
		for jf in jfreq_load:
			res = calculateJson(jf, self.gain_corrector)
			data[res['period']]['load'] = res

		self.data = data
		self.R = json_load['R']
		self.C = 1.2e-12
		pass

	def correct(self, R, period, F):
		d = self.data[period]
		Zom = d['open']['R']
		Zstdm = d['load']['R']
		Ystd = complex(1.0/self.R, 2*math.pi*F*self.C)
		Zstd = 1/Ystd
		#Zstd = complex(self.R, 0)
		Zxm = R
		Zx = Zstd*(1/Zstdm-1/Zom)*Zxm/(1-Zxm/Zom)
		if period==96:
			print "Zsm=", Zsm
			print "Zstdm=", Zstdm
			print "Zstd=", Zstd
			print "Zxm=", Zxm
			print "Zx=", Zx
		return Zx

class CorrectorShort:
	def __init__(self, gain_corrector, load_filename):
		self.gain_corrector = gain_corrector
		self.load(load_filename)
		pass
	def load(self, load_filename):
		json_short = readJson("cor/K_short.json")
		#json_load = readJson("cor/D0_100Om.json")
		#json_load = readJson("cor/D0_1Om.json")
		json_load = readJson(load_filename)

		data = {}

		jfreq_short = json_short['freq']
		for jf in jfreq_short:
			res = calculateJson(jf, self.gain_corrector)
			data[res['period']] = { 'short': res }

		jfreq_load = json_load['freq']
		for jf in jfreq_load:
			res = calculateJson(jf, self.gain_corrector)
			data[res['period']]['load'] = res

		self.data = data
		self.R = json_load['R']
		pass

	def correct(self, R, period, F):
		d = self.data[period]
		Zsm = d['short']['R']
		Zstdm = d['load']['R']
		Zstd = complex(self.R, 0)
		Zxm = R
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
		self.corr_short1Om = CorrectorShort(gain_corrector=None, load_filename="cor/D0_1Om.json")
		self.corr_short = CorrectorShort(gain_corrector=self.gain_corrector, load_filename="cor/D0_100Om.json")
		self.corr = []
		self.corr.append(Corrector2x(0, self.gain_corrector))
		self.corr.append(Corrector2x(1, self.gain_corrector))
		self.corr.append(Corrector2x(2, self.gain_corrector))
		self.corr.append(CorrectorOpen(self.gain_corrector))
		pass
	def correct(self, R, period, F, resistor_index):
		if abs(R)<100:
			return self.corr_short.correct(R, period, F)
		return self.corr[resistor_index].correct(R, period, F)

	def calculateJson(self, jf):
		if jf['attr']['gain_index_V']==7:
			#сопротивление меньше 1 Ом
			#не используем gain_corrector
			#по хорошему надо бы использовать его для тока, но не будем, вроде на всем диапазоне gain_index_I=0
			res = calculateJson(jf, gain_corrector=None)
			Zx = self.corr_short1Om.correct(res['R'], res['period'], res['F'])
			res['Zx'] = Zx
			return res

		res = calculateJson(jf, gain_corrector=self.gain_corrector)
		Zx = self.correct(res['R'], res['period'], res['F'], jf['attr']['resistor_index'])
		res['Zx'] = Zx
		return res

def calculateLC(res, serial=True):
	F = res['F']
	Zx = res['Zx']
	Cmax = 1e-2

	if serial:
		isC = False
		L = Zx.imag/(2*math.pi*F)

		if Zx.imag<-1e-10:
			isC = True
			C = -1/(2*math.pi*F*Zx.imag)
		else:
			C = 0
		#если сопротивление маленькое и индуктивность немного отрицательная, то таки считаем что это ошибка калибрации
		if abs(Zx)<1 and L<0 and L>-20e-9:
			isC = False

	if not serial: #parrallel
		isC = True
		Yx = 1/Zx
		C = Yx.imag/(2*math.pi*F)
		C = min(C, Cmax)
		C = max(C, -Cmax)

		if Yx.imag<-1e-10:
			isC = False
			L = -1/(2*math.pi*F*Yx.imag)
		else:
			L = 0

		#если сопротивление большое и емкость немного отрицательная, то таки считаем что это ошибка калибрации
		if abs(Zx)>1e5 and C<0 and C>-5e-12:
			isC = True


	return (L, C, isC)


def calculate(fileName):
	jout = readJson(fileName)
	res = calculateJson(jout)

	F = res['F']
	Rre = res['R'].real
	Rim = res['R'].imag
	print "F=", F
	print "dfi=", cmath.phase(res['R'])
	print "resistance=", abs(res['R']), "Om"
	print "Rre=", Rre, "Om"
	print "Rim=", Rim, "Om"

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

def main():
	if len(sys.argv)>=2:
		fileName = sys.argv[1]

	#plot(fileName)
	#plotRaw(fileName, "V", average=False)
	plotIV(fileName, average=True)
	#plotIV_2()

if __name__ == "__main__":
	main()