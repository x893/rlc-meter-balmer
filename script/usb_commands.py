# coding=UTF-8
import usb
import time
import datetime
import os

dev = None

COMMAND_SET_LED = 1


def findDevice():
    global dev
    dev = usb.core.find(idVendor=0x16C0, idProduct=0x05DC)

    if dev is None:
    	raise ValueError('Device not found')
    else:
        print "Device found"

    dev.set_configuration()

def readAll():
    time.sleep(0.1)
    while True:
        try:
            data = dev.read(129, 128, interface=1, timeout=50)
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
        data = dev.read(129, 128, interface=1, timeout=50)
        if len(data)==0:
            print "Read empty"
        else:
            print "readb=", data
    except usb.core.USBError:
        print "Read USB error"
    pass

def readInt():
    data = dev.read(129, 128, interface=1, timeout=50)
    if len(data)!=4:
        print "Fail read int len="+str(len(data))
    else:
        print data[0]+data[1]*0x100+data[2]*0x10000+data[3]*0x1000000
"""
def readCommand():
    time.sleep(0.1)
    try:
        data = dev.read(129, 128, interface=1, timeout=50)
        if len(data)==0:
            print "Read empty"
            return
        else:
            print "readb=", data
    except usb.core.USBError:
        print "Read USB error"
        return

    if data[0]==COMMAND_SET_VOLTAGE:
        print "Voltage channel"+str(data[1]) 
    elif data[0]==COMMAND_GET_ADC:
        v = data[1]+256*data[2]
        print "voltage="+str(v*3.3/4096.0)
    elif data[0]==COMMAND_CONVERSION:
        print "Start conversion"
    else:
        print "Unknown command="+str(data[0])
    pass

def readConversionData():
    with open("out.dat", "wb") as file:
        step = 30
        for i in xrange(0, 34):
            offset = i*30
            print "write=",dev.write(3, [COMMAND_GET_RESULT, offset%256, offset//256], interface=1)
            data = dev.read(129, 128, interface=1, timeout=50)
            values =data[4:]
            print values
            file.write(values)

    pass
"""
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

    if True:
        #readOne()
        for x in xrange(2):
            print "write=",dev.write(3, [COMMAND_SET_LED, ord('B')], interface=1)
            readOne()

    pass


main()

