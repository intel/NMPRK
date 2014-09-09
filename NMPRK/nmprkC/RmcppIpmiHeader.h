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

#ifndef _RMCPP_IPMI_REQUEST_HEADER_H
#define _RMCPP_IPMI_REQUEST_HEADER_H

#include "RmcppTypes.h"
#include "ByteVector.h"

/**
 * Base struct for more specialized session headers
 * that serializes RMCP header and Authentication Type
 * as both are common for all the types of packets.
 *
 * The idea is to have light-weight structures that are
 * rather allocated by the caller on the stack, so no
 * excessive memory operations on the heap are performed.
 */
struct IpmiSessionHeader
{
    /**
     * Default Ctor. Initializes the <code>authType</code>
     * to <code>AUTH_TYPE_NONE</code>.
     */
    IpmiSessionHeader();
    
    /**
     * Ctor.
     * @param authType Authentication type.
     */
    explicit IpmiSessionHeader(ipmi_auth_type authType);

    /**
     * Serializes the header.
     * @param out Output buffer.
     */
    void serialize(ByteVector& out) const;
    
    /**
     * Deserializes the header.
     * @param header Receives the deserialized data.
     * @param in Input buffer.
     * @param pos Current position in the <code>in</code>. Upon input, the <code>pos</code>
     *      indicates the position that the serialization should start on.
     *      Upon return, indicates the position next to the last byte consumed
     *      by the deserializer.
     */
    static void deserialize(IpmiSessionHeader& header, const ByteVector& in, size_t& pos);

    /// Authentication type.
    ipmi_auth_type authType;
};


/**
 * Header for RMCPP (IPMI 2.0) packets.
 * Base IpmiSessionHeader is always initialized with 
 * <code>AUTH_TYPE_RMCPP</code>.
 */
struct RmcppSessionHeader : IpmiSessionHeader
{
    /**
     * Default ctor.
     */
    RmcppSessionHeader();

    /**
     * Ctor for requests sent outside any session.
     * Both <code>sidc</code> and <code>managedSequence</code>
     * are initialized to zeros.
     * @param payload Payload type.
     */
    explicit RmcppSessionHeader(const ipmi_payload_t& payload);

    /**
     * Ctor for request sent within an established session.
     * @param payload Payload type.
     * @param sidc Session Id.
     * @param managedSequence Sequence number.
     */
    RmcppSessionHeader(const ipmi_payload_t& payload, const SessionId& sidc, 
                       const Sequence& managedSequence);

    /**
     * Serializes the header.
     * @param out Output buffer.
     */
    void serialize(ByteVector& out) const;

    /**
     * Deserializes the header.
     * @param header Receives the deserialized data.
     * @param in Input buffer.
     * @param pos Current position in the <code>in</code>. Upon input, the <code>pos</code>
     *      indicates the position that the serialization should start on.
     *      Upon return, indicates the position next to the last byte consumed
     *      by the deserializer.
     */
    static void deserialize(RmcppSessionHeader& header, const ByteVector& in, size_t& pos);

    /// Payload type.
    ipmi_payload_t payload;
    
    /// Session Id.
    SessionId sidc;

    /// Sequence Number.
    Sequence managedSequence;
};

#endif 
