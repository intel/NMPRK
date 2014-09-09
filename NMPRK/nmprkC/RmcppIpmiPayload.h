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

#ifndef _RMCPP_IPMI_REQUEST_PAYLOAD
#define _RMCPP_IPMI_REQUEST_PAYLOAD

#include "RmcppTypes.h"
#include "ByteVector.h"
#include "nmprkTypes.h"

class IpmiPayloadData;

class IpmiPayload
{
public:
 
    static void serializeRmcpp(ByteVector& out, const IpmiPayloadData& payload);
    /**
     * Deserializes the payload assuming RMCP+ format (2-byte payload length
     * on the beginning).
     * @param outData Receives the payload. Caller has to parse the buffer
     *    and create appropriate <code>IpmiPayloadData</code> object from it.
     * @param in Input data.
     * @param pos Current position in <code>in</code>. Upon input, the <code>pos</code>
     *      indicates the position that the serialization should start on.
     *      Upon return, indicates the position next to the last byte consumed
     *      by the deserializer.
     */
    static void deserializeRmcpp(ByteVector& outData, const ByteVector& in, size_t& pos);

    /// Encrypts the data and serializes with <code>serializeRmcpp()</code>.
    static void serializeEncryptedRmcpp(ByteVector& out, const ByteVector& key, const IpmiPayloadData& payload);

    /// Deserializes the data with <code>deserializeRmcpp()</code> and decrypts it.
    static void deserializeEncryptedRmcpp(ByteVector& outData, const ByteVector& key, const ByteVector& in, size_t& pos);
};

/**
 * Base class representing message payload copies the payload data as is
 * without any modification and instrumentation.
 */
class IpmiPayloadData
{
public:
    /// Data buffer.
    ByteVector data;
	virtual byte_t getCompletionCode() const { return (byte_t) 0xFF;};
	virtual int getDataLength() const {return static_cast<int>(data.length());};
	virtual const byte_t* getDataPtr() const { return data.c_ptr();};

};

class IpmiPayloadDataSolData : public IpmiPayloadData
{
public:
	IpmiPayloadDataSolData();
	IpmiPayloadDataSolData(const ipmi_req_t& req);
 /**
     * Retrieves the completion code.
     * @return Completion code.
     */
    inline byte_t getCompletionCode() const;
    
    /**
     * Retrieves pointer to the IPMI message data buffer.
     * @return Pointer to the IPMI message data buffer.
     */
    inline const byte_t* getDataPtr() const;
    
    /**
     * Retrieves the IPMI message data length.
     * @return The IPMI message data length.
     */
    inline int getDataLength() const;
};

inline
int
IpmiPayloadDataSolData::getDataLength() const
{
    return static_cast<int>(data.length());
}

inline
byte_t
IpmiPayloadDataSolData::getCompletionCode() const
{
	//HACK: avoid adding "payload type = SOL" to ipmi_rsp_t
	//this isnt so bad since SOL packets dont have compCodes
	//jason
    return 0xFF;
}

inline
const byte_t*
IpmiPayloadDataSolData::getDataPtr() const
{
    return data.c_ptr();
}




/**
 * Represents IPMI message (where payload [5-0] is 0). The request data is 
 * serialized along with function's data, responder's data and checksumms.
 */
class IpmiPayloadDataIpmiMsg : public IpmiPayloadData
{
public:
    /**
     * Ctor. Creates empty message buffer.
     */
    IpmiPayloadDataIpmiMsg();

    /**
     * Ctor.
     * @param req Request.
     * @param rqAddr Rq Addr.
     * @param rqSeq Rq Seq.
     * @param rqLun Rq Lun.
     */
    IpmiPayloadDataIpmiMsg(const ipmi_req_t& req, byte_t rqAddr, byte_t rqSeq, byte_t rqLun);

    /**
     * Retrieves the completion code.
     * @return Completion code.
     */
    inline byte_t getCompletionCode() const;
    
    /**
     * Retrieves pointer to the IPMI message data buffer.
     * @return Pointer to the IPMI message data buffer.
     */
    inline const byte_t* getDataPtr() const;
    
    /**
     * Retrieves the IPMI message data length.
     * @return The IPMI message data length.
     */
    inline int getDataLength() const;

    /**
     * Calculates checksum 1.
     * @param req Request.
     */
    static byte_t checksum1(const ipmi_req_t& req);
    
    /**
     * Calculates checksum 2.
     * @param req Request.
     * @param rqAddr Rq Addr.
     * @param rqSeq Rq Seq.
     * @param rqLun Rq Lun.
     */
    static byte_t checksum2(const ipmi_req_t& req, byte_t rqAddr, byte_t rqSeq, byte_t rqLun);
};

inline
byte_t
IpmiPayloadDataIpmiMsg::getCompletionCode() const
{
    // skip reqaddr, reqlun, checksum_1, bmcaddr, lun, command
    return data.at(6);
}

inline
const byte_t*
IpmiPayloadDataIpmiMsg::getDataPtr() const
{
    return data.c_ptr(7);
}

inline
int
IpmiPayloadDataIpmiMsg::getDataLength() const
{
    // 8 = 7 for ipmi additional data + 1 is for completion code
    return static_cast<int>(data.length() - 8);
}

#endif
