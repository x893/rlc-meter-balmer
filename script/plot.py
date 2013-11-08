# -*- coding: UTF-8 -*-

import array
import math
import datetime
import struct
import sys
# !!! Импортируем один из пакетов Matplotlib
import pylab
import matplotlib

fileName = "out.dat"
title = ""


def readFileAsShort(filename):
	with open(filename, "rb") as file:
		data = file.read()
		arr = array.array('H')
		arr.fromstring(data)
	return arr

def makeTimeList(readableData):
	xmin = 0
	#xstep = 1.125 # us
	xstep = 26/12.0 # us
	xlist = []
	for i in xrange(0, len(readableData)):
		xlist.append(xmin+i*xstep)
	return xlist

def plot0():
	out = readFileAsShort(fileName)
	print "data=",out

	# Вычислим значение функции в заданных точках
	# x us
	xlist = makeTimeList(out)

	# !!! Нарисуем одномерный график
	pylab.plot (xlist, out)

	# !!! Покажем окно с нарисованным графиком
	pylab.show()


if len(sys.argv)>=2:
	fileName = sys.argv[1]
if len(sys.argv)>=3:
	title = sys.argv[2]
plot0()
