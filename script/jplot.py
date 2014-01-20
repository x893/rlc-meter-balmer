# -*- coding: UTF-8 -*-
import array
import math
import datetime
import struct
import sys
import json
import smath

import matplotlib
import matplotlib.pyplot as plt


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
	return {"amplitude": amplitude, "fi": fi}

def calculateJson(jout):
	jattr = jout["attr"]
	period = jattr["period"]
	clock = jattr["clock"]
	ncycle = jattr["ncycle"]

	gain_V = jattr["gain_V"]
	gain_I = jattr["gain_I"]
	resistor = jattr["resistor"]
	toVolts = 3.3/4095.0

	F = clock/period #frequency, herz

	if 'summary' in jout:
		resultV = calcFast(period=period, clock=clock, ncycle=ncycle, sdata=jout['summary']['V'])
		resultI = calcFast(period=period, clock=clock, ncycle=ncycle, sdata=jout['summary']['I'])
	else:
		resultV = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=jout['data']['V'])
		resultI = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=jout['data']['I'])
	ampV = resultV['amplitude']
	ampI = resultI['amplitude']
	dfi = resultV['fi']-resultI['fi']
	if dfi>math.pi:
		dfi -= math.pi*2
	if dfi<-math.pi:
		dfi += math.pi*2

	ampV *= toVolts/gain_V
	ampI *= toVolts/gain_I

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
		"period": period
	}

def printC(C):
	if C>=1:
		print "C=", C, "F"
	elif C>=1e-6:
		print "C=", C*1e6, "mkF"
	elif C>=1e-9:
		print "C=", C*1e9, "nF"
	else:
		print "C=", C*1e12, "pF"

class Corrector:	
	def __init__(self):
		self.load()
		pass

	def load(self):		
		self.load0()
		#self.load3()
		pass

	def correct(self, Rre, Rim, period):
		return self.correct0(Rre, Rim, period)
		#return self.correct3(Rre, Rim, period)

	def load0(self):
		json_short = readJson("cor/0_short.json")
		json_load = readJson("cor/0_load_1.json")

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
		json_open = readJson("cor/3_open.json")
		json_load = readJson("cor/3_load.json")

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

		pass

	def correct0(self, Rre, Rim, period):
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

	def correct3(self, Rre, Rim, period):
		d = self.data[period]
		Zom = complex(d['open']['Rre'] , d['open']['Rim'])
		Zstdm = complex(d['load']['Rre'] , d['load']['Rim'])
		Zstd = complex(self.R, 0)
		Zxm = complex(Rre , Rim)
		Zx = Zstd*(complex(1)/Zstdm-complex(1)/Zom)*Zxm/(1-Zxm/Zom)
		if period==96:
			print "Zsm=", Zsm
			print "Zstdm=", Zstdm
			print "Zstd=", Zstd
			print "Zxm=", Zxm
			print "Zx=", Zx
		return Zx



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
	re_error = []
	im_error = []
	re_corr = []
	im_corr = []
	arr_L = []
	arr_C = []

	corr = Corrector()

	for jf in jfreq:
		res = calculateJson(jf)
		f_data.append(res['F'])

		re_data.append(math.fabs(res['Rre']))
		#re_data.append(res['Rre'])
		im_data.append(math.fabs(res['Rim']))
		#im_data.append(res['Rim'])
		re_error.append(jf['summary']['V']['square_error'])
		im_error.append(jf['summary']['I']['square_error'])

		Zx = corr.correct(res['Rre'], res['Rim'], res['period'])
		re_corr.append(Zx.real)
		im_corr.append(math.fabs(Zx.imag))
		
		F = res['F']
		if Zx.imag>0:
			L = Zx.imag/(2*math.pi*F)
		else:
			L = 0

		if Zx.imag<-1e-10:
			C = -1/(2*math.pi*F*Zx.imag)
		else:
			C = 0
		arr_L.append(L*1e6)
		arr_C.append(C*1e6)


	fig, ax = plt.subplots()
	ax.set_title("4700 uF")
	ax.set_xscale('log')
	ax.set_yscale('log')
	ax.set_xlabel("Hz")

	#ax.set_ylabel("Om")
	#ax.plot (f_data, re_data, '-', color="red")
	#ax.plot (f_data, im_data, '-', color="blue")

	#ax.plot (f_data, re_error, '.', color="red")
	#ax.plot (f_data, im_error, '.', color="blue")

	#ax.plot (f_data, re_corr, '.-', color="red")
	#ax.plot (f_data, im_corr, '.-', color="blue")

	#ax.set_ylabel("uH")
	#ax.plot (f_data, arr_L, '-', color="red")

	#ax.set_ylabel("uF")
	ax.plot (f_data, arr_C, '-', color="red")

	plt.show()
	pass


def main():
	if len(sys.argv)>=2:
		fileName = sys.argv[1]

	if True or fileName[0]=='f':
		plotFreq(fileName)
	else:
		#plot(fileName)
		#plotRaw(fileName, "V", average=False)
		plotIV(fileName, average=True)
		#plotIV_2()

if __name__ == "__main__":
	main()