import math
import array


fileName = "out.dat"
def readFileAsShort(filename):
	with open(filename, "rb") as file:
		data = file.read()
		arr = array.array('H')
		arr.fromstring(data)
	return arr

def calcSinCos(period, clock, ncycle, data):
	'''
		return (c0,csin, ccos)
		c0+csin*sin(f)+ccos*cos(f)
	'''
	arr = array.array('f', data)
	#print "ncycle=", ncycle
	fsin = [ math.sin(2*math.pi*i/ncycle) for i in xrange(ncycle)]
	fcos = [ math.cos(2*math.pi*i/ncycle) for i in xrange(ncycle)]
	
	N = len(data)

	#print "N =", len(data), len(arr)
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

	#print "c0=", c0
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
	#print "amplitude=", "{:.2f}".format(amplitude)
	csin /= amplitude
	ccos /= amplitude

	#print "sin=", math.acos(csin)
	#print "cos=", math.asin(ccos)
	fi = math.atan2(ccos, csin)
	#print "fi=", fi
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

def calcAll(period, clock, ncycle, data):
	(c0, csin, ccos) = calcSinCos(period, clock, ncycle, data)

	(amplitude, fi) = calcFi(csin, ccos)
	square_error = correctedSampleStandardDeviation(data, c0, amplitude, fi, ncycle)
	t_propagation = fi/(2*math.pi)*ncycle/clock

	print "square_error=", square_error

	return {"period": period, "clock": clock, "ncycle": ncycle,
		"c0": c0, "csin": csin, "ccos": ccos, "amplitude": amplitude,
		"fi": fi, "square_error": square_error, "t_propagation": t_propagation}



def main():
	pass

if __name__ == "__main__":
	main()
