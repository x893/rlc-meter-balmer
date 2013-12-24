# -*- coding: UTF-8 -*-

import array
import math
import datetime
import struct
import sys
# !!! Импортируем один из пакетов Matplotlib
import pylab
import matplotlib

title = ""

def readFileAsShort(filename):
	with open(filename, "rb") as file:
		data = file.read()
		arr = array.array('H')
		arr.fromstring(data)
	return arr

def makeTimeList(readableData):
	xmin = 0
	xstep = 1 # us
	xlist = []
	for i in xrange(0, len(readableData)):
		xlist.append(xmin+i*xstep)
	return xlist

def plotRaw(fileName):
	out = readFileAsShort(fileName)
	#out = out[0:100]
	print "data_len=", len(out)

	# Вычислим значение функции в заданных точках
	# x us
	xlist = makeTimeList(out)

	# !!! Нарисуем одномерный график
	#pylab.plot (xlist, out, 'r-')
	pylab.plot (xlist, out, '.')

	# !!! Покажем окно с нарисованным графиком
	pylab.show()
	pass

def plotCircle(filePrefix):
	out1 = readFileAsShort(filePrefix+'1.dat')
	out2 = readFileAsShort(filePrefix+'2.dat')

	pylab.plot (out1, out2, '.')
	pylab.show()
	pass

def help():
	print "plot.py g out1.dat - plot raw graph"
	print "plot.py c out - plot circle graph"
	pass

def main():
	if len(sys.argv)<3:
		help()
		return;
	if sys.argv[1]=='g':
		plotRaw(sys.argv[2])
	if sys.argv[1]=='c':
		plotCircle(sys.argv[2])
	pass

main()