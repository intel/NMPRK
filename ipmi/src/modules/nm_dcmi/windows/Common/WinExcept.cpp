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
    char* msgBuffer = NULL;
    if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
                      NULL, err, LANG_SYSTEM_DEFAULT, (LPSTR)&msgBuffer, 0, NULL))
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
