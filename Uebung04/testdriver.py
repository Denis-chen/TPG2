# Test file for Wdm1


import sys, serial, array
from time import sleep
my_platform = "";

if sys.platform == "win32":
    my_platform = "win32"
    import win32file, win32api
    sys.path += ["DeviceDriverAccess/Release"]
    from DeviceDriverAccess import GetDeviceViaInterface
    
    # Constants for Wdm1
    WDM1_GUID = pack("LHHBBBBBBBB", 0x1ef8a96b, 0x6c26, 0x42a4, 0xb9, 0x19, 0x82, 0x50, 0x93, 0x13, 0xbc, 0x5b)

    def CTL_CODE(DeviceType, Function, Method, Access):
        return (DeviceType << 16) | (Access << 14) | (Function << 2) | Method

elif sys.platform == "linux2":
    my_platform = "linux2"
    
    from fcntl import ioctl

    IOCTL_READ_DATABIT = -2147195904
    IOCTL_READ_CLOCKBIT = -2147195903
    IOCTL_WRITE_DATABIT = -2147195902
    IOCTL_WRITE_CLOCKBIT = -2147195901


import time
from struct import *


FILE_DEVICE_UNKNOWN = 0x00000022
METHOD_BUFFERED = 0
METHOD_IN_DIRECT = 1
METHOD_OUT_DIRECT = 2
METHOD_NEITHER = 3
FILE_ANY_ACCESS = 0

ZERO_BUFFER = 0x801
REMOVE_BUFFER = 0x802
GET_BUFFER_SIZE = 0x803
GET_BUFFER = 0x804
UNRECOGNISED = 0x805
GET_BUILDTIME = 0x806
READ_DATABIT = 0x807
READ_CLOCKBIT = 0x808
WRITE_DATABIT = 0x809
WRITE_CLOCKBIT = 0x810

RTC_BASEADDRESS = 0x51

DEVICE_FILENAME = "/home/user/TPG2/char_dev"

class HWDevice:
    def __init__(self):
        if my_platform == "win32":
            self.guid = WDM1_GUID
        self.drvHnd = None
        self.fd = -1
        self.OpenDrv()
        self.data = 0
        self.clock = 0        

    def OpenDrv(self):
        """
        Open a handle to the device driver. If the driver is already open,
        close it first an reopen it.
        """
        self.CloseDrv()

        # Windows Open --------
        if my_platform == "win32":
            try:
                name = GetDeviceViaInterface(self.guid)
            except:
                raise IOError (1, "Wdm1 Device not found")

            desiredAccess = win32file.GENERIC_READ | win32file.GENERIC_WRITE
            self.drvHnd = win32file.CreateFile(name,
                                               desiredAccess,
                                               win32file.FILE_SHARE_WRITE,
                                               None,
                                               win32file.OPEN_EXISTING,
                                               0,
                                               0)
        # Linux open ---------
        elif my_platform == "linux2":
           self.fd = open(DEVICE_FILENAME, "r+")
           print self.fd
           if (self.fd < 0):
               print "cant open device"
               return
	
            

    def CloseDrv(self):
        """
        Close the handle to device driver
        """
        if my_platform == "win32" and self.drvHnd is not None:
            win32file.CloseHandle(self.drvHnd)
            self.drvHnd = None
        elif my_platform == "linux2" and self.fd >= 0:
            self.fd.close()

    def Write(self, string):
        win32file.WriteFile(self.drvHnd, string, None)

    def Read(self, numofbytes=1):
        hr, result = win32file.ReadFile(self.drvHnd, numofbytes, None)
        return result

    def SetFilePointer(self, distance):
        win32file.SetFilePointer(self.drvHnd, distance, win32file.FILE_BEGIN)

    def DeviceIoControl(self, function, input):

        if my_platform == "win32":
            IOCTL_USB_GET_DEVICE_DESCRIPTOR = CTL_CODE(FILE_DEVICE_UNKNOWN, function, METHOD_BUFFERED, FILE_ANY_ACCESS)

            try:
                result = win32file.DeviceIoControl(self.drvHnd, IOCTL_USB_GET_DEVICE_DESCRIPTOR, input, 512)
            except win32file.error, e:
                print "problem with driver or stack over/underflow"
                print "Unexpected error:", e
                result = 0
        elif my_platform == "linux2":            
            if function == READ_DATABIT:
                result = ioctl(d.fd, IOCTL_READ_DATABIT, 0); 
            elif function == READ_CLOCKBIT:
                result = ioctl(d.fd, IOCTL_READ_CLOCKBIT, 0);
            elif function == WRITE_DATABIT:
                result = ioctl(d.fd, IOCTL_WRITE_DATABIT, input);
            elif function == WRITE_CLOCKBIT:
                result = ioctl(d.fd, IOCTL_WRITE_CLOCKBIT, input);

            #print "set msg"
            #print ioctl(d.fd, IOCTL_WRITE_DATABIT, pack("b",1));
            #print fcntl.fcntl(d.fd, IOCTL_SET_MSG, "test msg")
        
            #print "get msg"
            #buf = array.array('h', [0])
            #print fcntl.fcntl(d.fd, IOCTL_GET_MSG)

            #result = 0
            #self.fd.write("asdfasf TestString ...")
            #result = self.fd.read()

        return result


    def ReadI2C(self, adress, register, numOfBytes=1):
        self.SendStartI2C()

        ack = self.WriteByteI2C(adress << 1)
        #print "ReadI2C adress ack: %d" % ack

        ack = self.WriteByteI2C(register)
        #print "ReadI2C register ack: %d" % ack

        self.SendStartI2C()

        ack = self.WriteByteI2C((adress << 1) + 1)
        #print "ReadI2C adress 2 ack: %d" % ack

        byteList = []
        for x in range(0, numOfBytes):
            ack = 0

            if x == numOfBytes-1:
                ack = 1
            else:
                ack = 0

            byteList.append(self.ReadByteI2C(ack));
            #print "ReadI2C value ack: %d" % ack

        self.SendStopI2C()

        return byteList

    def WriteI2C(self, adress, register, data):
        self.SendStartI2C()

        ack = self.WriteByteI2C(adress << 1)
        #print "WriteI2C adress ack: %d" % ack

        ack = self.WriteByteI2C(register)
        #print "WriteI2C register ack: %d" % ack

        for dataByte in data:
            ack = self.WriteByteI2C(dataByte)
            #print "WriteI2C dataByte ack: %d" % ack

        self.SendStopI2C()

    def ReadByteI2C(self, ack):
        bit7 = self.ReadBitI2C()
        bit6 = self.ReadBitI2C()
        bit5 = self.ReadBitI2C()
        bit4 = self.ReadBitI2C()
        bit3 = self.ReadBitI2C()
        bit2 = self.ReadBitI2C()
        bit1 = self.ReadBitI2C()
        bit0 = self.ReadBitI2C()

        byte = (bit7 << 7) + (bit6 << 6) + (bit5 << 5) + (bit4 << 4) + (bit3 << 3) + (bit2 << 2) + (bit1 << 1) + bit0

        #send ack
        self.WriteBitI2C(ack)

        return byte

    def WriteByteI2C(self, byte):
        self.WriteBitI2C(byte&128 != 0)
        self.WriteBitI2C(byte&64 != 0)
        self.WriteBitI2C(byte&32 != 0)
        self.WriteBitI2C(byte&16 != 0)
        self.WriteBitI2C(byte&8 != 0)
        self.WriteBitI2C(byte&4 != 0)
        self.WriteBitI2C(byte&2 != 0)
        self.WriteBitI2C(byte&1 != 0)

        #send ack
        ack = self.ReadBitI2C()

        return ack

    def ReadBitI2C(self):
        self.WriteClock(True)
        bit = self.ReadData()
        self.WriteClock(False)
        return bit

    def WriteBitI2C(self, value):
        bit = self.WriteData(value)
        self.WriteClock(True)
        self.WriteClock(False)

    def SendStartI2C(self):
        self.WriteData(True)

        self.WriteClock(True)
        self.WriteData(False)
        self.WriteClock(False)
        #print "-----"

    def SendStopI2C(self):
        #print "-----"
        self.WriteData(False)

        self.WriteClock(True)
        self.WriteData(True)        
        #self.WriteClock(False)

    def ReadClock(self):
        clockBit = self.DeviceIoControl(READ_CLOCKBIT ,"")
        result = unpack("b",clockBit)[0]
        return abs(result-1)
        #print "Read clock: %d" % self.clock
        #return self.clock

    def WriteClock(self, isOn):
        if isOn == False:
            self.DeviceIoControl(WRITE_CLOCKBIT ,pack("b",1))
            #self.clock = 1
        else:
            self.DeviceIoControl(WRITE_CLOCKBIT ,pack("b",0))
            #self.clock = 0

        #print "Write clock %d" % self.clock

    def ReadData(self):
        dataBit = self.DeviceIoControl(READ_DATABIT ,"")
        #result = unpack("b",dataBit)[0]
        return abs(dataBit-1)
        #print "Read data: %d" % self.data
        #return self.data

    def WriteData(self, isOn):
        if isOn == False:
            self.DeviceIoControl(WRITE_DATABIT ,pack("b",1))
            #self.data = 1
        else:
            self.DeviceIoControl(WRITE_DATABIT ,pack("b",0))
            #self.data = 0

        #print "Write data %d" % self.data

    def GetTime(self):
        #return "%d:%d:%d" % d.ReadI2C(RTC_BASEADDRESS, 4)[0].decode("utf-8"), d.ReadI2C(RTC_BASEADDRESS, 3)[0].decode("utf-8"), d.ReadI2C(RTC_BASEADDRESS, 2)[0].decode("utf-8")
        string = "%d:%d:%d" % (d.ReadI2C(RTC_BASEADDRESS, 4)[0], d.ReadI2C(RTC_BASEADDRESS, 3)[0], d.ReadI2C(RTC_BASEADDRESS, 2)[0])
        return string


d = HWDevice()

ser = serial.Serial(0)

#for i in range(1,100):
   # print d.DeviceIoControl(READ_DATABIT, 0)

   # print "write data 0"
 #   d.DeviceIoControl(WRITE_DATABIT ,pack("b",1))
  #  print d.DeviceIoControl(WRITE_CLOCKBIT ,pack("b",1))
   # sleep(1)
    #print d.DeviceIoControl(READ_CLOCKBIT, 0)
    
    #print d.DeviceIoControl(WRITE_DATABIT ,pack("b",0))
    #print "write clock 1"
    #d.DeviceIoControl(WRITE_CLOCKBIT ,pack("b",0))
    #sleep(1)

#print d.DeviceIoControl(READ_DATABIT, 0)


print d.GetTime()
print("Sleep for 4 seconds")
time.sleep(4);
print d.GetTime()


ser.close()
d.CloseDrv()
