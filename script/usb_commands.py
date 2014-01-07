# coding=UTF-8
import usb
import time
import datetime
import os
import struct
import array
import math
import smath
import json

dev = None
gainVoltageIdx = 0
gainCurrentIdx = 0
resistorIdx = 0

COMMAND_SET_LED = 1
COMMAND_SET_FREQUENCY = 2
COMMAND_SET_GAIN = 3
COMMAND_ADC_READ_BUFFER = 4
COMMAND_ADC_ELAPSED_TIME = 5
COMMAND_START_SYNCHRO = 6
COMMAND_SET_RESISTOR = 7
COMMAND_LAST_COMPUTE = 8

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
    return dev.read(129, 128, interface=0, timeout=50)

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
        (adc_min_v, adc_max_v, count_v, sin_sum_v, cos_sum_v, mid_sum_v, 
         adc_min_i, adc_max_i, count_i, sin_sum_i, cos_sum_i, mid_sum_i,
         error, nop_number
            )=struct.unpack_from('=HHHffIHHHffIBI', data, 1)

        if count_v==0:
            count_v = 1
        if count_i==0:
            count_i = 1
        print "adc_min_v=", adc_min_v, " adc_max_v=", adc_max_v, " count_v=", count_v, " mid_sum_v=", mid_sum_v/count_v
        print " sin_v=", sin_sum_v
        print " cos_v=", cos_sum_v

        print "adc_min_i=", adc_min_i, " adc_max_i=", adc_max_i, " count_i=", count_i, " mid_sum_i=", mid_sum_i/count_i
        print " sin_i=", sin_sum_i
        print " cos_i=", cos_sum_i
        print "nop_number=", nop_number, " error=", error
    else:
        print "Unknown command="+str(data[0])
    pass
"""
def readConversionData():
    with open("out.dat", "wb") as file:
        step = 30
        for i in xrange(0, 34):
            offset = i*30
            print "write=",dev.write(3, [COMMAND_GET_RESULT, offset%256, offset//256], interface=0)
            data = dev.read(129, 128, interface=1, timeout=50)
            values =data[4:]
            print values
            file.write(values)

    pass
"""

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

def adcStart():
    dwrite([COMMAND_ADC_START])
    readCommand()
    pass

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
    print "adcReadBuffer time=", time72
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

def getGainValue(idx):
    mulPre = 3.74
    mulX = [1,2,4,5,8,10,16,32]
    return mulPre*mulX[idx]

def getResistorValue(idx):
    r = [100, 1000, 10000, 100000]
    return r[idx]

def adcSynchro(inPeriod):
    time.sleep(0.1)
    dwrite(struct.pack("=BIB", COMMAND_START_SYNCHRO, inPeriod, 2))
    data = dread()
    (period, clock, ncycle, num_skip) = struct.unpack_from('=IIIB', data, 1)

    time.sleep(0.1)
    (out1, out2) = adcReadBuffer()

    return (period, clock, ncycle, out1, out2)

def adcSynchroBin(inPeriod):
    (period, clock, ncycle, out1, out2) = adcSynchro(inPeriod)
    print "period=",period
    print "clock=",clock
    print "F=",clock/float(period)
    print "ncycle=", ncycle
    print "num_skip=", num_skip
    with open("out1.dat", "wb") as file1:
        out1.tofile(file1)
    with open("out2.dat", "wb") as file2:
        out2.tofile(file2)
    pass

def getMinMax(arr):
    xmin = arr[0]
    xmax = arr[0]
    for x in arr:
        if x<xmin:
            xmin = x
        if x>xmax:
            xmax = x
    return (xmin, xmax)


def setGainAuto(inPeriod):
    idxV = 0
    idxI = 0

    goodMin = 500
    goodMax = 3500

    for i in xrange(0,8):
        setSetGain(1, i)
        setSetGain(0, i)
        (period, clock, ncycle, outV, outI) = adcSynchro(inPeriod)
        (vmin, vmax) = getMinMax(outV)
        (imin, imax) = getMinMax(outI)
        print i
        print " vmin="+str(vmin)
        print " vmax="+str(vmax)
        print " imin="+str(imin)
        print " imax="+str(imax)
        if vmin>=goodMin and vmax<=goodMax:
            idxV = i
        if imin>=goodMin and imax<=goodMax:
            idxI = i

    setSetGain(1, idxV)
    setSetGain(0, idxI)
    print "gain auto", " V="+str(idxV), "I="+str(idxI)
    pass

def adcSynchroJson(inPeriod):
    (period, clock, ncycle, out1, out2) = adcSynchro(inPeriod)
    jout = {}
    jattr = {}
    jdata = {}
    jattr["period"] = period
    jattr["clock"] = clock
    jattr["ncycle"] = ncycle
    jattr["gain_index_V"] = gainVoltageIdx
    jattr["gain_index_I"] = gainCurrentIdx
    jattr["gain_V"] = getGainValue(gainVoltageIdx)
    jattr["gain_I"] = getGainValue(gainCurrentIdx)
    jattr["resistor_index"] = resistorIdx
    jattr["resistor"] = getResistorValue(resistorIdx)

    jout["attr"] = jattr
    jout["data"] = jdata

    jdata["V"] = arrByteToShort(out1)
    jdata["I"] = arrByteToShort(out2)

    f = open('out.json', 'w')
    f.write(json.dumps(jout))
    f.close()

    dwrite([COMMAND_LAST_COMPUTE])
    readCommand()

    pass

def adcSynchro1(inPeriod):
    (period, clock, ncycle, out1, out2) = adcSynchro(inPeriod)
    print "period=",period, "clock=",clock , "F=",clock/float(period), "ncycle=", ncycle

    result1 = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=out1)
    result2 = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=out2)

    fiV = result1['fi']
    fiI = result2['fi']+math.pi

    if fiV > math.pi:
        fiV -= math.pi*2
    if fiI > math.pi:
        fiI -= math.pi*2

    dfi = fiV-fiI+math.pi
    if dfi > math.pi:
        dfi -= math.pi*2


    print "fiV=", fiV
    print "fiI=", fiI
    print "dfi=", dfi/math.pi*180, "grad"

    return (result1, result2)

def setResistor(r):
    global resistorIdx
    resistorIdx = r
    dwrite([COMMAND_SET_RESISTOR, r])
    readCommand()


def allFreq():
    out = []

    with open("data.py", "wb") as file:
        P = 72*10, #100 000 Hz
        #P = 7200 #10000.00 Hz
        #P = 3600 #20000.00 Hz
        PERIOD_ROUND = [P]*10
        for period in PERIOD_ROUND:
            dwrite([COMMAND_SET_LED, ord('0')])
            readCommand()
            da = adcSynchro1(period)

            for data in da:
                print>>file, 'ticks=', '{:3.2f}'.format(72000000*data['t_propagation']), 'fi=', data['fi'], ' amplitude='+'{:3.1f}'.format(data['amplitude'])
                #print>>file, data


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

    #setFreq(10000)
    #adcStart()

    freq = 100000
    period = 72000000/freq
    setResistor(0)
    #setGainAuto(period)
    setSetGain(1, 7) #V
    setSetGain(0, 1) #I
    time.sleep(0.3)
    #adcSynchroBin(period)
    adcSynchroJson(period)
    #res = adcSynchro1(period)
    #print "quants=", res[1]['t_propagation']
    #print "ticks=", res[0]['t_propagation']*72000000
    #allFreq()
    pass


main()

