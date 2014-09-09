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

#include "RmcppIpmiError.h"

#include <sstream>
#include <iomanip>

using namespace std;

IpmiError::IpmiError(const char* funcName, unsigned char completionCode)
         :runtime_error(""), completionCode(completionCode)
{
    stringstream s;
    s << funcName << ": 0x" << setw(2) << setfill('0') << hex << (int)completionCode;
    msg = s.str();
}

IpmiError::~IpmiError() throw()
{
}

const char*
IpmiError::what() const throw()
{
    return msg.c_str();
}

IpmiGetSessionChallengeError::IpmiGetSessionChallengeError(unsigned char completionCode)
                            :IpmiError("GetSessionChallenge", completionCode)
{
    const char* desc = "Unexpected completion code";
    switch (completionCode)
    {
    case 0x81:
        desc = "Invalid user name";
        break;

    case 0x82:
        desc = "Null user name not enabled";
        break;
    }
    msg += " ";
    msg += desc;
}

IpmiActivateSessionError::IpmiActivateSessionError(unsigned char completionCode)
                        :IpmiError("ActivateSession", completionCode)
{
    const char* desc = NULL;
    switch (completionCode)
    {
    case 0x81:
        desc = "No session slot available";
        break;

    case 0x82:
        desc = "No slot available for user";
        break;

    case 0x83:
        desc = "No slot available due to max privilege capability";
        break;

    case 0x84:
        desc = "Session sequence out of range";
        break;

    case 0x85:
        desc = "Invalid Session ID in request";
        break;

    case 0x86:
        desc = "Requested maximum privilege level exceeds user and/or channel privilege limit";
        break;
    }
    if (NULL != desc)
    {
        msg += " ";
        msg += desc;
    }

}

RmcppStatusCodeError::RmcppStatusCodeError(const char* messageName, unsigned char statusCode)
                    :runtime_error(""), statusCode(statusCode)
{
    stringstream s;
    s << messageName << ": status code 0x" << setw(2) << setfill('0') << hex << (int)statusCode;
    const char* scDesc;
    //Table 13-15, RMCP+ and RAKP Message Status Codes
    switch (statusCode)
    {
    case 0x00:
        scDesc = "OK";
        break;

    case 0x01:
        scDesc = "Insufficient resources to create a session";
        break;

    case 0x02:
        scDesc = "Invalid session ID";
        break;

    case 0x05:
        scDesc = "Invalid integrity algorithm";
        break;

    case 0x11:
        scDesc = "No Cipher Suite match with proposed security algorithm";
        break;

    case 0x0D:
        scDesc = "Invalid username";
        break;

    case 0x0F:
        scDesc = "Invalid integrity check value (wrong password)";
        break;

    default:
        scDesc = "Uknown error";
        break;
    }
    s << " (" << scDesc << ")";
    msg = s.str();
}

RmcppStatusCodeError::~RmcppStatusCodeError() throw()
{
}

const char*
RmcppStatusCodeError::what() const throw()
{
    return msg.c_str();
}
