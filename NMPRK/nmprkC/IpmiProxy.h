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
***		transmitted, distributed, or disclosed in any way without Intel�s	  **
***		prior express written permission.									  **
***																			  **
***		No license under any patent, copyright, trade secret or other		  **
***		intellectual property right is granted to or conferred upon you by	  **
***		disclosure or delivery of the Materials, either expressly, by		  **
***		implication, inducement, estoppel or otherwise. Any license under	  **
***		such intellectual property rights must be express and approved by	  **
***		Intel in writing.													  **
********************************************************************************/

#ifndef _IPMI_PROXY_H
#define _IPMI_PROXY_H

#ifdef WIN32
#define _WIN32_DCOM
//#include <atlbase.h>
//#include <wbemidl.h>
//#include <comdef.h>
//#include <SetupAPI.h>
#endif
#include <string>

#include "LastError.h"
#include "nmprkTypes.h"
#include "DebugLog.h"

#if 0
typedef unsigned char bool_t;
typedef unsigned char byte_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;
typedef unsigned long long ulong_t;

#define MAX_IPMI_SIZE 490

#define SI_STATUS_SUCCESS						0
#define SI_STATUS_FAILURE						-1
#define SI_STATUS_INVALID_HANDLE				-2
#define SI_STATUS_INVALID_DATA_LENGTH			-3
#define SI_STATUS_NOT_FOUND						-4
#define SI_STATUS_PROXY_ERROR					-5
#define SI_STATUS_PROXY_ERROR_XFER				-6
#define SI_STATUS_PROXY_ERROR_NO_RSP			-7
#define SI_STATUS_PROXY_ERROR_INVALID_RSP		-8
#define SI_STATUS_PROXY_ERROR_INVALID_ASYNC_RSP	-9
#define SI_STATUS_INVALID_ARGUMENT				-10

/**
 * This data structure is used to describe the IPMI/DCMI request command 
 * to be sent to the UUT. Raw data bytes of the command are stored in the 
 * format specified in IPMI v2.0 specification.
 */
typedef struct ipmi_req_t
{
    /**
     * A field that identifies the functional class of the message. 
     * See IPMI v2.0 specification, section 5.1 for more detailed 
     * information.
     */
    byte_t netFun;

    /**
     * Command identifier, as defined in section 5 of IPMI v2.0 specification.
     */
    byte_t cmd;

    /**
     * The array of data bytes that store the additional info of the IPMI 
     * request. Only the first <code>len</code> bytes are relevant.
     */ 
    byte_t data[MAX_IPMI_SIZE];

    /** 
     * The number of relevant data bytes in the <code>data</code> array.
     */
	int len;
	//size_t len;

    /**
     * Responder's address (this byte is always 0x20 when the BMC is the 
     * responder). See IPMI v2.0 specification, section 5 for more detailed 
     * information.
     */
    byte_t rsAddr;

    /**
     * Responder's LUN (usually 0x00). See IPMI v2.0 specification, 
     * section 5.1 for more detailed information.
     */
    byte_t rsLun;
	
} ipmi_req_t;

/**
 * This data structure is used to pass IPMI response from the transport 
 * plug-in to the main application. The response is stored in the raw 
 * format, exactly as defined in IPMI v2.0 specification.
 */
typedef struct ipmi_rsp_t
{
    /**
     * The completion code returned in response to a previous request. 
     * The completion codes are as defined in IPMI v2.0 specification, 
     * section 5.2 and 5.3.
     */
    byte_t compCode;

    /** 
     * The array of data bytes that store the additional info of the IPMI 
     * response (the completion code is not in this array but in the separate 
     * field <code>compCode</code>). Only the first <code>len</code> bytes are 
     * relevant.
     */
    byte_t data[MAX_IPMI_SIZE];

    /**
     * The number of relevant data bytes in the <code>data</code> array.
     */
    //size_t len;
	int len;
} ipmi_rsp_t;

#endif
class IpmiProxyException : public std::exception
{
public:
    IpmiProxyException(int code, std::string str) { code_ = code; msg_ = str; }
    ~IpmiProxyException() throw () {}
	virtual int code() { return code_; /* TODO: */}
    virtual const char* what() const throw() { return msg_.c_str(); }
private:
    std::string msg_;
	int code_;
};

/**
 * Provides an access to the Microsoft_IPMI interface
 * exposed by the WMI.
 */
class IpmiProxy
{
public:
	static bool GetKcsIpmiProxyInstance(IpmiProxy **proxy, std::string& errorMessage);

    /**
     * Dtor.
     */
	virtual ~IpmiProxy();

    /**
     * Invokes
     */
    //virtual IpmiResponse RequestResponse(const IpmiRequest& req) throw() = 0;

	//virtual IpmiResponse GetAsyncResponse(int msTimeout) throw() = 0;

	virtual ipmi_rsp_t RequestResponse(const ipmi_req_t& req) throw() = 0;

	virtual ipmi_rsp_t GetAsyncResponse(int msTimeout) = 0;

protected:
	/**
     * Ctor.
     */
	IpmiProxy();
};

#endif //#ifndef _IPMI_PROXY_H
