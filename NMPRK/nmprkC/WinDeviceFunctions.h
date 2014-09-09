/*******************************************************************************
********************************************************************************
***                                                                           **
***							INTEL CONFIDENTIAL								  **
***                    COPYRIGHT 2011 INTEL CORPORATION                       **
***							All Rights Reserved								  **
***                                                                           **
***                INTEL CORPORATION PROPRIETARY INFORMATION                  **
***                                                                           **
***		The source code contained or described herein and all documents		  **
***		related to the source code ("Material") are owned by Intel			  **
***		Corporation or its suppliers or licensors. Title to the Material	  **
***		remains with Intel Corporation or its suppliers and licensors.		  **
***		The Material contains trade secrets and proprietary and confidential  **
***		information of Intel or its suppliers and licensors.				  **
***		The Material is protected by worldwide copyright and trade secret	  **
***		laws and treaty provisions. No part of the Material may be used,	  **
***		copied, reproduced, modified, published, uploaded, posted,			  **
***		transmitted, distributed, or disclosed in any way without Intel’s	  **
***		prior express written permission.									  **
***																			  **
***		No license under any patent, copyright, trade secret or other		  **
***		intellectual property right is granted to or conferred upon you by	  **
***		disclosure or delivery of the Materials, either expressly, by		  **
***		implication, inducement, estoppel or otherwise. Any license under	  **
***		such intellectual property rights must be express and approved by	  **
***		Intel in writing.													  **
********************************************************************************/
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

bool IsDeviceInstalled(wchar_t *classGuid, std::wstring deviceName);
bool IsDeviceEnabled(wchar_t *classGuid, std::wstring deviceName);
SP_DEVINFO_DATA *GetDeviceInfo(wchar_t *classGuid, std::wstring deviceName, HDEVINFO &hDevInfo);
void ChangeDeviceState(wchar_t *classGuid, std::wstring deviceName, bool enable);