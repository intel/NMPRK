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

#include "RmcppTypes.h"
#include "ByteVector.h"

#include <iomanip>

using namespace std;

namespace RmcppTransportPlugin
{

std::ostream& AppendByteArray(std::ostream& os, const byte_t* arr, size_t length)
{
    os << "[ ";
    for (size_t i = 0; i < length; ++i)
    {
        os << hex << setw(2) << setfill('0') << (int)arr[i] << " ";
    }
    return os << "]";
}

std::ostream&
operator<<(std::ostream& os, const SessionId& sid)
{
    return AppendByteArray(os, sid, sizeof(SessionId));
}


std::ostream&
operator<<(std::ostream& os, const Guid& guid)
{
    return AppendByteArray(os, guid, sizeof(Guid));
}

std::ostream&
operator<<(std::ostream& os, const ByteVector& bv)
{
    return AppendByteArray(os, bv.c_ptr(), bv.length());
}

std::ostream& operator<<(std::ostream& os, const ipmi_auth_type& authType)
{
    const char* AUTH_TYPE_STR[] = { "NONE", "MD2", "MD5", "STRAIGHT", "RMCPP" };
    return os << AUTH_TYPE_STR[authType];
}

std::ostream& operator<<(std::ostream& os, const ipmi_auth_algo& authAlgo)
{
    const char* AUTH_ALGO_STR[] = { "RAKP_NONE", "RAKP_HMAC_SHA1", "RAKP_HMAC_MD5" };
    return os << AUTH_ALGO_STR[authAlgo];
}

std::ostream& operator<<(std::ostream& os, const ipmi_integ_algo& integAlgo)
{
    const char* INTEG_ALGO_STR[] = { "NONE", "HMAC_SHA1_96", "HMAC_MD5_128", "MD5_128" };
    return os << INTEG_ALGO_STR[integAlgo];
}

std::ostream& operator<<(std::ostream& os, const ipmi_confid_algo& confidAlgo)
{
    const char* CONFID_ALGO_STR[] = { "NONE", "AES_CBC_128" };
    return os << CONFID_ALGO_STR[confidAlgo];
}

}

