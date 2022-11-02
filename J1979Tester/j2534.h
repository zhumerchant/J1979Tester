/*
  This file is part of IDS.  IDS is free software: you can
  redistribute it and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation, version 2.
 
  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.
 
  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51
  Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 
  Copyright Yann Diorcet
*/

#pragma once

#ifndef _J2534_H
#define _J2534_H

#include <memory>
#include "j2534_v0404.h"

/*
 * OS
 */
#include <exception>
#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else // _WIN32

#include <dlfcn.h>

#endif // _WIN32

/*
 * DLL
 */
#ifdef _WIN32
#ifndef SWIG
#ifdef J2534_EXPORTS
#define J2534_API_API __declspec(dllexport)
#else
#define J2534_API_API __declspec(dllimport)
#endif // J2534_EXPORTS
#else // SWIG
#define J2534_API_API
#endif // SWIG
#else // _WIN32
#define J2534_API_API
#endif // _WIN32

/*
 * Exceptions
 */

class J2534_API_API J2534LoadException : public std::exception {
public:
    J2534LoadException(const char *error);

    virtual const char *what() const noexcept;

private:
    std::string mError;
};


class J2534_API_API J2534FunctionException : public std::exception {
public:
    J2534FunctionException(long code);

    long code() const;

    virtual const char *what() const noexcept;

private:
    long mCode;
};


/*
 * Classes
 */

class J2534Device;

typedef std::shared_ptr <J2534Device> J2534DevicePtr;
typedef std::weak_ptr <J2534Device> J2534DeviceWeakPtr;

class J2534Channel;

typedef std::shared_ptr <J2534Channel> J2534ChannelPtr;
typedef std::weak_ptr <J2534Channel> J2534ChannelWeakPtr;

class J2534Library;

typedef std::shared_ptr <J2534Library> J2534LibraryPtr;
typedef std::weak_ptr <J2534Library> J2534LibraryWeakPtr;

class J2534_API_API J2534Device : public std::enable_shared_from_this<J2534Device> {
    friend class J2534Channel;

public:
    J2534Device(const J2534LibraryPtr &library, unsigned long device);

    virtual ~J2534Device();

    virtual J2534ChannelPtr connect(unsigned long ProtocolID, unsigned long Flags, unsigned long BaudRate);

    virtual void setProgrammingVoltage(unsigned long PinNumber, unsigned long Voltage);

    virtual void readVersion(char *pFirmwareVersion, char *pDllVersion, char *pApiVersion);

    virtual void ioctl(unsigned long IoctlID, void *pInput, void *pOutput);

    virtual J2534LibraryPtr getLibrary() const;

private:
    J2534LibraryPtr mLibrary;
    unsigned long mDeviceID;
};

class J2534_API_API J2534Channel : public std::enable_shared_from_this<J2534Channel> {
public:
    typedef unsigned long TimeType;
    typedef unsigned long PeriodicMessage;
    typedef unsigned long MessageFilter;

    J2534Channel(const J2534DevicePtr &device, unsigned long channel);

    virtual ~J2534Channel();

    virtual size_t readMsgs(std::vector <PASSTHRU_MSG> &msg, TimeType Timeout);

    virtual size_t writeMsgs(std::vector <PASSTHRU_MSG> &msg, TimeType Timeout);

    virtual PeriodicMessage startPeriodicMsg(PASSTHRU_MSG *pMsg, TimeType TimeInterval);

    virtual void stopPeriodicMsg(PeriodicMessage periodicMessage);

    virtual MessageFilter startMsgFilter(unsigned long FilterType, PASSTHRU_MSG *pMaskMsg, PASSTHRU_MSG *pPatternMsg,
                                         PASSTHRU_MSG *pFlowControlMsg);

    virtual void stopMsgFilter(MessageFilter messageFilter);

    virtual void ioctl(unsigned long IoctlID, void *pInput, void *pOutput);

    virtual J2534DevicePtr getDevice() const;

private:
    J2534DevicePtr mDevice;
    unsigned long mChannelID;
};

class J2534_API_API J2534Library : public std::enable_shared_from_this<J2534Library> {
    friend class J2534Device;

    friend class J2534Channel;

public:
    J2534Library(const char *library);

    virtual ~J2534Library();

    virtual J2534DevicePtr open(void *pName);

    virtual void getLastError(char *pErrorDescription);

private:
    struct j2534_fcts {
        PTOPEN passThruOpen;
        PTCLOSE passThruClose;
        PTCONNECT passThruConnect;
        PTDISCONNECT passThruDisconnect;
        PTREADMSGS passThruReadMsgs;
        PTWRITEMSGS passThruWriteMsgs;
        PTSTARTPERIODICMSG passThruStartPeriodicMsg;
        PTSTOPPERIODICMSG passThruStopPeriodicMsg;
        PTSTARTMSGFILTER passThruStartMsgFilter;
        PTSTOPMSGFILTER passThruStopMsgFilter;
        PTSETPROGRAMMINGVOLTAGE passThruSetProgrammingVoltage;
        PTREADVERSION passThruReadVersion;
        PTGETLASTERROR passThruGetLastError;
        PTIOCTL passThruIoctl;
    } mFcts;
#ifdef _WIN32
    HMODULE mModule;
#else //_WIN32
    void *mModule;
#endif //_WIN32
};

#endif //_J2534_H
