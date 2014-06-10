# coding=UTF-8
import usb
import time
import datetime
import os
import struct
import array
import math
import cmath
from jplot import calculateJson
import jplot
import json
import smath

DEFAULT_DAC_AMPLITUDE = 1200
#DEFAULT_DAC_AMPLITUDE = 800
COMMAND_SET_LED = 1
COMMAND_SET_FREQUENCY = 2
COMMAND_SET_GAIN = 3
COMMAND_ADC_READ_BUFFER = 4
COMMAND_ADC_ELAPSED_TIME = 5
COMMAND_START_SYNCHRO = 6
COMMAND_SET_RESISTOR = 7
COMMAND_LAST_COMPUTE = 8
COMMAND_REQUEST_DATA = 9
COMMAND_DATA_COMPLETE = 10
COMMAND_SET_LOW_PASS = 11
COMMAND_START_GAIN_AUTO = 12
COMMAND_RVI_INDEXES = 13
COMMAND_SET_CORRECTOR2XR = 14
COMMAND_SET_CORRECTOR2X = 15
COMMAND_SET_CORRECTOR_OPENR = 16
COMMAND_SET_CORRECTOR_OPEN = 17
COMMAND_SET_CORRECTOR_SHORTR = 18
COMMAND_SET_CORRECTOR_SHORT = 19
COMMAND_SET_CORRECTOR_PERIOD = 20
COMMAND_CORRECTOR_FLASH_CLEAR = 21
COMMAND_FLASH_CURRENT_DATA = 22
COMMAND_SET_SERIAL = 23
COMMAND_SET_CONTINUOUS_MODE = 24

LOW_PASS_PERIOD = 24000 #3 KHz

HARDWARE_CORRECTOR_PERIODS = [720000, 72000, 7200, 768, 288]



dev = None
gainVoltageIdx = 0
gainCurrentIdx = 0
resistorIdx = 0
currentLowPass = 0
ncycle = 0
period = 0
clock = 0
amplitude = DEFAULT_DAC_AMPLITUDE

def periodToFreqency(period):
    clock = 72000000
    return clock/period

def getGainValuesX():
    return [1,2,4,5,8,10,16,32]

def getGainCentralIdx():
    return jplot.getGainCentralIdx() #[0,1,2]

def getGainOpenShortIdx():
    return jplot.getGainOpenShortIdx() #[0,1,2,4,6,7]

def inited():
    return not (dev is None)

def findDevice():
    global dev
    dev = usb.core.find(idVendor=0x16C0, idProduct=0x05DC)

    if dev is None:
    	print 'Device not found'
        return False
    else:
        print 'Device found'

    dev.set_configuration()
    return True

def dwrite(data):
    return dev.write(1, data, interface=0)
def dread():
    d = dev.read(129, 128, interface=0, timeout=50)
    return d

def readAll():
    time.sleep(0.1)
    while True:
        try:
            data = dread()
            if len(data)==0:
                break
            print "readb=", data
            print "read=", data.tostring()
        except usb.core.USBError:
            break
    pass

def readOne():
    time.sleep(0.1)
    try:
        data = dread()
        if len(data)==0:
            print "Read empty"
        else:
            #print "readb=", data
            #print "read=", data.tostring()
            pass
    except usb.core.USBError:
        print "Read USB error"
    pass

def readInt():
    data = dread()
    if len(data)!=4:
        print "Fail read int len="+str(len(data))
    else:
        print data[0]+data[1]*0x100+data[2]*0x10000+data[3]*0x1000000

def readCommand():
    global ncycle, period, clock
    time.sleep(0.01)
    try:
        data = dread()
        if len(data)==0:
            print "Read empty"
            return
        else:
            #print "readb=", data
            pass
    except usb.core.USBError:
        print "Read USB error"
        return

    cmd = data[0]
    if cmd==COMMAND_SET_LED:
        print "Set led="+str(data[1]) 
    elif cmd==COMMAND_SET_FREQUENCY:
        period = struct.unpack_from('I', data, 1)[0]
        clock = struct.unpack_from('I', data, 5)[0]
        print "period=",period
        print "clock=",clock
        print "F=",clock/float(period)
    elif cmd==COMMAND_SET_GAIN:
        #print "set gain"
        pass
    elif cmd==COMMAND_ADC_ELAPSED_TIME:
        print "Elapset ticks=", struct.unpack_from('I', data, 1)[0]
    elif cmd==COMMAND_START_SYNCHRO:
        (period, clock, ncycle) = struct.unpack_from('=III', data, 1)
        print "period=",period
        print "clock=",clock
        print "F=",clock/float(period)
        print "ncycle=", ncycle
    elif cmd==COMMAND_SET_RESISTOR:
        r = data[1]
        if r==0:
            print "r=100 Om"
        elif r==1:
            print "r=1 KOm"
        elif r==2:
            print "r=10 KOm"
        elif r==3:
            print "r=100 KOm"
    elif cmd==COMMAND_LAST_COMPUTE:
        pass
    elif cmd==COMMAND_SET_LOW_PASS:
        pass
    elif cmd==COMMAND_START_GAIN_AUTO:
        pass
    else:
        print "Unknown command="+str(data[0])
    pass

def setFreq(F):
    print "write=",dwrite(struct.pack("=BI", COMMAND_SET_FREQUENCY, F))
    readCommand()
    pass

def setSetGain(isVoltage, gain):
    global gainVoltageIdx, gainCurrentIdx
    if isVoltage!=0:
        gainVoltageIdx = gain
    else:
        gainCurrentIdx = gain
    dwrite([ COMMAND_SET_GAIN, isVoltage, gain])
    readCommand()
    pass

def startGainAuto(countComputeX, predefinedResistorIdx=255):
    dwrite(struct.pack("=BBB", COMMAND_START_GAIN_AUTO, countComputeX, predefinedResistorIdx))
    readCommand()
    pass


def setResistor(r):
    global resistorIdx
    resistorIdx = r
    dwrite([COMMAND_SET_RESISTOR, r])
    #readCommand()
    data = dread()
    assert(data[0]==COMMAND_SET_RESISTOR)

def setLowPass(on):
    global currentLowPass
    if on:
        r = 1
    else:
        r = 0        
    currentLowPass = r
    dwrite([COMMAND_SET_LOW_PASS, r])
    data = dread()
    assert(data[0]==COMMAND_SET_LOW_PASS)

def adcElapsedTime():
    dwrite([COMMAND_ADC_ELAPSED_TIME])
    readCommand()
    pass

def adcReadRVI():
    global gainVoltageIdx, gainCurrentIdx, resistorIdx

    dwrite([COMMAND_RVI_INDEXES])
    data = dread()
    assert(data[0]==COMMAND_RVI_INDEXES)
    (resistorIdx, gainVoltageIdx, gainCurrentIdx) = struct.unpack_from('=BBB', data, 1)
    print "V="+str(gainVoltageIdx), "I="+str(gainCurrentIdx), "R="+str(resistorIdx)
    pass

def adcReadBuffer():
    dwrite([COMMAND_ADC_READ_BUFFER])
    time.sleep(0.01)
    data = dread()
    (size, time72, g_adc_cycles) = struct.unpack_from('=III', data, 1)

    #print "adcReadBuffer size=", size
    #print "adcReadBuffer time=", time72
    #print "g_adc_cycles=", g_adc_cycles

    result = None
    while size>0:
        data = dread()
        size -= len(data)/4
        #print size, "d=", len(data)
        #print data
        if not result:
            result = data
        else:
            result += data

    arr = array.array('H')
    arr.fromstring(result)

    arr1 = array.array('H')
    arr2 = array.array('H')
    size2 = len(arr)/2
    for i in xrange(0, size2):
        arr1.append(arr[i])
        arr2.append(arr[i+size2])

    return (arr1, arr2)

def arrByteToShort(barray):
    sarray = array.array('H', barray)
    return sarray.tolist()

def getGainValueV(idx):
    mulPre = 3.74
    mulX = getGainValuesX()[idx]
    return mulPre*mulX

def getGainValueI(idx):
    mulPre = 3.74
    mulX = getGainValuesX()[idx]
    return mulPre*mulX

def getResistorValues():
    return [100, 1000, 10000, 100000]

def getResistorValue(idx):
    r = getResistorValues()
    return r[idx]

def getResistorValueStr(idx):
    r = ['100 Om', '1 KOm', '10 KOm', '100 KOm']
    return r[idx]

def adcSynchro(inPeriod, inAmplitude = None):
    global ncycle, period, clock, amplitude
    if inAmplitude!=None:
        amplitude = inAmplitude
    else:
        amplitude = DEFAULT_DAC_AMPLITUDE
    dwrite(struct.pack("=BIH", COMMAND_START_SYNCHRO, inPeriod, amplitude))
    data = dread()
    #print data
    (period, clock, ncycle) = struct.unpack_from('=III', data, 1)
    #print "period=",period, "freq=", clock/period
    # " cycle_x4=", period/(24.0*4)
    time.sleep(0.1)

def adcRequestData():
    dwrite([COMMAND_REQUEST_DATA]);
    dread()
    time.sleep(0.1)
    dwrite([COMMAND_DATA_COMPLETE]);
    data = dread()
    complete = struct.unpack_from('=B', data, 1)[0]
    if complete!=1:
        print "complete error = ", complete

    (out1, out2) = adcReadBuffer()

    return (out1, out2)

def setCorrector2x(corrector, period):

    for iresistor in xrange(3):
        #iresistor == rezistorIdx
        corr = corrector.corr[iresistor]
        Z1 = corr.Rmin
        Z2 = corr.Rmax

        dwrite(struct.pack("=BBBBff", COMMAND_SET_CORRECTOR2XR, iresistor,0,0,
            Z1, Z2
            ))
        data = dread()
        assert(data[0]==COMMAND_SET_CORRECTOR2XR)
        assert(data[1]==iresistor)

        for gain_index_I in getGainCentralIdx():
            d = corr.data[gain_index_I][period]
            Zm1 = d['min']['R']
            Zm2 = d['max']['R']
            dwrite(struct.pack("=BBBBffff", COMMAND_SET_CORRECTOR2X, iresistor, gain_index_I, 0,
                Zm1.real, Zm1.imag,
                Zm2.real, Zm2.imag
                ))
            data = dread()
            assert(data[0]==COMMAND_SET_CORRECTOR2X)
            assert(data[1]==iresistor)
            assert(data[2]==gain_index_I)
    pass

def setCorrectorOpen(corrector, period, maxAmplitude):
    corr = corrector.corr[3]
    amp = maxAmplitude.getMaxGainI(resistorIndex=3, period=period)
    dwrite(struct.pack("=BBBBff", COMMAND_SET_CORRECTOR_OPENR, amp,0,0,
        corr.R, corr.C
        ))
    data = dread()
    assert(data[0]==COMMAND_SET_CORRECTOR_OPENR)

    for gain_idx in xrange(len(getGainOpenShortIdx())):
        gain_index_I = getGainOpenShortIdx()[gain_idx]
        d = corr.data[gain_index_I][period]
        Zstdm = d['load']['R']
        Zom = d['open']['R']
        dwrite(struct.pack("=BBBBffff", COMMAND_SET_CORRECTOR_OPEN, gain_idx, 0, 0,
            Zstdm.real, Zstdm.imag,
            Zom.real, Zom.imag
            ))
        data = dread()
        assert(data[0]==COMMAND_SET_CORRECTOR_OPEN)
        assert(data[1]==gain_idx)
    pass

def setCorrectorShort(corrector, period):
    corr = corrector.corr_short
    dwrite(struct.pack("=BBBBff", COMMAND_SET_CORRECTOR_SHORTR, 0,0,0,
        corr.R100, corr.R1
        ))
    data = dread()
    assert(data[0]==COMMAND_SET_CORRECTOR_SHORTR)

    for gain_idx in xrange(len(getGainOpenShortIdx())):
        gain_index_I = getGainOpenShortIdx()[gain_idx]
        d = corr.data[gain_index_I][period]
        Zsm = d['short']['R']
        Zstdm = d['load']['R']
        dwrite(struct.pack("=BBBBffff", COMMAND_SET_CORRECTOR_SHORT, gain_idx, 0, 0,
            Zsm.real, Zsm.imag,
            Zstdm.real, Zstdm.imag
            ))
        data = dread()
        assert(data[0]==COMMAND_SET_CORRECTOR_SHORT)
        assert(data[1]==gain_idx)
    pass

def setCorrector(corrector, period, maxAmplitude):
    setCorrector2x(corrector, period)
    setCorrectorOpen(corrector, period, maxAmplitude)
    setCorrectorShort(corrector, period)

    dwrite(struct.pack("=BBBBI", COMMAND_SET_CORRECTOR_PERIOD, 0,0,0, period))
    data = dread()
    assert(data[0]==COMMAND_SET_CORRECTOR_PERIOD)
    pass

def FlashCorrector(corrector, maxAmplitude):
    dwrite([COMMAND_CORRECTOR_FLASH_CLEAR])
    data = dread()
    assert(data[0]==COMMAND_CORRECTOR_FLASH_CLEAR)
    print "flash clear code=", data[1]

    for period in HARDWARE_CORRECTOR_PERIODS:
        setCorrector(corrector, period, maxAmplitude)
        dwrite([COMMAND_FLASH_CURRENT_DATA])
        data = dread()
        assert(data[0]==COMMAND_FLASH_CURRENT_DATA)
        print "flash write code=", data[1]
    pass

def setSerial(ser=True):
    if ser:
        ser = 1
    else:
        ser = 0

    dwrite([COMMAND_SET_SERIAL, ser])
    data = dread()
    assert(data[0]==COMMAND_SET_SERIAL)
    
def setContinuousMode(m=True):
    if m:
        m = 1
    else:
        m = 0

    dwrite([COMMAND_SET_CONTINUOUS_MODE, m])
    data = dread()
    assert(data[0]==COMMAND_SET_CONTINUOUS_MODE)


def adcRequestLastCompute():
    dwrite([COMMAND_REQUEST_DATA]);
    dread()

    for i in xrange(0,20):
        time.sleep(0.01)
        dwrite([COMMAND_DATA_COMPLETE]);
        data = dread()
        complete = struct.unpack_from('=B', data, 1)[0]
        if complete==1:
            break

    if complete!=1:
        print "complete error = ", complete
    #else:
        #print "complete ok"
    return adcLastCompute()

def adcRequestLastComputeX(count=10):
    data = adcRequestLastCompute()
    dataI = data['summary']['I']
    dataV = data['summary']['V']
    for i in xrange(1, count):
        d = adcRequestLastCompute()
        dV = d['summary']['V']
        dI = d['summary']['I']

        dataV['sin'] += dV['sin']
        dataV['cos'] += dV['cos']
        dataV['square_error'] += dV['square_error']
        
        dataI['sin'] += dI['sin']
        dataI['cos'] += dI['cos']
        dataI['square_error'] += dI['square_error']

    dataV['sin'] /= count
    dataV['cos'] /= count
    dataV['square_error'] /= count

    dataI['sin'] /= count
    dataI['cos'] /= count
    dataI['square_error'] /= count
    return data

def adcRequestLastComputeHardAuto(countComputeX, predefinedResistorIdx=255):
    startGainAuto(countComputeX, predefinedResistorIdx)

    for i in xrange(0,20):
        time.sleep(0.1)
        dwrite([COMMAND_DATA_COMPLETE]);
        data = dread()
        complete = struct.unpack_from('=B', data, 1)[0]
        if complete==1:
            break

    if complete!=1:
        print "complete error = ", complete
    else:
        print "complete ok"

    adcReadRVI()    
    return adcLastCompute()


def getMinMax(arr):
    xmin = arr[0]
    xmax = arr[0]
    for x in arr:
        if x<xmin:
            xmin = x
        if x>xmax:
            xmax = x
    return (xmin, xmax)


def setGainAuto(predefinedRes=-1, maxAmplitude=None):
    idxV = 0
    idxI = 0

    goodMin = 2700
    goodMax = 3700

    goodDelta = goodMax-goodMin

    setSetGain(1, 0)
    setSetGain(0, 0)

    resistorValues = getResistorValues()

    if predefinedRes>=0:
        setResistor(predefinedRes)
    else:
        #ищем резистор начиная с минимальных значений, ибо при перегрузе могут быть странные эффекты
        for i in xrange(0, len(resistorValues)):
            setResistor(i)
            jout = adcRequestLastCompute()
            jI = jout['summary']['I']
            imin = jI['min']
            imax = jI['max']
            di = imax - imin
            #print "gainR=", i
            #print " imin="+str(imin)
            #print " imax="+str(imax)

            #прикидываем, что следующий диапазон уже плох
            if di*10>goodDelta:
                break
            pass

    if predefinedRes==0:
        jout = adcRequestLastCompute()

    gainValues = getGainValuesX()
    stopV = False
    stopI = False

    imax = None
    if maxAmplitude:
        imax = maxAmplitude.getMaxGainI(resistorIdx, period)

    if resistorIdx==0:
        jV = jout['summary']['V']
        vmin = jV['min']
        vmax = jV['max']
        jI = jout['summary']['I']
        imin = jI['min']
        imax = jI['max']
        if (imax-imin) < (vmax-vmin):
            stopV = True
            gainIdx = getGainCentralIdx()
            #print "getGainCentralIdx"
        else:
            stopI = True
            gainIdx = getGainOpenShortIdx() #short calibration
            #print "getGainOpenShortIdx"

    elif resistorIdx==3:
        stopV = True
        gainIdx = getGainOpenShortIdx() #open calibration
    else:
        stopV = True
        gainIdx = getGainCentralIdx()

    #print gainIdx
    for i in gainIdx:
        #print i, stopV, stopI
        if imax!=None and i>imax:
            stopI = True

        if not stopV:
            setSetGain(1, i)
        if not stopI:
            setSetGain(0, i)

        jout = adcRequestLastCompute()
        jV = jout['summary']['V']
        vmin = jV['min']
        vmax = jV['max']
        jI = jout['summary']['I']
        imin = jI['min']
        imax = jI['max']
        #print "gainI=", i
        #print " vmin="+str(vmin)
        #print " vmax="+str(vmax)
        #print " imin="+str(imin)
        #print " imax="+str(imax)
        #print " DV="+str(vmax-vmin)
        #print " DI="+str(imax-imin)

        if not stopV and vmax<goodMax and vmin>goodMin:
            idxV = i
        else:
            stopV = True

        if not stopI and imax<goodMax and imin>goodMin:
            idxI = i
        else:
            stopI = True
        if stopI and stopV:
            break

    setSetGain(1, idxV)
    setSetGain(0, idxI)
    print "gain auto", " V="+str(idxV), "I="+str(idxI), "R="+str(resistorIdx)
    pass

def adcLastCompute():
    dwrite([COMMAND_LAST_COMPUTE])
    data = dread()
    (count, 
     adc_min_v, adc_max_v, sin_v, cos_v, mid_v, square_error_v,
     adc_min_i, adc_max_i, sin_i, cos_i, mid_i, square_error_i,
     error, nop_number
        )=struct.unpack_from('=HHHffffHHffffBI', data, 1)

    if count==0:
        count = 1
    #print "nop_number=", nop_number, " error=", error
    jout = {}
    jdata = {}
    jout["attr"] = getAttr()
    jout["summary"] = jdata

    jdata["V"] = {
        "min": adc_min_v,
        "max": adc_max_v,
        "mid": mid_v,
        "sin": sin_v,
        "cos": cos_v,
        "square_error": square_error_v,
    }

    jdata["I"] = {
        "min": adc_min_i,
        "max": adc_max_i,
        "mid": mid_i,
        "sin": sin_i,
        "cos": cos_i,
        "square_error": square_error_i,
    }

    #print jout
    return jout

def getAttr():
    jattr = {}
    jattr["period"] = period
    jattr["clock"] = clock
    jattr["ncycle"] = ncycle
    jattr["gain_index_V"] = gainVoltageIdx
    jattr["gain_index_I"] = gainCurrentIdx
    jattr["gain_V"] = getGainValueV(gainVoltageIdx)
    jattr["gain_I"] = getGainValueI(gainCurrentIdx)
    jattr["resistor_index"] = resistorIdx
    jattr["resistor"] = getResistorValue(resistorIdx)
    jattr["low_pass"] = currentLowPass
    return jattr

def adcSynchroJson(soft=True, corrector = None, count=10):
    (out1, out2) = adcRequestData()
    jout = {}
    jdata = {}

    jout["attr"] = getAttr()
    jout["data"] = jdata

    jdata["V"] = arrByteToShort(out1)
    jdata["I"] = arrByteToShort(out2)

    f = open('out.json', 'w')
    f.write(json.dumps(jout))
    f.close()
    
    f = open('sout.json', 'w')
    #jout = adcLastCompute()
    if soft:
        jout = adcRequestLastComputeX(count)
    else:
        jout = adcRequestLastComputeHardAuto(count)
    f.write(json.dumps(jout))
    f.close()

    if corrector:
        data = corrector.calculateJson(jout)
        R = data['Zx']
    else:
        data = calculateJson(jout)
        R = data['R']

    print "Rre=", R.real
    print "Rim=", R.imag
    print "D={:3.3f} grad".format(cmath.phase(R)*180.0/math.pi)
    print "ErrV=", jout['summary']['V']['square_error']
    print "ErrI=", jout['summary']['I']['square_error']

    pass


def period10Hz_100Hz():
    arr = []
    for freq in xrange(10, 100, 10):
        period = 72000000/freq
        period = (period/96)*96;
        arr.append(period)
    return arr

def period100Hz_1KHz():
    arr = []
    for freq in xrange(100, 1000, 20):
        period = 72000000/freq
        period = (period/96)*96;
        arr.append(period)
    return arr

def period1KHz_10KHz():
    arr = []
    for freq in xrange(1000, 10000, 200):
        period = 72000000/freq
        period = (period/96)*96;
        arr.append(period)
    return arr

def period10Khz_max():
    arr = []
    for period in xrange(75*96, 1*96, -96):
        arr.append(period)
    return arr

def period10Khz_100KHz():
    arr = []
    for period in xrange(75*96, 7*96, -96):
        arr.append(period)
    return arr


def period90Khz_max():
    arr = []
    for period in xrange(10*96, 3*96, -96):
        arr.append(period)
    return arr

def periodAll():
    return period100Hz_1KHz()+period1KHz_10KHz()+period10Khz_max()
    #return period100Hz_1KHz()+period1KHz_10KHz()+period10Khz_100KHz()+period100Khz_max()

def allFreq(amplitude=DEFAULT_DAC_AMPLITUDE, resistorIndex=None, VIndex=None, IIndex=None, fileName='freq.json'):
    sc = ScanFreq()
    sc.init(amplitude=amplitude, resistorIndex=resistorIndex, VIndex=VIndex, IIndex=IIndex, fileName=fileName)
    while sc.next():
        print "f=", periodToFreqency(period), "p=", period
        pass
    sc.save()
    pass

def oneFreq(period, lowPass='auto', inAmplitude = None, maxAmplitude=None, count=None):
    if lowPass=='auto':
        lowPass = (period>=LOW_PASS_PERIOD)

    if count==None:
        if period>=LOW_PASS_PERIOD:
            count = 10
        else:
            count = 100

    adcSynchro(period, inAmplitude=inAmplitude)
    setLowPass(lowPass)
    setGainAuto(maxAmplitude=maxAmplitude)
    time.sleep(0.01)
    return adcRequestLastComputeX(count)

class ScanFreq:
    def init(self, amplitude=DEFAULT_DAC_AMPLITUDE, resistorIndex=None,
             VIndex=None, IIndex=None, fileName='freq.json',
             maxAmplitude=None):

        self.resistorIndex = resistorIndex
        self.VIndex = VIndex
        self.IIndex = IIndex
        self.fileName = fileName
        self.maxAmplitude = maxAmplitude

        #self.resistorIndex = 0
        #self.VIndex = 5
        #self.IIndex = 5

        self.jout = {}
        self.jfreq = []

        self.jout['freq'] = self.jfreq
        #self.PERIOD_ROUND = period10Hz_100Hz()+period100Hz_1KHz()
        #self.PERIOD_ROUND = period1KHz_10KHz()
        #self.PERIOD_ROUND = period10Khz_max()
        #self.PERIOD_ROUND = period100Khz_max()
        #self.PERIOD_ROUND = period10Khz_max()
        #self.PERIOD_ROUND = period90Khz_max()
        self.PERIOD_ROUND = periodAll()
        adcSynchro(self.PERIOD_ROUND[0], amplitude)
        self.current_value = 0
        time.sleep(0.2)
        pass
    def count(self):
        return len(self.PERIOD_ROUND)
    def current(self):
        return self.current_value        
    def next(self):
        global currentLowPass

        possiblePeriod = self.PERIOD_ROUND[self.current_value]
        adcSynchro(possiblePeriod, amplitude)

        oldLowPass = currentLowPass
        if period>=LOW_PASS_PERIOD: #3 KHz
            setLowPass(True)
        else:
            setLowPass(False)

        #print period, oldLowPass, currentLowPass
        if oldLowPass!=currentLowPass:
            #print "time.sleep(1)"
            time.sleep(1)

        if self.VIndex is None:
            if self.resistorIndex is None:
                setGainAuto(maxAmplitude=self.maxAmplitude)
            else:
                setGainAuto(self.resistorIndex, maxAmplitude=self.maxAmplitude)
        else:
            if self.maxAmplitude:
                imax = self.maxAmplitude.getMaxGainI(self.resistorIndex, period)
            else:
                imax = self.IIndex

            setResistor(self.resistorIndex)
            setSetGain(1, self.VIndex) #V
            setSetGain(0, min(self.IIndex, imax)) #I

        time.sleep(0.01)
        if period>=LOW_PASS_PERIOD:
            count = 10
        else:
            count = 30
        jresult = adcRequestLastComputeX(count)
        #jresult = adcRequestLastComputeX(10)
        self.jfreq.append(jresult)

        self.current_value += 1
        return self.current_value<self.count()
    def save(self):
        f = open(self.fileName, 'w')
        f.write(json.dumps(self.jout))
        f.close()

def printEndpoint(e):
    print "Endpoint:"
    print "bLength=", e.bLength
    print "bDescriptorType=", e.bDescriptorType
    print "bEndpointAddress=", e.bEndpointAddress
    print "bmAttributes=", e.bmAttributes
    print "wMaxPacketSize=", e.wMaxPacketSize
    print "bInterval=", e.bInterval
    print "bRefresh=", e.bRefresh
    print "bSynchAddress=", e.bSynchAddress
    pass

def initDevice():
    if not (dev is None):
        return True

    if not findDevice():
        return False
    dev.reset()
    for cfg in dev:
        for i in cfg:
            print "interface=", i.bInterfaceNumber
            for e in i:
                print e.bEndpointAddress
                #printEndpoint(e)

    #for x in xrange(4):
    #    print "write=",dwrite([1])
    #    readOne()
    return True    

def main():
    if not initDevice():
        return

    if False:
        #period = HARDWARE_CORRECTOR_PERIODS[3]
        #period = 19968
        #period = 7488
        #period = 1*96
        period = periodToFreqency(10)

        if True:
            corrector = None
            maxAmplitude = None
        else:
            corrector = jplot.Corrector()
            maxAmplitude = jplot.MaxAmplitude()
        #FlashCorrector(corrector)
        #return

        #if corrector:
        #    setCorrector(corrector, period, maxAmplitude)
        #setCorrector2x(corrector, period)
        #setCorrectorOpen(corrector, period, maxAmplitude=maxAmplitude)
        #dwrite(struct.pack("=BBBBI", COMMAND_SET_CORRECTOR_PERIOD, 0,0,0, period))
        #data = dread()
        #assert(data[0]==COMMAND_SET_CORRECTOR_PERIOD)

        soft = True
        setSerial(False)
        setContinuousMode(False)
        adcSynchro(period, inAmplitude=DEFAULT_DAC_AMPLITUDE)
        #adcSynchro(period, inAmplitude=0)

        if soft:
            setLowPass(False)
            if True:
                setGainAuto()
                #setGainAuto(predefinedRes=3)
            else:
                setResistor(0)
                setSetGain(1, 0) #V
                setSetGain(0, 0) #I
        time.sleep(0.1)
        adcSynchroJson(soft=soft, corrector=corrector)
        #adcSynchroJson(soft=True, count=10)
    else:
        #allFreq(amplitude=DEFAULT_DAC_AMPLITUDE/2, resistorIndex=0, VIndex=0, IIndex=1, fileName='cor/R0V0I1_100Om.json')
        #allFreq(amplitude=DEFAULT_DAC_AMPLITUDE, resistorIndex=0, VIndex=0, IIndex=2, fileName='freq_200Om.json')
        allFreq(fileName='freq_short_100.json')
    pass


if __name__ == "__main__":
    main()

