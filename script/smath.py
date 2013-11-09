import math
import array

fileName = "out.dat"
def readFileAsShort(filename):
	with open(filename, "rb") as file:
		data = file.read()
		arr = array.array('H')
		arr.fromstring(data)
	return arr

def calcSinCos(period, clock, adc_cycles, data):
	'''
		return (c0,csin, ccos)
		c0+csin*sin(f)+ccos*cos(f)
	'''
	arr = array.array('f', data)
	ncycle = period/adc_cycles
	print ncycle
	fsin = [ math.sin(2*math.pi*i/ncycle) for i in xrange(ncycle)]
	fcos = [ math.cos(2*math.pi*i/ncycle) for i in xrange(ncycle)]
	
	N = len(data)
	N = ncycle
	sum_c0 = 0
	for i in xrange(N):
		sum_c0 += arr[i]
	c0 = sum_c0/N

	for i in xrange(N):
		arr[i] -= c0

	sum_sin = 0
	sum_cos = 0
	for i in xrange(N):
		ic = i%ncycle
		sum_sin += arr[i]*fsin[ic]
		sum_cos += arr[i]*fcos[ic]

	csin = sum_sin*2/N
	ccos = sum_cos*2/N

	print "c0=", c0
	print "csin=", csin
	print "ccos=", ccos
	#print arr
	return (c0, csin, ccos)

def calcFi(csin, ccos):
	'''
		input 
		csin*sin(f)+ccos*cos(f) == amplitude*sin(f+fi)
		return (amplitude, fi)
	'''
	amplitude = math.sqrt(csin*csin+ccos*ccos)
	print "amplitude=", amplitude
	csin /= amplitude
	ccos /= amplitude

	#print "sin=", math.acos(csin)
	#print "cos=", math.asin(ccos)
	fi = math.atan2(ccos, csin)
	print "fi=", fi
	return (amplitude, fi)

def deltaError(data, c0, amplitude, fi, ncycle):
	N = len(data)
	out = [ c0+amplitude*math.sin(2*math.pi*i/ncycle+fi)-data[i] for i in xrange(N)]
	return out

def main():
	data = readFileAsShort(fileName)
	#period = 7200 # F=10000
	period = 3600 # F=20000
	clock = 72000000
	adc_cycles = 120
	ncycle = period/adc_cycles
	#data = [ math.sin(2*math.pi*i/ncycle+3.5) for i in xrange(ncycle)]
	(c0, csin, ccos) = calcSinCos(period, clock, adc_cycles, data)

	(amplitude, fi) = calcFi(csin, ccos)

	err = deltaError(data, c0, amplitude, fi, ncycle)
	print err
	with open("out_error.dat", "wb") as file:
		arr = array.array('H')
		for i in xrange(len(err)):
			arr.append(int(math.floor(err[i]+0.5+2500)))
		arr.tofile(file)
	pass

if __name__ == "__main__":
	main()