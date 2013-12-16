# coding=UTF-8
import usb
import time
import datetime
import os
import struct
import array
import math
import smath

dev = None

COMMAND_SET_LED = 1
COMMAND_SET_FREQUENCY = 2
COMMAND_ADC_START = 3
COMMAND_ADC_READ_BUFFER = 4
COMMAND_ADC_ELAPSED_TIME = 5
COMMAND_START_SYNCHRO = 6
COMMAND_SET_RESISTOR = 7

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
            print "readb=", data
            #print "read=", data.tostring()
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
            print "readb=", data
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
    elif cmd==COMMAND_ADC_START:
        print "adcStart"
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
    size = struct.unpack_from('=I', data, 1)[0]

    print "adcReadBuffer size=", size

    result = None
    while size>0:
        data = dread()
        #print data
        size -= len(data)/4
        if not result:
            result = data
        else:
            result += data

    arr = array.array('H')
    arr.fromstring(result)

    arr1 = array.array('H')
    arr2 = array.array('H')
    for i in xrange(0, len(arr), 2):
        arr1.append(arr[i])
        arr2.append(arr[i+1])
    #print arr
    return (arr1, arr2)

def adcSynchro(inPeriod):
    print "adcStartSynchro=",dwrite(struct.pack("=BIB", COMMAND_START_SYNCHRO, inPeriod, 1))
    data = dread()
    (period, clock, ncycle) = struct.unpack_from('=III', data, 1)
    print "period=",period
    print "clock=",clock
    print "F=",clock/float(period)
    print "ncycle=", ncycle
    time.sleep(1)
    (out1, out2) = adcReadBuffer()
    with open("out1.dat", "wb") as file1:
        out1.tofile(file1)
    with open("out2.dat", "wb") as file2:
        out2.tofile(file2)
    pass

def adcSynchro1(inPeriod):
    print "adcStartSynchro=",dwrite(struct.pack("=BIB", COMMAND_START_SYNCHRO, inPeriod, 1))
    data = dread()
    (period, clock, ncycle) = struct.unpack_from('=III', data, 1)
    print "period=",period, "clock=",clock , "F=",clock/float(period), "ncycle=", ncycle
    time.sleep(0.3)
    (out1, out2) = adcReadBuffer()

    result1 = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=out1)
    result2 = smath.calcAll(period=period, clock=clock, ncycle=ncycle, data=out2)
    return (result1, result2)

def setResistor(r):
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

    if False:
        #readOne()
        for x in xrange(3):
            print "write=",dwrite([COMMAND_SET_LED, ord('0')])
            readCommand()

    #readOne()
    print "write=",dwrite([1])
    readOne()
    print "write=",dwrite([1])
    readOne()
    print "write=",dwrite([1])
    readOne()
    print "write=",dwrite([1])
    readOne()

    #setFreq(10000)
    #adcStart()

    freq = 10000
    period = 72000000/freq
    #setResistor(2)
    time.sleep(0.3)
    adcSynchro(period)
    #res = adcSynchro1(period)
    #print "quants=", res[1]['t_propagation']
    #print "ticks=", res[1]['t_propagation']*72000000
    #allFreq()
    pass


main()

