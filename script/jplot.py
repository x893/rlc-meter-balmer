# -*- coding: UTF-8 -*-
import array
import math
import datetime
import struct
import sys
import json

import matplotlib
import matplotlib.pyplot as plt


def readJson(filename):
	with open(filename, "rb") as file:
		data = json.load(file)
	return data

def makeTimeList(readableData):
	xmin = 0
	xstep = 1 # us
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


if len(sys.argv)>=2:
	fileName = sys.argv[1]

plot(fileName)
