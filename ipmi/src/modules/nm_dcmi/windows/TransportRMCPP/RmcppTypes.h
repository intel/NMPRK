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

#ifndef _RMCPP_TYPES_H
#define _RMCPP_TYPES_H

#include <ostream>

// fwd declarations
class ByteVector;

namespace RmcppTransportPlugin
{

#pragma pack(1)

// redefined here to not include "transport.h"
typedef unsigned char byte_t;

/// IPMI Session ID
typedef byte_t SessionId[4];

/// IPMI Sequence number
typedef byte_t Sequence[4];

/// GUID 
typedef byte_t Guid[16];

/**
 * Prints content of the specified array in
 * format "[ 0f 12 aa 34 ]".
 *
 * @param os Output stream.
 * @param arr Pointer to the array to be printed.
 * @param length Length of the array.
 * 
 * @return Reference to <code>os</code>.
 */
std::ostream& AppendByteArray(std::ostream& os, const byte_t* arr, size_t length);

/**
 * Prints the value of the session id.
 * @param os Output stream.
 * @param sid Session id.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const SessionId& sid);

/**
 * Prints the value of the guid.
 * @param os Output stream.
 * @param guid Guid.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const Guid& guid);

/**
 * Prints the value of the byte vector.
 * @param os Output stream.
 * @param bv Byte vector.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const ByteVector& bv);


/**
 * Auth Type / Format
 * per IPMI spec Table 13-8.
 */
enum ipmi_auth_type
{
    AUTH_TYPE_NONE              = 0x00,
    AUTH_TYPE_MD2               = 0x01,
    AUTH_TYPE_MD5               = 0x02,
    AUTH_TYPE_STRAIGHT          = 0x04,
    AUTH_TYPE_RMCPP             = 0x06
};

/**
 * Prints literal value of the Auth Type enumerator.
 * @param os Output stream.
 * @param authType Auth Type.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const ipmi_auth_type& authType);

/**
 * Authentication Algorithm Numbers
 * per IPMI spec Table 13-17.
 *
 * Algorithms not enumerated are unsupported
 * by the RMCPP transport plug-in.
 */
enum ipmi_auth_algo
{
    AUTH_ALGO_RAKP_NONE         = 0x00,
    AUTH_ALGO_RAKP_HMAC_SHA1    = 0x01,
    AUTH_ALGO_RAKP_HMAC_MD5     = 0x02,
};

/**
 * Prints literal value of the Auth Algo enumerator.
 * @param os Output stream.
 * @param authType Auth Algo.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const ipmi_auth_algo& authAlgo);

/**
 * Integrity Algorithm Numbers
 * per IPMI spec Table 13-18.
 *
 * Algorithms not enumerated are unsupported
 * by the RMCPP transport plug-in.
 */
enum ipmi_integ_algo
{
    INTEG_ALGO_NONE             = 0x00,
    INTEG_ALGO_HMAC_SHA1_96     = 0x01,
    INTEG_ALGO_HMAC_MD5_128     = 0x02,
    INTEG_ALGO_MD5_128          = 0x03
};

/**
 * Prints literal value of the Integ Algo enumerator.
 * @param os Output stream.
 * @param authType Integ Algo.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const ipmi_integ_algo& integAlgo);

/**
 * Confidentionality Algorithm Numbers
 * per IPMI spec Table 13-19.
 *
 * Algorithms not enumerated are unsupported
 * by the RMCPP transport plug-in.
 */
enum ipmi_confid_algo
{
    CONFID_ALGO_NONE            = 0x00,
    CONFID_ALGO_AES_CBC_128     = 0x01
};

/**
 * Prints literal value of the Confid Algo enumerator.
 * @param os Output stream.
 * @param authType Confid Algo.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const ipmi_confid_algo& confidAlgo);


/**
 * Payload Type Numbers
 * per IPMI spec Table 13-16.
 */
enum ipmi_payload
{
    PAYLOAD_IPMI_MESSAGE        = 0x00,
    PAYLOAD_SOL                 = 0x01,
    PAYLOAD_OEM                 = 0x02,
    PAYLOAD_RMCPP_OPEN_SES_REQ  = 0x10,
    PAYLOAD_RMCPP_OPEN_SES_RSP  = 0x11,
    PAYLOAD_RMCPP_RAKP1         = 0x12,
    PAYLOAD_RMCPP_RAKP2         = 0x13,
    PAYLOAD_RMCPP_RAKP3         = 0x14,
    PAYLOAD_RMCPP_RAKP4         = 0x15
};

/**
 * Payload Type per IPMI spec Table 13-8.
 */
typedef union _ipmi_payload_t
{
     byte_t value;
     struct _bit_fields
     {
         byte_t type             : 6;
         byte_t authenticated    : 1;
         byte_t encrypted        : 1;
     } b;
} ipmi_payload_t;

}

#pragma pack()

#endif //#ifndef _RMCPP_TYPES_H
