/*
  Copyright (C) 2016 Yann Diorcet
  
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
*/

#include <string.h>
#include "j2534.h"

#ifdef _WIN32
#define LOAD_FCT(proxy_handle, name, type, dest) { \
    dest = (type)GetProcAddress(proxy_handle, #name); \
    if(dest == NULL) { \
        throw J2534LoadException("Can't load \"#name\" function from library"); \
    } \
}
#else // _WIN32
#define LOAD_FCT(proxy_handle, name, type, dest) { \
    dest = (type)dlsym(proxy_handle, #name); \
    if(dest == NULL) { \
        throw J2534LoadException("Can't load \"#name\" function from library"); \
    } \
}
#endif // _WIN32


J2534LoadException::J2534LoadException(const char *error) {
    if (error != NULL) {
        mError.append(error);
    }
}

const char *J2534LoadException::what() const noexcept {
    return mError.c_str();
}

J2534FunctionException::J2534FunctionException(long code) : mCode(code) {
}

long J2534FunctionException::code() const {
    return mCode;
}

const char *J2534FunctionException::what() const noexcept {
    static char buffer[256];
    snprintf(buffer, 256, "Error code: %ld", mCode);
    return buffer;
}

J2534Device::J2534Device(const J2534LibraryPtr &library, unsigned long device) : mLibrary(library), mDeviceID(device) {

}

J2534Device::~J2534Device() {
    long ret = mLibrary->mFcts.passThruClose(mDeviceID);
    if (ret != STATUS_NOERROR) {
        //throw J2534FunctionException(ret);
    }
}

J2534ChannelPtr J2534Device::connect(unsigned long ProtocolID, unsigned long Flags, unsigned long BaudRate) {
    unsigned long ChannelID;
    long ret = mLibrary->mFcts.passThruConnect(mDeviceID, ProtocolID, Flags, BaudRate, &ChannelID);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
    return std::make_shared<J2534Channel>(shared_from_this(), ChannelID);
}

void J2534Device::setProgrammingVoltage(unsigned long PinNumber, unsigned long Voltage) {
    long ret = mLibrary->mFcts.passThruSetProgrammingVoltage(mDeviceID, PinNumber, Voltage);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
}

void J2534Device::readVersion(char *pFirmwareVersion, char *pDllVersion, char *pApiVersion) {
    long ret = mLibrary->mFcts.passThruReadVersion(mDeviceID, pFirmwareVersion, pDllVersion, pApiVersion);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
}

void J2534Device::ioctl(unsigned long IoctlID, void *pInput, void *pOutput) {
    long ret = mLibrary->mFcts.passThruIoctl(mDeviceID, IoctlID, pInput, pOutput);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
}

J2534LibraryPtr J2534Device::getLibrary() const {
    return mLibrary;
}

J2534Channel::J2534Channel(const J2534DevicePtr &device, unsigned long channel) : mDevice(device), mChannelID(channel) {

}

J2534Channel::~J2534Channel() {
    long ret = mDevice->mLibrary->mFcts.passThruDisconnect(mChannelID);
    if (ret != STATUS_NOERROR) {
        //throw J2534FunctionException(ret);
    }
}

size_t J2534Channel::readMsgs(std::vector <PASSTHRU_MSG> &msg, TimeType Timeout) {
    unsigned long tmpNumMsgs = msg.size();
    long ret = mDevice->mLibrary->mFcts.passThruReadMsgs(mChannelID, &msg[0], &tmpNumMsgs, Timeout);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
    return tmpNumMsgs;
}

size_t J2534Channel::writeMsgs(std::vector <PASSTHRU_MSG> &msg, TimeType Timeout) {
    unsigned long tmpNumMsgs = msg.size();
    long ret = mDevice->mLibrary->mFcts.passThruWriteMsgs(mChannelID, &msg[0], &tmpNumMsgs, Timeout);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
    return tmpNumMsgs;
}

J2534Channel::PeriodicMessage J2534Channel::startPeriodicMsg(PASSTHRU_MSG *pMsg, TimeType TimeInterval) {
    unsigned long MsgID;
    long ret = mDevice->mLibrary->mFcts.passThruStartPeriodicMsg(mChannelID, pMsg, &MsgID, TimeInterval);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
    return MsgID;
}

void J2534Channel::stopPeriodicMsg(PeriodicMessage perdiodicMessage) {
    long ret = mDevice->mLibrary->mFcts.passThruStopPeriodicMsg(mChannelID, perdiodicMessage);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
}

J2534Channel::MessageFilter J2534Channel::startMsgFilter(unsigned long FilterType, PASSTHRU_MSG *pMaskMsg,
                                                         PASSTHRU_MSG *pPatternMsg, PASSTHRU_MSG *pFlowControlMsg) {
    unsigned long FilterID;
    long ret = mDevice->mLibrary->mFcts.passThruStartMsgFilter(mChannelID, FilterType, pMaskMsg, pPatternMsg,
                                                               pFlowControlMsg, &FilterID);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
    return FilterID;
}

void J2534Channel::stopMsgFilter(MessageFilter messageFilter) {
    long ret = mDevice->mLibrary->mFcts.passThruStopMsgFilter(mChannelID, messageFilter);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
}

void J2534Channel::ioctl(unsigned long IoctlID, void *pInput, void *pOutput) {
    long ret = mDevice->mLibrary->mFcts.passThruIoctl(mChannelID, IoctlID, pInput, pOutput);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
}

J2534DevicePtr J2534Channel::getDevice() const {
    return mDevice;
}

J2534Library::J2534Library(const char *library) {
#ifdef _WIN32
    mModule = LoadLibraryA(library);
#else // _WIN32
    mModule = dlopen(library, RTLD_LAZY);
#endif // _WIN32
    if (!mModule) {
        throw J2534LoadException("Can't load library");
    }
    try {
        LOAD_FCT(mModule, PassThruOpen, PTOPEN, mFcts.passThruOpen);
        LOAD_FCT(mModule, PassThruClose, PTCLOSE, mFcts.passThruClose);
        LOAD_FCT(mModule, PassThruConnect, PTCONNECT, mFcts.passThruConnect);
        LOAD_FCT(mModule, PassThruDisconnect, PTDISCONNECT, mFcts.passThruDisconnect);
        LOAD_FCT(mModule, PassThruReadMsgs, PTREADMSGS, mFcts.passThruReadMsgs);
        LOAD_FCT(mModule, PassThruWriteMsgs, PTWRITEMSGS, mFcts.passThruWriteMsgs);
        LOAD_FCT(mModule, PassThruStartPeriodicMsg, PTSTARTPERIODICMSG, mFcts.passThruStartPeriodicMsg);
        LOAD_FCT(mModule, PassThruStopPeriodicMsg, PTSTOPPERIODICMSG, mFcts.passThruStopPeriodicMsg);
        LOAD_FCT(mModule, PassThruStartMsgFilter, PTSTARTMSGFILTER, mFcts.passThruStartMsgFilter);
        LOAD_FCT(mModule, PassThruStopMsgFilter, PTSTOPMSGFILTER, mFcts.passThruStopMsgFilter);
        LOAD_FCT(mModule, PassThruSetProgrammingVoltage, PTSETPROGRAMMINGVOLTAGE, mFcts.passThruSetProgrammingVoltage);
        LOAD_FCT(mModule, PassThruReadVersion, PTREADVERSION, mFcts.passThruReadVersion);
        LOAD_FCT(mModule, PassThruGetLastError, PTGETLASTERROR, mFcts.passThruGetLastError);
        LOAD_FCT(mModule, PassThruIoctl, PTIOCTL, mFcts.passThruIoctl);
    } catch (J2534LoadException &exception) {
#ifdef _WIN32
        FreeLibrary(mModule);
#else // _WIN32
        dlclose(mModule);
#endif // _WIN32
        throw exception;
    }
}

J2534Library::~J2534Library() {
#ifdef _WIN32
    FreeLibrary(mModule);
#else // _WIN32
    dlclose(mModule);
#endif // _WIN32
}

J2534DevicePtr J2534Library::open(void *pName) {
    unsigned long DeviceID;
    long ret = mFcts.passThruOpen(pName, &DeviceID);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
    return std::make_shared<J2534Device>(shared_from_this(), DeviceID);
}

void J2534Library::getLastError(char *pErrorDescription) {
    long ret = mFcts.passThruGetLastError(pErrorDescription);
    if (ret != STATUS_NOERROR) {
        throw J2534FunctionException(ret);
    }
}
