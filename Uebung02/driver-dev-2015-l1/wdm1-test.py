# Test file for Wdm1

import win32file, win32api, sys
sys.path += ["DeviceDriverAccess/Release"]

from DeviceDriverAccess import GetDeviceViaInterface

from struct import *

# Constants for Wdm1
WDM1_GUID = pack("LHHBBBBBBBB", 0x1ef8a96b, 0x6c26, 0x42a4, 0xb9, 0x19, 0x82, 0x50, 0x93, 0x13, 0xbc, 0x5b)

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

def CTL_CODE(DeviceType, Function, Method, Access):
    return (DeviceType << 16) | (Access << 14) | (Function << 2) | Method

class HWDevice:    
    def __init__(self,guid):
        self.guid = guid
        self.drvHnd = None
        self.OpenDrv()

    def OpenDrv(self):
        """
        Open a handle to the device driver. If the driver is already open,
        close it first an reopen it.
        """
        self.CloseDrv()
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

    def CloseDrv(self):
        """
        Close the handle to device driver
        """
        if self.drvHnd is not None:
            win32file.CloseHandle(self.drvHnd)
            self.drvHnd = None

    def Write(self, string):
        win32file.WriteFile(self.drvHnd, string, None)

    def Read(self, numofbytes=1):
        hr, result = win32file.ReadFile(self.drvHnd, numofbytes, None)
        return result

    def SetFilePointer(self, distance):
        win32file.SetFilePointer(self.drvHnd, distance, win32file.FILE_BEGIN)

    def DeviceIoControl(self, function):      
        
        IOCTL_USB_GET_DEVICE_DESCRIPTOR = CTL_CODE(FILE_DEVICE_UNKNOWN, function, METHOD_BUFFERED, FILE_ANY_ACCESS)

        result = win32file.DeviceIoControl(self.drvHnd, IOCTL_USB_GET_DEVICE_DESCRIPTOR, "", 512)        
        return result


d = HWDevice(WDM1_GUID)

print "Clear buffer ..."
d.DeviceIoControl(REMOVE_BUFFER)

bufferLength = d.DeviceIoControl(GET_BUFFER_SIZE)
result, = unpack('i', bufferLength)
print "Buffer length should be zero. Buffer Length = %d" % result

print "Write buffer ('Hello World Buffer! :D') ..."
d.Write("Hello World Buffer! :D")

bufferLength = d.DeviceIoControl(GET_BUFFER_SIZE)
result, = unpack('i', bufferLength)
print "Buffer length after write = %d" % result

print "Read 5 bytes from buffer ..."
result = d.Read(5)
print "Read bytes = %s" % result

print "Move FilePointer 5 bytes back ..."
d.SetFilePointer(5)

print "Read 50 bytes from buffer ..."
result = d.Read(50)
print "Read bytes = %s" % result

print "Clear buffer ..."
d.DeviceIoControl(REMOVE_BUFFER)

bufferLength = d.DeviceIoControl(GET_BUFFER_SIZE)
result, = unpack('i', bufferLength)
print "Buffer length should be zero. Buffer Length = %d" % result
