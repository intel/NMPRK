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

#include "RmcppTypes.h"
#include "ByteVector.h"

#include <iomanip>

using namespace std;

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
