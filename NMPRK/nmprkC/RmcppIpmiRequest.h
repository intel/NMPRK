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

#ifndef _RMCPP_IPMI_REQUEST_H
#define _RMCPP_IPMI_REQUEST_H

#include "RmcppTypes.h"
#include "ByteVector.h"
#include "nmprkTypes.h"

/// This intermediate class, useful to initiate request with the defaults.
struct InternalIpmiReq : ipmi_req_t
{
    /// Ctor.
    InternalIpmiReq();
};

/// Initiates req for Get Channel Auth Cap message.
struct IpmiGetChannelAuthCap : InternalIpmiReq
{
    IpmiGetChannelAuthCap();
};

/// Initiates req for Get DCMI Capability Info Command.
struct IpmiGetDcmiCapInfo : InternalIpmiReq
{
	IpmiGetDcmiCapInfo();
};
/// Initiates the req for Open Session message.
struct IpmiOpenSession : InternalIpmiReq
{
    IpmiOpenSession(byte_t messageTag, byte_t maxPrivLevel, const SessionId& sidm, 
                    ipmi_auth_algo authAlgo, ipmi_integ_algo integAlgo, ipmi_confid_algo confidAlgo);
};

/// Initiates the req for RAKP-1 message.
struct IpmiRakp1 : InternalIpmiReq
{
    IpmiRakp1(byte_t messageTag, const SessionId& sidc, const ByteVector& randM, byte_t roleM,
              const ByteVector& uNameM);
};

/// Initiates the req for RAKP-3 message.
struct IpmiRakp3 : InternalIpmiReq
{
    IpmiRakp3(byte_t messageTag, const SessionId& sidc, const ByteVector& digest);
};

/// Initiates the req for Disconnect message.
struct IpmiDisconnectReq : InternalIpmiReq
{
    IpmiDisconnectReq(const SessionId& sidc);
};

#endif
