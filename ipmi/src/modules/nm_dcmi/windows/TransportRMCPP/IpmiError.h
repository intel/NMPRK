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


#ifndef _IPMI_ERROR_H
#define _IPMI_ERROR_H

#include <stdexcept>

namespace RmcppTransportPlugin
{

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

}

#endif

