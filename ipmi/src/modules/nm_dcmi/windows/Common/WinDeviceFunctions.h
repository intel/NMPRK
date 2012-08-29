/***************************************************************************
 * Copyright 2012 Intel Corporation                                        *
 *Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 * http://www.apache.org/licenses/LICENSE-2.0                              *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/
/**************************************************************************
 * Author: Stewart Dale <IASI NM TEAM>                                    *
 * Updates:							                                      *
 * 4/30: prep for initial external release                                *
 **************************************************************************/

#define _WIN32_DCOM
#include <iostream>
#include <sstream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include <SetupAPI.h>
#include <Cfgmgr32.h>

class WinDeviceFunctionsException : public std::exception
{
public:
    WinDeviceFunctionsException(std::string str) { msg_ = str; }
    ~WinDeviceFunctionsException() throw () {}
    virtual const char* what() const throw () { return msg_.c_str(); }
private:
    std::string msg_;
};

bool IsDeviceInstalled(std::wstring deviceName);
bool IsDeviceEnabled(std::wstring deviceName);
SP_DEVINFO_DATA *GetDeviceInfo(std::wstring deviceName, HDEVINFO &hDevInfo);
void ChangeDeviceState(std::wstring deviceName, bool enable);