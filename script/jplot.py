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

def getGainCentralIdx():
    return [0,1,2]

def getGainOpenShortIdx():
    return [0,1,2,4,6,7]

def formatR(R):
	RA = math.fabs(R)
	if RA<1e-2:
		return '{:3.2f} mOm'.format(R*1e3)
	if RA<1:
		return '{:3.1f} mOm'.format(R*1e3)
	if RA<1e1:
		return '{:3.2f} Om'.format(R)
	if RA<1e3:
		return '{:3.1f} Om'.format(R)
	if RA<8e3:
		return '{:3.3f} KOm'.format(R*1e-3)
	if RA<2e4:
		return '{:3.2f} KOm'.format(R*1e-3)
	if RA<1e6:
		return '{:3.1f} KOm'.format(R*1e-3)
	if RA<1e7:
		return '{:3.2f} MOm'.format(R*1e-6)
	return '{:3.1f} MOm'.format(R*1e-6)

def formatC(C):
	CA = math.fabs(C)
	if CA<1e-11:
		return '{:3.3f} pF'.format(C*1e12)
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

def calculateJson(jout):
	jattr = jout["attr"]
	period = jattr["period"]
	clock = jattr["clock"]
	ncycle = jattr["ncycle"]

	resistor = jattr["resistor"]

	F = clock/float(period) #frequency, herz

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


class Corrector2x:
	def __init__(self, diapazon):
		self.load(diapazon)
		pass
	def load(self, diapazon):
		if diapazon==0:
			name0 = '100Om'
			name1 = '1KOm'
		elif diapazon==1:
			name0 = '1KOm'
			name1 = '10KOm'
		elif diapazon==2:
			name0 = '10KOm'
			name1 = '100KOm'

		data = {}
		for i in getGainCentralIdx():
			prefix = 'cor/R'+str(diapazon)+'V0I'+str(i)+'_'
			fname0 = prefix+name0+'.json'
			fname1 = prefix+name1+'.json'
			json_min = readJson(fname0)
			json_max = readJson(fname1)

			self.Rmin = json_min['R']
			self.Rmax = json_max['R']

			cur = {}
			data[i] = cur
			jfreq_min = json_min['freq']
			for jf in jfreq_min:
				res = calculateJson(jf)
				cur[res['period']] = { 'min': res }

			jfreq_max = json_max['freq']
			for jf in jfreq_max:
				res = calculateJson(jf)
				cur[res['period']]['max'] = res

		self.data = data
		self.C = 1.2e-12
		pass

	def correct(self, R, period, F, attr):
		gain_index_I = attr['gain_index_I']
		d = self.data[gain_index_I][period]
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
	def __init__(self):
		self.load()
		pass
	def load(self):
		data = {}
		for i in getGainOpenShortIdx():
			prefix = 'cor/R3V0I'+str(i)+'_'
			fname0 = prefix+'100KOm.json'
			fname1 = prefix+'open.json'
			json_min = readJson(fname0)
			json_max = readJson(fname1)

			self.R = json_min['R']

			cur = {}
			data[i] = cur
			jfreq_min = json_min['freq']
			for jf in jfreq_min:
				res = calculateJson(jf)
				cur[res['period']] = { 'load': res }

			jfreq_max = json_max['freq']
			for jf in jfreq_max:
				res = calculateJson(jf)
				cur[res['period']]['open'] = res

		self.data = data
		#self.C = 0
		self.C = 0.5e-12
		pass

	def correct(self, R, period, F, attr):
		gain_index_I = attr['gain_index_I']
		d = self.data[gain_index_I][period]
		Zom = d['open']['R']
		Zstdm = d['load']['R']
		Ystd = complex(1.0/self.R, 2*math.pi*F*self.C)
		Zstd = 1/Ystd
		#Zstd = complex(self.R, 0)
		Zxm = R
		#Zx = Zstd*(1/Zstdm-1/Zom)*Zxm/(1-Zxm/Zom)
		Zx = Zstd*(1/Zstdm-1/Zom)/(1/Zxm-1/Zom)
		return Zx

class CorrectorShort:
	def __init__(self):
		self.load()
		pass
	def load(self):
		data = {}
		for i in getGainOpenShortIdx():
			prefix = 'cor/R0V'+str(i)+'I0_'
			fname0 = prefix+'short.json'
			if i==7:
				fname1 = prefix+'1Om.json'
			else:
				fname1 = prefix+'100Om.json'
			json_min = readJson(fname0)
			json_max = readJson(fname1)

			self.R100 = json_max['R']

			cur = {}
			data[i] = cur
			jfreq_min = json_min['freq']
			for jf in jfreq_min:
				res = calculateJson(jf)
				cur[res['period']] = { 'short': res }

			jfreq_max = json_max['freq']
			for jf in jfreq_max:
				res = calculateJson(jf)
				if res['period'] in cur:
					cur[res['period']]['load'] = res

		self.data = data
		pass

	def correct(self, R, period, F, attr):
		gain_index_V = attr['gain_index_V']
		d = self.data[gain_index_V][period]
		Zsm = d['short']['R']
		Zstdm = d['load']['R']
		Zstd = complex(self.R100, 0)
		Zxm = R
		Zx = Zstd/(Zstdm-Zsm)*(Zxm-Zsm)
		return Zx

class Corrector:
	def __init__(self):
		self.load()
		pass
	def load(self):
		self.corr_short = CorrectorShort()
		self.corr = []
		self.corr.append(Corrector2x(0))
		self.corr.append(Corrector2x(1))
		self.corr.append(Corrector2x(2))
		self.corr.append(CorrectorOpen())
		pass
	def correct(self, R, period, F, attr):
		resistor_index = attr['resistor_index']
		if abs(R)<100:
			return self.corr_short.correct(R, period, F, attr)
		#return self.corr[resistor_index].correct(R, period, F, attr)
		#if resistor_index==3: #заглушка
		#	return R

		return self.corr[resistor_index].correct(R, period, F, attr)

	def calculateJson(self, jf):
		res = calculateJson(jf)
		Zx = self.correct(res['R'], res['period'], res['F'], jf['attr'])
		res['Zx'] = Zx
		return res

class MaxAmplitude:
	'''
	Для резистора 100 КОм для оpen щупов ограничиваем амплитуду сигнала.
	'''
	def __init__(self):
		json_open = readJson("cor/R3AUTO_open.json")
		data = {}

		jfreq_open = json_open['freq']
		for jf in jfreq_open:
			attr = jf['attr']			
			data[attr['period']] = { 'gain_index_I': attr['gain_index_I'] }

		self.data = data
		pass
	def getMaxGainI(self, resistorIndex, period):
		if resistorIndex!=3:
			return 7
		d = self.data[period]
		return d['gain_index_I']

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
	#plotRaw(fileName, "I", average=False)
	plotIV(fileName, average=True)
	#plotIV_2()

if __name__ == "__main__":
	main()