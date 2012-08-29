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


#ifndef _RMCPP_IPMI_SESSION_H
#define _RMCPP_IPMI_SESSION_H

#include <cstring>
#include "transport.h"
#include "ByteVector.h"
#include "OsSpecificSockets.h"
#include "RmcppTypes.h"
#include <stdexcept>

namespace RmcppTransportPlugin
{

/**
 * Base abstract class for IPMI session.
 */
class IpmiSession
{
public:
    /**
     * Ctor.
     * @param targetIp Target IP.
     * @param userName Username.
     * @param password Password.
     */
    IpmiSession(const char* targetIp, const char* userName, const char* password);
    
    /**
     * Dtor.
     */
    virtual ~IpmiSession();

    /**
     * Sets the target IP.
     * @param targetIp Target IP.
     */
    inline void setTargetIp(const char* targetIp);
    
    /**
     * Sets the username.
     * @param userName Username.
     */
    inline void setUsername(const char* userName);
    
    /**
     * Sets the password.
     * @param password Password.
     */
    inline void setPassword(const char* password);

    /**
     * Pings the specified target with ASF presence ping.
     * @param targetIp Target IP.
     * @param targetPort Target port.
     */
    static void ping(const char* targetIp, short int targetPort);

    /**
     * Sends the request and wait for a response. This method uses
     * virtual <code>serializeRequest()</code> to format raw data
     * to be sent to the network socket and <code>deserializeResponse()</code>
     * to decode raw data received from the network socket in response.
     *
     * @param req Request data to be sent.
     * @param rsp Receives a response data.
     * @param inSession Indicates whether the request is session message
     *      (session id and sequence number is non-null) or non-session message.
     */
    void sendRecv(const dcmi_req_t& req, dcmi_rsp_t& rsp, bool inSession = true);

	void sendRecv(timeval timeoutVal, bool bSend, 
					 bool bRecv, bool& timeout, 
					 const dcmi_req_t& req, dcmi_rsp_t& rsp, bool inSession = true);

    /**
     * Establishes a session with the target machine.
     */
    virtual void open() = 0;

    /**
     * Closes session.
     */
    virtual void close() = 0;

    /// Default target port
    static const short int TARGET_PORT = 623;

	bool session_send_sol;

protected:
    /**
     * Establishes connection with the target machine.
     * @param targetIp Target IP.
     * @param tartegPort Target port.
     */
    static SOCKET setupSocket(const char* targetIp, short int targetPort);
    
    /**
     * Version of the <code>sendRecv()</code> for internal requests.
     * @param in Request data in raw format.
     * @param out Receives response data in raw format.
     */
    void sendRecv(const ByteVector& in, ByteVector& out);

	void sendRecv(timeval timeoutVal, bool bSend, 
					 bool bRecv, bool &timeout, 
					 ByteVector& in, ByteVector& out);    

    /**
     * Increments sequence number by one.
     * @param seq Sequence number to increase.
     */
    static void incr4LSB(Sequence& seq);

    /**
     * Waits for a socket to be readable.
     * @param sock Socket to listen to.
     * @param seconds Seconds to wait before timing out.
     */
	static int waitForSocket(SOCKET sock, int seconds);


    /// Req Addr
    static const byte_t RQ_ADDR = 0xC2;
    /// Req Seq
    static const byte_t RQ_SEQ = 0x00;
    /// Req Lun
    static const byte_t RQ_LUN = 0x00;

    /// Target IP
    std::string targetIp;

    /// Username
    ByteVector uNameM;

    /// Password;
    ByteVector kUid;

    /// Session ID
    SessionId sidc;

    /// Console sequence
    Sequence managedSequence;

    /// Max Priv Level
    byte_t maxPrivLevel;


	SOCKET session_socket;


    //socket timeout limit
    //timeval timeout;

private:
    /**
     * Serializes request to raw data buffer.
     * @param out Output buffer that receives raw data.
     * @param req Request to serialize.
     * @param inSession Indicates whether this is session message or
     *      non-session message.
     */
    virtual void serializeRequest(ByteVector& out, const dcmi_req_t& req, bool inSession) = 0;
    
    /**
     * Deserializes response from raw data buffer.
     * @param rsp Receives deserialized response data.
     * @param in Input buffer that contains response raw data response.
     */
    virtual void deserializeResponse(dcmi_rsp_t& rsp, const ByteVector& in) = 0;
};

inline
void
IpmiSession::setTargetIp(const char* targetIp)
{
    this->targetIp = targetIp;
}

inline
void
IpmiSession::setUsername(const char* userName)
{
    uNameM.copy(reinterpret_cast<const byte_t*>(userName), static_cast<int>(strlen(userName)));
}

inline
void
IpmiSession::setPassword(const char* password)
{
    kUid.copy(reinterpret_cast<const byte_t*>(password), static_cast<int>(strlen(password)));
}


/**
 * Implementation of IPMI session for RMCPP (IPMI 2.0).
 */
class RmcppIpmiSession : public IpmiSession
{
public:
    /**
     * Ctor.
     * @param targetIp Target IP.
     * @param userName Username.
     * @param password Password.
     * @param authAlgo Authentication algorithm.
     * @param integAlgo Integrity algorithm.
     * @param confidAlgo Confidentionality algorithm.
     * @param sol Indicates whether Serial Over Lan should be used
     *      (currently not supported).
     */
    RmcppIpmiSession(const char* targetIp, const char* userName, const char* password,
                     ipmi_auth_algo authAlgo,
                     ipmi_integ_algo integAlgo,
                     ipmi_confid_algo confidAlgo);

    /**
     * Establishes a session with the target machine using RAKP.
     */
    void open();

    /**
     * Closes session.
     */
    void close();

    /**
     * Sets the KG (BMC Key).
     * @param kG KG.
     */
    inline void setKg(const ByteVector& kG);

	ipmi_auth_algo getAuthAlgo() { return authAlgo; }
	void setAuthAlgo(ipmi_auth_algo a) { authAlgo = a; }

	ipmi_integ_algo getIntegAlgo() { return integAlgo; }
	void setIntegAlgo(ipmi_integ_algo a) { integAlgo = a; }

	ipmi_confid_algo getConfidAlgo() { return confidAlgo; }
	void setConfidAlgo(ipmi_confid_algo a) { confidAlgo = a; }
    
private:
    /**
     * Sends Get Channel Auth Cap non-session request to the target machine.
     * @param rsp Receives response.
     */
    void getChannelAuthCap(dcmi_rsp_t& rsp);
    
    /**
     * Sends Get DCMI Capability Info non-session request to the target machine.
     * @param rsp Receives response.
     */
    void getDcmiCapInfo(dcmi_rsp_t& rsp);
    
	/**
     * Sends Open Session request, receives and interprets the response.
     * If status code is non-zero then <code>RmcppStatusCodeError</code>
     * is thrown.
     * If received Console Id is different than the sent one,
     * <code>std::runtime_error</code> is thrown.
     */
    void openSession();
    
    /**
     * Sends RAKP-1 request, receives and interprets the RAKP-2 response.
     * If status code is non-zero then <code>RmcppStatusCodeError</code>
     * is thrown.
     * If received Console Id is different than the sent one,
     * <code>std::runtime_error</code> is thrown.
     *
     * @param randC Receives locally generated random number.
     * @param randM Receives random number generated by the target machine.
     * @param guidC Receives GUID of the target machine.
     * @param roleM Receives the Role.
     * @param sik Receives the SIK generated locally.
     */
    void rakp12(ByteVector& randC, ByteVector& randM, Guid& guidC, byte_t& roleM, ByteVector& sik);

    /**
     * Sends RAKP-3 request, receives and interprets the RAKP-4 response.
     * If status code is non-zero then <code>RmcppStatusCodeError</code>
     * is thrown.
     * If received Console Id is different than the sent one,
     * <code>std::runtime_error</code> is thrown.
     *
     * @param randC Locally generated random number.
     * @param randM Random number generated by the target machine.
     * @param guidC GUID of the target machine.
     * @param roleM The Role.
     * @param sik The SIK generated locally.
     */
    void rakp34(const ByteVector& randC, const ByteVector& randM, const Guid& guidC, byte_t roleM, const ByteVector& sik);

    /**
     * Serializes request to raw data buffer in RMCP+ format.
     * @param out Output buffer that receives raw data.
     * @param req Request to serialize.
     * @param inSession Indicates whether this is session message or
     *      non-session message.
     */
    virtual void serializeRequest(ByteVector& out, const dcmi_req_t& req, bool inSession);

    /**
     * Deserializes response from raw data buffer that is expected to be in RMCP+ format.
     * @param rsp Receives deserialized response data.
     * @param in Input buffer that contains response raw data response.
     */
    virtual void deserializeResponse(dcmi_rsp_t& rsp, const ByteVector& in);

    /**
     * Serializes internal request (no encryption, no authentication).
     * @param out Output buffer that receives raw data.
     * @param payloadType Payload type.
     * @param req Request to serialize.
     */
    void serializeIntRequest(ByteVector& out, ipmi_payload payloadType, const dcmi_req_t& req);

    /**
     * Deserializes response to internal request. If the deserialized paylaod
     * type does not match <code>expectedPayloadType</code> then
     * <code>std::runtime_error</code> is thrown.
     * @param rsp Receives deserialized response data.
     * @param expectedPayloadType Expected payload type.
     * @param in Input buffer that contains response raw data response.
     */
    void deserializeIntResponse(ByteVector& rsp, ipmi_payload expectedPayloadType,
                                const ByteVector& in);
    /**
     * Sends internal request, receives and deserializes response.
     * @param reqPayloadType Type of the request payload.
     * @param req Request to be serialized and sent.
     * @param rspPayloadType Expected type of the response payload.
     * @param rsp Receives the deserialized response.
     */
    void sendRecvInt(ipmi_payload reqPayloadType, const dcmi_req_t& req, 
                     ipmi_payload rspPayloadType, ByteVector& rsp);

    /**
     * Calculates and serializes integrity data.
     * @param out Upon input, contains the message (header + payload) data. 
     *      Upon return, contains both integrity data and message data.
     */
    void serializeRmcppIntegrity(ByteVector& out);

    /**
     * Verifies the integrity of the incoming message.
     * @param in Contains data to be verified (both integrity data
     *      and message data as received from the target machine).
     */
    void verifyRmcppIntegrity(const ByteVector& in);

    /// Authentication algorithm.
    ipmi_auth_algo authAlgo;
    
    /// Integrity algorithm.
    ipmi_integ_algo integAlgo;

    /// Confidentiality algorithm.
    ipmi_confid_algo confidAlgo; 

    /// Session Id.
    SessionId sidm;
    
    /// K1
    ByteVector k1;

    /// K2
    ByteVector k2;

    /// KG (BMC Key)
    ByteVector kG;
   
    /**
     * Indicates whether KG should be used instead of KUID to generate SIK.
     * Set by the response to get channel auth cap.
     */
    bool useKg;
};

inline
void RmcppIpmiSession::setKg(const ByteVector& kG)
{
    if (kG.length() != 20)
        throw std::logic_error("RMCPP: Incorrect KG length (expected 20 bytes)");
    this->kG = kG;
}


};

#endif

