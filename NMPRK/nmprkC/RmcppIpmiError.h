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

#ifndef _RMCPP_IPMI_ERROR_H
#define _RMCPP_IPMI_ERROR_H

#include <stdexcept>

/// Represents IPMI error indicated by non-zero completion code.
class IpmiError : public std::runtime_error
{
public:
    IpmiError(const char* funcName, unsigned char completionCode);
    virtual ~IpmiError() throw();
    inline unsigned char getCompletionCode() const;
    virtual const char* what() const throw();

protected:
    std::string msg;

private:
    unsigned char completionCode;
};

inline
unsigned char
IpmiError::getCompletionCode() const
{
    return completionCode;
}

/** 
 * Handles base IPMI error completion codes as well as
 * specific Get Session Challange completion codes.
 */
class IpmiGetSessionChallengeError : public IpmiError
{
public:
    IpmiGetSessionChallengeError(unsigned char completionCode);
};

/** 
 * Handles base IPMI error completion codes as well as
 * specific Activate Session completion codes.
 */
class IpmiActivateSessionError : public IpmiError
{
public:
    IpmiActivateSessionError(unsigned char completionCode);
};

/// Represents RMCP+ error indicated by non-zero Status Code.
class RmcppStatusCodeError : public std::runtime_error
{
public:
    RmcppStatusCodeError(const char* messageName, unsigned char statusCode);
    virtual ~RmcppStatusCodeError() throw();
    inline unsigned char getStatusCode() const;
    virtual const char* what() const throw();
private:
    unsigned char statusCode;
    std::string msg;
};

inline
unsigned char
RmcppStatusCodeError::getStatusCode() const
{
    return statusCode;
}

#endif

