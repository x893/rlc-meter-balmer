# coding=UTF-8
import usb
import time
import datetime
import os
import struct
import array
import math
import smath
from jplot import calculateJson
import json

dev = None
gainVoltageIdx = 0
gainCurrentIdx = 0
resistorIdx = 0
ncycle = 0
period = 0
clock = 0

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

PERIOD_ROUND = [
        72*10000, #100 Hz
        72*4000, #250 Hz
        72*2000, #500 Hz
        72*1000, #1 000 Hz
        72*400, #2 500 Hz
        72*200, #5 000 Hz
        72*100, #10 000 Hz
        72*40, #25 000 Hz
        72*20, #50 000 Hz
        72*10, #100 000 Hz
        72*4, #250 000 Hz
        192, #375 000 Hz
        ]


def findDevice():
    global dev
    dev = usb.core.find(idVendor=0x16C0, idProduct=0x05DC)

    if dev is None:
    	raise ValueError('Device not found')
    else:
        print "Device found"

    dev.set_configuration()

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

def setResistor(r):
    global resistorIdx
    resistorIdx = r
    dwrite([COMMAND_SET_RESISTOR, r])
    #readCommand()
    data = dread()
    assert(data[0]==COMMAND_SET_RESISTOR)

def adcElapsedTime():
    dwrite([COMMAND_ADC_ELAPSED_TIME])
    readCommand()
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

def getGainValuesX():
    return [1,2,4,5,8,10,16,32]

def getGainValuesXV(idx):
    return getGainValuesX()[idx]

def getGainValuesXI(idx):
    k = getGainValuesX()[idx]

    #if resistorIdx==0:
    #    #100 Om
    #    k0 = [0.9854473080477981, 0.9875172259658508, 0.9882146508255546, 0.9912066417767416, 0.9931192698984098, 0.9921645049747444]
    #    if idx<=5:
    #        k /= k0[idx]

    #if resistorIdx==1:
    #    #1 KOm
    #    k0 = [0.9867411190934384, 0.9882005031539411, 0.9894009477327412, 0.9922429422273069, 0.9940168413611414, 0.9931864720269113]
    #    if idx<=5:
    #        k /= k0[idx]        

    return k

def getGainValueV(idx):
    mulPre = 3.74
    mulX = getGainValuesXV(idx)
    return mulPre*mulX

def getGainValueI(idx):
    mulPre = 3.74
    mulX = getGainValuesXI(idx)
    return mulPre*mulX

def getResistorValues():
    return [100, 1000, 10000, 100000]

def getResistorValue(idx):
    r = getResistorValues()
    return r[idx]

def adcSynchro(inPeriod):
    global ncycle, period, clock
    dwrite(struct.pack("=BI", COMMAND_START_SYNCHRO, inPeriod))
    data = dread()
    #print data
    (period, clock, ncycle) = struct.unpack_from('=III', data, 1)
    print "period=",period, " cycle_x4=", period/(24.0*4)
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

def adcRequestLastCompute():
    dwrite([COMMAND_REQUEST_DATA]);
    dread()

    for i in xrange(0,10):
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


def getMinMax(arr):
    xmin = arr[0]
    xmax = arr[0]
    for x in arr:
        if x<xmin:
            xmin = x
        if x>xmax:
            xmax = x
    return (xmin, xmax)


def setGainAuto():
    idxV = 0
    idxI = 0

    goodMin = 1500
    goodMax = 4000
    #goodMin = 500
    #goodMax = 3300

    goodDelta = goodMax-goodMin
    goodDeltaIdx = [goodDelta, goodDelta, goodDelta, goodDelta, goodDelta, 600, 600, 600]

    setSetGain(1, 0)
    setSetGain(0, 0)

    resistorValues = getResistorValues()

    #ищем резистор с максимальным значением, при котором нет перегруза
    if False:
        for i in xrange(len(resistorValues)-1, -1, -1):
            setResistor(i)
            jout = adcRequestLastCompute()
            jI = jout['summary']['I']
            imin = jI['min']
            imax = jI['max']
            #print "gainR=", i
            #print " imin="+str(imin)
            #print " imax="+str(imax)
            if imin>=goodMin and imax<=goodMax:
                break
            pass
    else:
        #ищем резистор начиная с минимальных значений, ибо при перегрузе могут быть странные эффекты
        for i in xrange(0, len(resistorValues)):
            setResistor(i)
            jout = adcRequestLastCompute()
            jI = jout['summary']['I']
            imin = jI['min']
            imax = jI['max']
            di = imax - imin
            print "gainR=", i
            print " imin="+str(imin)
            print " imax="+str(imax)

            #прикидываем, что следующий диапазон уже плох
            if di*10>goodDelta:
                break
            pass


    gainValues = getGainValuesX()
    stopV = False
    stopI = False
    for i in xrange(0, len(gainValues)):
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
        delta = goodDeltaIdx[i]

        if not stopV and vmax-vmin<delta:
            idxV = i
        else:
            stopV = True

        if not stopI and imax-imin<delta:
            idxI = i
        else:
            stopI = True

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
    return jattr

def adcSynchroJson():
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
    jout = adcRequestLastComputeX()
    f.write(json.dumps(jout))
    f.close()

    data = calculateJson(jout)
    print "Rre=", data['Rre']
    print "Rim=", data['Rim']

    pass


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
    for period in xrange(75*96, 96, -96):
        arr.append(period)
    return arr

def periodAll():
    return period100Hz_1KHz()+period1KHz_10KHz()+period10Khz_max()

def allFreq():
    jout = {}
    jfreq = []

    jout['freq'] = jfreq
    #PERIOD_ROUND = period100Hz_1KHz()
    #PERIOD_ROUND = period1KHz_10KHz()
    #PERIOD_ROUND = period10Khz_max()
    PERIOD_ROUND = periodAll()
    print PERIOD_ROUND
    for period in PERIOD_ROUND:
        adcSynchro(period)
        setGainAuto()
        time.sleep(0.01)
        jresult = adcRequestLastComputeX()
        jfreq.append(jresult)
        pass

    f = open('freq.json', 'w')
    f.write(json.dumps(jout))
    f.close()

def calibrate1Kom():
    period = periodByFreq(123)
    Kout = []
    adcSynchro(period)
    setResistor(0)
    for igain in xrange(0, 6):
        setSetGain(1, 0) #V
        setSetGain(0, igain) #I
        time.sleep(0.01)
        jresult = adcRequestLastComputeX()
        data = calculateJson(jresult)
        Kout.append(data['Rre']/1e3)

    print "KI 1Kom = ", Kout
    pass

def calibrate10Kom():
    period = periodByFreq(123)
    Kout = []
    adcSynchro(period)
    setResistor(1)
    for igain in xrange(0, 6):
        setSetGain(1, 0) #V
        setSetGain(0, igain) #I
        time.sleep(0.01)
        jresult = adcRequestLastComputeX()
        data = calculateJson(jresult)
        Kout.append(data['Rre']/10e3)

    print "KI 10Kom = ", Kout
    pass

def calibrate10_Om():
    R = 9.9
    Rzero = 0.0098
    period = periodByFreq(123)
    Kout = []
    adcSynchro(period)
    setResistor(0)
    for vgain in xrange(0, 4):
        setSetGain(1, vgain) #V
        setSetGain(0, 0) #I
        time.sleep(0.01)
        jresult = adcRequestLastComputeX()
        data = calculateJson(jresult)
        Kout.append((data['Rre']-Rzero)/R)

    print "KV 10 Om = ", Kout
    pass

def calibrate1_Om():
    R = 0.99
    Rzero = 0.0098
    period = periodByFreq(123)
    Kout = []
    adcSynchro(period)
    setResistor(0)
    for vgain in xrange(4, 8):
        setSetGain(1, vgain) #V
        setSetGain(0, 0) #I
        time.sleep(0.01)
        jresult = adcRequestLastComputeX()
        data = calculateJson(jresult)
        Kout.append((data['Rre']-Rzero)/R)

    print "KV 1 Om = ", Kout
    pass

def periodByFreq(freq):
    return 72000000/freq

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

def main():
    findDevice()
    dev.set_configuration()
    for cfg in dev:
        for i in cfg:
            print "interface=", i.bInterfaceNumber
            for e in i:
                print e.bEndpointAddress
                #printEndpoint(e)

    #readOne()
    for x in xrange(4):
        print "write=",dwrite([1])
        readOne()

    #calibrate1_Om()
    #return

    if False:
        #period = periodByFreq(123)
        period = periodByFreq(6500)
        #period = periodByFreq(6646)
        #period = 384

        adcSynchro(period)

        if True:
            setGainAuto()
        else:
            setResistor(0)
            setSetGain(1, 5) #V
            setSetGain(0, 0) #I
        time.sleep(0.1)
        adcSynchroJson()
    else:
        allFreq()
    pass


main()

