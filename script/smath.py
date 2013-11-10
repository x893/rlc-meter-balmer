import math
import array

fileName = "out.dat"
def readFileAsShort(filename):
	with open(filename, "rb") as file:
		data = file.read()
		arr = array.array('H')
		arr.fromstring(data)
	return arr

def calcSinCos(period, clock, adc_tick, data):
	'''
		return (c0,csin, ccos)
		c0+csin*sin(f)+ccos*cos(f)
	'''
	arr = array.array('f', data)
	ncycle = period/adc_tick
	print "ncycle=", ncycle
	fsin = [ math.sin(2*math.pi*i/ncycle) for i in xrange(ncycle)]
	fcos = [ math.cos(2*math.pi*i/ncycle) for i in xrange(ncycle)]
	
	N = len(data)

	print "N =", len(data), len(arr)
	#N = ncycle
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

def correctedSampleStandardDeviation(data, c0, amplitude, fi, ncycle):
	N = len(data)
	sum = 0
	for i in xrange(N):
		v = c0+amplitude*math.sin(2*math.pi*i/ncycle+fi)
		sum += (data[i]-v)*(data[i]-v)

	return math.sqrt(sum/(N-1))

def calcAll(period, clock, adc_tick, data):
	ncycle = period/adc_tick
	(c0, csin, ccos) = calcSinCos(period, clock, adc_tick, data)

	(amplitude, fi) = calcFi(csin, ccos)
	square_error = correctedSampleStandardDeviation(data, c0, amplitude, fi, ncycle)
	t_propagation = fi/2*math.pi*ncycle/clock

	return {"period": period, "clock": clock, "adc_tick": adc_tick,
		"c0": c0, "csin": csin, "ccos": ccos, "amplitude": amplitude,
		"fi": fi, "square_error": square_error, "t_propagation": t_propagation}

def main():
	data = readFileAsShort(fileName)
	#period = 720 # F=100000
	#period = 7200 # F=10000
	#period = 3600 # F=20000
	#period = 72000 # F=1000
	period = 120000 # F=600
	#period = 7*120 #F=85714
	clock = 72000000
	adc_tick = 120
	ncycle = period/adc_tick
	#data = [ math.sin(2*math.pi*i/ncycle) for i in xrange(ncycle)]

	print "calcAll=",calcAll(period, clock, adc_tick, data)

	(c0, csin, ccos) = calcSinCos(period, clock, adc_tick, data)

	(amplitude, fi) = calcFi(csin, ccos)

	err = deltaError(data, c0, amplitude, fi, ncycle)
	with open("out_error.dat", "wb") as file:
		arr = array.array('H')
		for i in xrange(len(err)):
			arr.append(int(math.floor(err[i]+0.5+2500)))
		arr.tofile(file)

	print "serror=", correctedSampleStandardDeviation(data, c0, amplitude, fi, ncycle)
	print "dt=", fi/2*math.pi*ncycle/clock
	pass

def goodF():
	Finit = [100,200,300,400,500,600,700,800, 900, 
		 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 
		 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 100000,
		 120000, 150000
	    ]
	FCPU = 72000000
	adc_tick = 120
	min_period = FCPU/adc_tick
	#F = FCPU/(adc_tick*N) = min_period/N
	for f in Finit:
		#print str(min_period/(min_period/f))+", N="+str((min_period/f))
		N = (min_period/f)
		print str(N*adc_tick)+", #"+'{:3.2f}'.format(min_period/float(N))+' Hz'

if __name__ == "__main__":
	#main()
	goodF()