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


def calculate(fileName):
	jout = readJson(fileName)
	jattr = jout["attr"]
	period = jattr["period"]
	clock = jattr["clock"]
	ncycle = jattr["ncycle"]

	gain_V = jattr["gain_V"]
	gain_I = jattr["gain_I"]
	resistor = jattr["resistor"]
	toVolts = 3.3/4095.0

	F = clock/period #frequency, herz

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

	print "F=", F
	print "dfi=", dfi
	print "ampV=", ampV, "V"
	print "ampI=", current, "A"
	print "resistance=", resistanceComplex, "Om"

	print "cRe=", cRe
	print "cIm=", cIm

	if cIm<0:
		# capacitor
		Rre = resistanceComplex*cRe
		Rim = -resistanceComplex*cIm
		C = 1/(2*math.pi*F*Rim)
		print "ESR=", Rre, " Om"
		print "C=", C*1e6, " mkF"
		pass

	if cIm>0:
		# inductance
		Rre = resistanceComplex*cRe
		Rim = resistanceComplex*cIm
		L = Rim/(2*math.pi*F)
		print "R=", Rre, " Om"
		#print "L=", L*1e6-0.137, " mkH"
		print "L=", L*1e6, " mkH"
		pass

	pass

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

	#ax.plot (xdata, ydata, '-')
	ax.plot (xdata, ydata, '.')
	pass

def plotIV(fileName, average = False):
	calculate(fileName)
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

if len(sys.argv)>=2:
	fileName = sys.argv[1]

#plot(fileName)
#plotRaw(fileName, "V", average=False)
plotIV(fileName, average=False)
#plotIV_2()
