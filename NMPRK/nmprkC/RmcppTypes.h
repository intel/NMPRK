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

#ifndef _RMCPP_TYPES_H
#define _RMCPP_TYPES_H

#include <ostream>
#include "IpmiProxy.h"

// fwd declarations
class ByteVector;

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

#pragma pack(1)

// redefined here to not include "transport.h"
//typedef unsigned char byte_t;

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
 * Prints literal value of the Auth Type enumerator.
 * @param os Output stream.
 * @param authType Auth Type.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const ipmi_auth_type& authType);

/**
 * Prints literal value of the Auth Algo enumerator.
 * @param os Output stream.
 * @param authType Auth Algo.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const ipmi_auth_algo& authAlgo);

/**
 * Prints literal value of the Integ Algo enumerator.
 * @param os Output stream.
 * @param authType Integ Algo.
 * @return Reference to <code>os</code>.
 */
std::ostream& operator<<(std::ostream& os, const ipmi_integ_algo& integAlgo);

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

#pragma pack()

#endif //#ifndef _RMCPP_TYPES_H
