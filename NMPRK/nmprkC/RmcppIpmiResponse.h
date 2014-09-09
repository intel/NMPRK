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

#ifndef _RMCPP_IPMI_RESPONSE_H
#define _RMCPP_IPMI_RESPONSE_H

#include "RmcppTypes.h"

// Following structures as per IPMI 2.0 spec.

#pragma pack(1)

struct get_channel_auth_cap_response
{
    byte_t channelNumber;
    union
    {
        byte_t value;
        struct
        {
            byte_t iolNone      : 1;
            byte_t iolMd2       : 1;
            byte_t iolMd5       : 1;
            byte_t reserved1    : 1;
            byte_t iolStraight  : 1;
            byte_t oemProp      : 1;
            byte_t reserved2    : 1;
            byte_t ipmi2Sup     : 1;
        } b;
    } authTypeSupport;
    union
    {
        byte_t value;
        struct
        {
            byte_t anonymousLoginEnabled    : 1;
            byte_t nullUsernamesEnabled     : 1;
            byte_t nonNullUsernamesEnabled  : 1;
            byte_t userLevelAuthDisabled    : 1;
            byte_t perMsgAuthDisabled       : 1;
            byte_t kgSetToNonZero           : 1;
            byte_t reserved1                : 2;
        } b;
    } authStatus;
    union
    {
        byte_t value;
        struct
        {
            byte_t ipmi15Supported  : 1;
            byte_t ipmi20Supported  : 1;
            byte_t reserved1        : 6;
        } b;
    } extendedCaps;
};

struct rmcpp_os_payload
{
    byte_t type;
    byte_t reserved1[2];
    byte_t length;
    byte_t algorithm;
    byte_t reserved2[3];
};

struct rmcpp_open_session_response
{
    byte_t messageTag;
    byte_t statusCode;
    byte_t maxPrivLevel;
    byte_t reserved1;
    SessionId sidm;
    SessionId sidc;
    rmcpp_os_payload authPayload;
    rmcpp_os_payload integPayload;
    rmcpp_os_payload confidPayload;
};

#pragma warning(disable:4200)
struct rmcpp_rakp2
{
    byte_t messageTag;
    byte_t statusCode;
    byte_t reserved1[2];
    SessionId sidm;
    byte_t randC[16];
    Guid guidC;
    // keyExAuthCode may be 0 or 16 or 20 depending on rakp auth algo
    byte_t keyExAuthCode[0];
};

struct rmcpp_rakp4
{
    byte_t messageTag;
    byte_t statusCode;
    byte_t reserved2[2];
    SessionId sidm;
    // integCheckValue size depends on auth algo
    byte_t integCheckValue[0];
};
#pragma warning(default:4200)

#pragma pack()

#endif

