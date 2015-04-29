//-----------------------------------------------------------------------------
// Title      : Open a device via a device interface
// Project    : Driver development for Windows
//-----------------------------------------------------------------------------
// File       : DeviceDriverAccess.cpp
// Author     : Reichoer Stefan
// Created    : 09.04.2003
// Platform   : Windows 2000, Python2.4
//-----------------------------------------------------------------------------
// Description:
// This is an extension DLL for Python to access a device driver from
// python code
//-----------------------------------------------------------------------------
// Copyright (c) 2003-2004 Stefan Reichör
//-----------------------------------------------------------------------------


#include "afxwin.h"
#include "Python.h"

//needed for GetDeviceViaInterface
#include <objbase.h>
#include <setupapi.h>
#include <initguid.h>


void initDeviceDriverAccess();

PyObject *ShowVersion(PyObject *self, PyObject *args)
{
    PyObject *result = PyString_FromString("DeviceDriverAccess compiled for Python " PY_VERSION ", built on " __DATE__ ", " __TIME__);
    return result;
}

/**
   Opens a device via a device interface.
*/
PyObject *GetDeviceViaInterface(PyObject *self, PyObject *args)
{
    char *guidData;
    DWORD guidSize;
    DWORD instance=0;

    GUID* pGuid;

    if (!PyArg_ParseTuple(args, "s#|l:GetDeviceViaInterface",
                          &guidData, &guidSize, // @pyparm string|data||The data to write.
                          &instance))
        return NULL;

    pGuid = (LPGUID) guidData;

    // Get handle to relevant device information set
    HDEVINFO info = SetupDiGetClassDevs(pGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if(info==INVALID_HANDLE_VALUE)
    {
        //printf("No HDEVINFO available for this GUID\n");
        return NULL;
    }

    // Get interface data for the requested instance
    SP_INTERFACE_DEVICE_DATA ifdata;
    ifdata.cbSize = sizeof(ifdata);
    if(!SetupDiEnumDeviceInterfaces(info, NULL, pGuid, instance, &ifdata))
    {
        //printf("No SP_INTERFACE_DEVICE_DATA available for this GUID instance\n");
        SetupDiDestroyDeviceInfoList(info);
        return NULL;
    }

    // Get size of symbolic link name
    DWORD ReqLen;
    SetupDiGetDeviceInterfaceDetail(info, &ifdata, NULL, 0, &ReqLen, NULL);
    PSP_INTERFACE_DEVICE_DETAIL_DATA ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(new char[ReqLen]);
    if( ifDetail==NULL)
    {
        SetupDiDestroyDeviceInfoList(info);
        return NULL;
    }

    // Get symbolic link name
    ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
    if( !SetupDiGetDeviceInterfaceDetail(info, &ifdata, ifDetail, ReqLen, NULL, NULL))
    {
        SetupDiDestroyDeviceInfoList(info);
        delete ifDetail;
        return NULL;
    }

    //printf("Symbolic link is %s\n",ifDetail->DevicePath);

    PyObject *result = PyString_FromString(ifDetail->DevicePath);

    delete ifDetail;
    return result;
}

//#palign  "{" 40, " \w+" 45
static PyMethodDef DeviceDriverAccessMethods[] = {
    {"ShowVersion",                         ShowVersion, METH_VARARGS},
    {"GetDeviceViaInterface",               GetDeviceViaInterface, METH_VARARGS},
    {NULL,                        NULL}        /* Sentinel */
};

void initDeviceDriverAccess() {
    Py_InitModule("DeviceDriverAccess", DeviceDriverAccessMethods);
}


// Local Variables:
// c++-font-lock-extra-types: ("Py[A-Z]\\sw+" "DWORD" "GUID" "PSP_INTERFACE_DEVICE_DETAIL_DATA" "SP_INTERFACE_DEVICE_DATA" "LPGUID" "HDEVINFO")
// compile-command: "scons"
// End:

// arch-tag: 98b5b538-24e3-4cc5-9701-61948b3d1c59
