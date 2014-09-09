/*******************************************************************************
********************************************************************************
***                                                                           **
***							INTEL CONFIDENTIAL								  **
***                    COPYRIGHT 2008 INTEL CORPORATION                       **
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
#include "stdafx.h"

#include "WinExcept.h"
#include <sstream>

using namespace std;

WinError::WinError(const std::string& func, DWORD err) 
        :runtime_error(""), func(func), err(err)
{
}

const char*
WinError::what() const
{
    stringstream ss;
    ss << "WINAPI " << func << " failed rc: " << hex << err;
    wchar_t* msgBuffer = NULL;
    if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
                      NULL, err, LANG_SYSTEM_DEFAULT, (LPWSTR)&msgBuffer, 0, NULL))
    {
        ss << " (" << msgBuffer << ")";
        LocalFree(msgBuffer);
    }
    msg = ss.str();
    return msg.c_str();
}

DWORD
WinError::getErr() const
{
    return err;
}

WinApiError::WinApiError(const std::string& func, DWORD err)
           :WinError(func, err)
{
}

#ifdef _WINSOCKAPI_
WinSockError::WinSockError(const std::string& func, DWORD err)
            :WinError(func, err)
{
}
#endif
