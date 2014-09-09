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

#include "RmcppIpmiSession.h"
#include "RmcppIpmiRequest.h"
#include "RmcppIpmiHeader.h"
#include "RmcppIpmiPayload.h"
#include "RmcppIpmiResponse.h"
#include "RmcppIpmiError.h"
#include "CryptoProxy.h"
#include "DebugLog.h"

#include <sstream>
#include <iomanip>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

#define SI_THIS_MODULE SI_DEBUG_MODULE_RMCPP

struct hmac_properties
{
    CryptoProxy::HmacAlgId algId;
    unsigned int blockSize;
    unsigned int hashSize;
    unsigned int integSize;
    const char* name; // for logging purposes
};

// The next array is constructed in the way
// to be selectable by value of ipmi_auth_algo.
static hmac_properties HMAC_PROPERTIES[] = {
    {(CryptoProxy::HmacAlgId)-1, 0, 0, 0},  //this entry corresponds to AUTH_ALGO_RAKP_NONE and should not be used
    {CryptoProxy::HMAC_SHA1, CryptoProxy::SHA1_BLOCKSIZE, CryptoProxy::SHA1_HASHSIZE, 12, "SHA1"},
    {CryptoProxy::HMAC_MD5, CryptoProxy::MD5_BLOCKSIZE, CryptoProxy::MD5_HASHSIZE, 16, "MD5"},
};

struct AutoCloseSock
{
    AutoCloseSock(SOCKET s = INVALID_SOCKET);
    ~AutoCloseSock();
    SOCKET s;
};

AutoCloseSock::AutoCloseSock(SOCKET s)
             :s(s)
{
}

AutoCloseSock::~AutoCloseSock()
{
    if (INVALID_SOCKET != s)
    {
        closesocket(s);
    }
}

IpmiSession::IpmiSession(const char* targetIp, const char* userName, const char* password)
           :targetIp(targetIp), 
            uNameM(reinterpret_cast<const byte_t*>(userName), static_cast<int>(strlen(userName))), 
            kUid(reinterpret_cast<const byte_t*>(password), static_cast<int>(strlen(password))),
            maxPrivLevel(0x00)
{

	session_send_sol = false;	
    memset(managedSequence, 0x00, sizeof(managedSequence));
    memset(sidc, 0x00, sizeof(sidc));
	session_socket = INVALID_SOCKET;
}

IpmiSession::~IpmiSession()
{
}

void
IpmiSession::ping(const char* targetIp, short int targetPort)
{
    SI_DEBUG_TRACE(SI_THIS_MODULE, "ASF Ping %s", targetIp);

    AutoCloseSock rmcpSocket(setupSocket(targetIp, targetPort));

    //raw RMCPP ping header & etc
    byte_t pingData[] = { 0x06, 0x00, 0xFF, 0x06, 0xBE, 0x11, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00 };

    send(rmcpSocket.s, reinterpret_cast<const char*>(pingData), sizeof(pingData), 0);
	waitForSocket(rmcpSocket.s, 5);
    if (SOCKET_ERROR == shutdown(rmcpSocket.s, SD_SEND))
        throw OsSockError("shutdown");
    byte_t buffer[MAX_IPMI_SIZE + 10];
    int packetSize = recv(rmcpSocket.s, reinterpret_cast<char *>(buffer), sizeof(buffer), 0);
    if (packetSize == 0)
    {
        throw runtime_error("Connection unexpectedly closed");
    }
    if (packetSize < 0)
    {
        throw OsSockError("recv");
    }
    SI_DEBUG_TRACE(SI_THIS_MODULE, "Got ASF Pong");
}

void
IpmiSession::sendRecv(const ByteVector& in, ByteVector& out)
{
    send(session_socket, reinterpret_cast<const char*>(in.c_ptr()),
         static_cast<int>(in.size()), 0);


    // DONT shutdown the connection (UDP)

    out.resize(MAX_IPMI_SIZE + 10);

    waitForSocket(session_socket, 15);

    int packetSize = recv(session_socket, reinterpret_cast<char*>(&out.front()),
                          static_cast<int>(out.size()), 0);
    if (packetSize == 0)
    {
        throw runtime_error("Connection unexpectedly closed");
    }
    if (packetSize < 0)
    {
        throw OsSockError("recv");
    }
    out.resize(packetSize);
}

void
IpmiSession::sendRecv(timeval timeoutVal, bool bSend, 
					 bool bRecv, bool& timeout, 
					 ByteVector& in, ByteVector& out)
{
	fd_set sockets;
    int retCode;

	if(bSend)
	{
	    send(session_socket, reinterpret_cast<const char*>(in.c_ptr()),
		     static_cast<int>(in.size()), 0);
	}
	if ((bSend && bRecv) || bRecv)
	{
		timeout = false;
		out.resize(MAX_IPMI_SIZE + 10);
		FD_ZERO(&sockets);
		FD_SET(session_socket,&sockets); 
		while (true)
		{
			retCode = select((int)session_socket+1,&sockets,NULL,NULL,&timeoutVal);
			if (retCode == 0)
			{
				timeout = true;
				return;
			//timeout
			}
			else if (retCode == -1)
			{
#ifdef WIN32
				if(WSAGetLastError() == WSAEINTR)
#else
                                if(errno == EINTR)
#endif
				{
					continue;
				}
				throw OsSockError("select error");
			}
		    out.resize(MAX_IPMI_SIZE + 10);
			int packetSize = recv(session_socket, reinterpret_cast<char*>(&out.front()),
							  static_cast<int>(out.size()), 0);
			if (packetSize == 0)
			{
				throw runtime_error("Connection unexpectedly closed");
			}
			if (packetSize < 0)
			{
				throw OsSockError("recv");
			}
			//LOG_LODEBUG << "Received packet from socket" << endl;
			out.resize(packetSize);
			break;
		}
	}
}

void
IpmiSession::sendRecv(timeval timeoutVal, bool bSend, 
					 bool bRecv, bool& timeout, 
					 const ipmi_req_t& req, ipmi_rsp_t& rsp, bool inSession)
{
    ByteVector serializedReq;
    ByteVector serializedRsp;
	memset(&rsp,0, sizeof(ipmi_rsp_t));
	if(bSend)
	{
	    serializeRequest(serializedReq, req, inSession);
	}

	sendRecv(timeoutVal, bSend, bRecv, timeout, serializedReq, serializedRsp);
	if(bRecv & !timeout)
	{
		deserializeResponse(rsp, serializedRsp);
	}
}

void
IpmiSession::sendRecv(const ipmi_req_t& req, ipmi_rsp_t& rsp, bool inSession)
{
    ByteVector serializedReq;

	///IPMI/command
    serializeRequest(serializedReq, req, inSession);
	//request is now bytes

	ByteVector serializedRsp;
    sendRecv(serializedReq, serializedRsp);
	
    deserializeResponse(rsp, serializedRsp);

	//response is now IPMI/compcode
}

SOCKET
IpmiSession::setupSocket(const char* targetIp, short int targetPort)
{
    // make a local copy of the connection information
	SOCKET fdSocket = socket(AF_INET, SOCK_DGRAM, 0);

	
//non-blocking socket
///*
#ifdef	WIN32
	u_long iMode = 1;
	ioctlsocket(fdSocket, FIONBIO, &iMode);
#else
	fcntl(fdSocket, F_SETFL, O_NONBLOCK);
#endif //
//*/
    if (INVALID_SOCKET == fdSocket)
        throw OsSockError("socket");

    size_t sockAddrSize = sizeof(struct sockaddr_in);
    struct sockaddr_in serverAddr;

    memset((char *) &serverAddr, 0, sockAddrSize);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(targetPort);

    // MOST of this next section was copied from the MSDN sample Simple.C
    struct hostent* pHostEntry;

    /* server address is a name */
    pHostEntry = gethostbyname(targetIp);
    if (pHostEntry == NULL) // Unable to create data properly
        throw OsSockError("gethostbyname");

    // 
    // Copy the resolved information into the sockaddr_in structure 
    // 
    memcpy(&(serverAddr.sin_addr), pHostEntry->h_addr, pHostEntry->h_length);
    serverAddr.sin_family = pHostEntry->h_addrtype;

    if (INADDR_NONE == serverAddr.sin_addr.s_addr)
        throw runtime_error("setupSocket(): Got INADDR_NONE");

    int iReturn =:: connect(fdSocket,(struct sockaddr *) &serverAddr,(int) sockAddrSize);

    if (SOCKET_ERROR == iReturn)
    throw OsSockError("connect");

    return fdSocket;
}

void
IpmiSession::incr4LSB(Sequence& seq)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        seq[i]++;
        //make sure it didnt overflow
        if (seq[i] != 0x00)
            break;
        //otherwise, carry
    }
}


int
IpmiSession::waitForSocket(SOCKET sock, int seconds)
{
	//waitforsocket - poor mans (threadless) timeout

	//todo: remove int seconds: set timeout (and default) somewhere else
	timeval timeout;
	timeout.tv_sec = seconds;
	timeout.tv_usec = 0;
	fd_set sockets;
	FD_ZERO(&sockets);
	FD_SET(sock,&sockets); //ignore C4127 caused by FD_SET 
	return select((int)sock+1,&sockets,NULL,NULL,&timeout);

}

RmcppIpmiSession::RmcppIpmiSession(const char* targetIp, const char* userName, const char* password,
                                   ipmi_auth_algo authAlgo,
                                   ipmi_integ_algo integAlgo,
                                   ipmi_confid_algo confidAlgo)
                :IpmiSession(targetIp, userName, password), authAlgo(authAlgo), integAlgo(integAlgo),
                 confidAlgo(confidAlgo), useKg(false)
{
	
	session_send_sol = false;
    *reinterpret_cast<unsigned int*>(sidm) = 0x85be2329;
}

void RmcppIpmiSession::getDcmiCapInfo(ipmi_rsp_t& rsp)
{
	//LOG_SUMMARY<<"Issuing GetDCMICapabilityInfo Command with session-less";

	// turn off confid and integ for this first message
    ipmi_confid_algo userConfidAlgo = confidAlgo;
    ipmi_integ_algo userIntegAlgo = integAlgo;
	try
	{
		confidAlgo = CONFID_ALGO_NONE;
        integAlgo = INTEG_ALGO_NONE;
		sendRecv(IpmiGetDcmiCapInfo(), rsp, false);
		if(rsp.compCode == 0)
		{
			//LOG_SUMMARY<<"GetDCMICapabilityInfo Command Executed Successfully for session-less privilege level."; 
			//LOG_LODEBUG<<"\n\tDCMI Specification conformance Major version is:\t0x"<< hex<<setw(2)<<setfill('0')<<int(rsp.data[1]);
			//LOG_LODEBUG<<"\n\tDCMI Specification conformance Minor version is:\t0x"<<hex<<uppercase<<int(rsp.data[2]);
			//LOG_LODEBUG<<"\n\tDCMI Specification Parameter Revision is       :\t0x"<<hex<<uppercase<<setfill('0')<<setw(2)<<int(rsp.data[3]);
			//LOG_LODEBUG<<"\n\tDCMI Specification Parameter Revision is       :\t0x"<<hex<<uppercase<<setfill('0')<<setw(2)<<int(rsp.data[4]);
			//LOG_LODEBUG<<"\n\tDCMI Specification Parameter Revision is       :\t0x"<<hex<<uppercase<<setfill('0')<<setw(2)<<int(rsp.data[5]);
			//LOG_LODEBUG<<"\n\tDCMI Specification Parameter Revision is       :\t0x"<<hex<<uppercase<<setfill('0')<<setw(2)<<int(rsp.data[6]);
		}
		else
		{
			//LOG_ERR<<"GetDCMICapabilityInfo command failed. Session-less might not be supported.";
		}
	}
	catch (exception&)
	{
		confidAlgo = userConfidAlgo;
        integAlgo = userIntegAlgo;
		//LOG_ERR<<"Did not receive response for GetDCMICapabilityInfo Command.";
		//LOG_ERR<<"Session-less might not be supported.";
		//throw;
	}
	confidAlgo = userConfidAlgo;
    integAlgo = userIntegAlgo;
}

void
RmcppIpmiSession::getChannelAuthCap(ipmi_rsp_t& rsp)
{
    SI_DEBUG_TRACE(SI_THIS_MODULE, "Get Channel Auth Cap (RMCP+)...");
	
    // turn off confid and integ for this first message
    ipmi_confid_algo userConfidAlgo = confidAlgo;
    ipmi_integ_algo userIntegAlgo = integAlgo;
    try
    {
        confidAlgo = CONFID_ALGO_NONE;
        integAlgo = INTEG_ALGO_NONE;
        sendRecv(IpmiGetChannelAuthCap(), rsp, false);
		
    }
    catch (exception&)
    {
        confidAlgo = userConfidAlgo;
        integAlgo = userIntegAlgo;
        throw;
    }
    confidAlgo = userConfidAlgo;
    integAlgo = userIntegAlgo;
}

void
RmcppIpmiSession::openSession()
{
    SI_DEBUG_TRACE(SI_THIS_MODULE, "  Open Session");

    //Build Open Session Request
    const byte_t messageTag = 0x42;

    ByteVector rsp;
    sendRecvInt(PAYLOAD_RMCPP_OPEN_SES_REQ, 
                IpmiOpenSession (messageTag, maxPrivLevel, sidm, authAlgo, integAlgo, confidAlgo),
                PAYLOAD_RMCPP_OPEN_SES_RSP,
                rsp);

    const rmcpp_open_session_response& osRsp =
        reinterpret_cast<const rmcpp_open_session_response&>(rsp.front());

    if (osRsp.messageTag != messageTag)
    {
        stringstream s;
        s << "Message Tag does not match " << hex << setw(2) << setfill('0')
            << (int)osRsp.messageTag;
        throw runtime_error(s.str());
    }

    if (osRsp.statusCode != 0x00)
    {
        throw RmcppStatusCodeError("Open Session", osRsp.statusCode);
    }
    maxPrivLevel = osRsp.maxPrivLevel;
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Max Priv Level: 0x%x", (int)maxPrivLevel);

    //SI_DEBUG_TRACE(SI_THIS_MODULE, "    Sent Console ID:     %s", sidm);
    //SI_DEBUG_TRACE(SI_THIS_MODULE, "    Returned Console ID: %s", osRsp.sidm);
    if (memcmp(sidm, osRsp.sidm, sizeof(SessionId)) != 0)
    {
        throw runtime_error("Console ID does not match");
    }
    SI_DEBUG_TRACE(SI_THIS_MODULE, "      Matches");

    //cout << endl << "Managed System Session ID \t" << endl;
    memcpy(sidc, osRsp.sidc, sizeof(SessionId));//Managed System Session ID 
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Management System Session ID: %s", sidc);

    //Authentication Payload
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Authentication: ", (ipmi_auth_algo)osRsp.authPayload.algorithm);
    //Integrity Payload
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Integrity: ", (ipmi_integ_algo)osRsp.integPayload.algorithm);
    //Confidentiality Payload
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Confidentiality: ", (ipmi_confid_algo)osRsp.confidPayload.algorithm);
}

void
RmcppIpmiSession::rakp12(ByteVector& randC, ByteVector& randM, Guid& guidC, byte_t& roleM, ByteVector& sik)
{
    SI_DEBUG_TRACE(SI_THIS_MODULE, "  RAKP 1-2");

    const byte_t messageTag = 0x43;

    // generate random RM
    //randM.copy(4, 16);//"4" chosen from fair dice roll, guaranteed to be random
    CryptoProxy::getInstance()->genRand(16, randM);

    ByteVector rsp;
    sendRecvInt(PAYLOAD_RMCPP_RAKP1, IpmiRakp1 (messageTag, sidc, randM, roleM, uNameM),
                PAYLOAD_RMCPP_RAKP2, rsp);

    const rmcpp_rakp2& rakp2 = reinterpret_cast<const rmcpp_rakp2&>(rsp.front());
    if (rakp2.messageTag != messageTag)
    {
        throw runtime_error("Message Tag does not match");
    }
    if (rakp2.statusCode != 0x00)
    {
        throw RmcppStatusCodeError("Rakp12", rakp2.statusCode);
    }

    //compare saved BMC ID to one in RAKP2 packet.
    if (memcmp(rakp2.sidm, sidm, sizeof(SessionId)) != 0)
    {
        stringstream s;
        s << "Returned Console ID " << rakp2.sidm << " does not match sent " << sidm;
        throw runtime_error(s.str());
    }

    //store Managed System Random Number (for RAKP 3-4)
    randC.copy(rakp2.randC, 16);

    //store Managed System GUID (for RAKP 3-4)
    memcpy(guidC, rakp2.guidC, sizeof(Guid));

    // hashData(SIDM, SIDC, RM, RC, GUIDC, RoleM, ULengthM, < UNameM >)
    ByteVector hashData;

    hashData.append(sidm, sizeof(SessionId));
    hashData.append(sidc, sizeof(SessionId));
    hashData += randM;
    hashData += randC;
    hashData.append(rakp2.guidC, sizeof(Guid));
    hashData += roleM;
    hashData += static_cast<byte_t>(uNameM.length());
    hashData += uNameM;

    // Concatenate data for generating the SIK.
    ByteVector sikData;

    sikData += randM;
    sikData += randC;
    sikData += roleM;
    sikData += static_cast<byte_t>(uNameM.length());
    sikData += uNameM;

    ByteVector digest;
    
    switch (authAlgo)
    {
    case AUTH_ALGO_RAKP_NONE:
        //len - byteCount = 0
        break;

    case AUTH_ALGO_RAKP_HMAC_SHA1:
    case AUTH_ALGO_RAKP_HMAC_MD5:
        if (rsp.size() - sizeof(rmcpp_rakp2) != HMAC_PROPERTIES[authAlgo].hashSize)
            throw runtime_error(string("wrong packet length for ") + HMAC_PROPERTIES[authAlgo].name);
        // NOTE: SIK should be generated by using 160-bit KG (no trunc).
        // if the BMC's LAN channel is configured to use NULL KG,
        // kUid is used as the KG.
        //
        // useKg was set by response from getChannelAuthCap
        if (useKg)
        {
            SI_DEBUG_TRACE(SI_THIS_MODULE, "    SIK generated using KG");
            CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[authAlgo].algId, sikData, kG, digest);
        }
        else
        {
            SI_DEBUG_TRACE(SI_THIS_MODULE, "    SIK generated using KUID");
            CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[authAlgo].algId, sikData, kUid, digest);
        }
        sik = digest;

        CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[authAlgo].algId, hashData, kUid, digest);
        break;
    default:
        throw runtime_error("Uknown Authentication code");
    }

    // verify the received Key Exchange Authentication Code
    if (memcmp(digest.c_ptr(), rakp2.keyExAuthCode, digest.size()) != 0)
    {
        throw runtime_error("RAKP12: managedDigest != local digest");
    }
    SI_DEBUG_TRACE(SI_THIS_MODULE, "      Matches Mng Digest");

    // calculate K1 from SIK if necessary
    if (INTEG_ALGO_HMAC_SHA1_96 == integAlgo || INTEG_ALGO_HMAC_MD5_128 == integAlgo)
    {
        ByteVector const1;
        //NOTE: Spec says "HMAC block size" - seems that it means "hash size"
        //const1.append(0x01, HMAC_PROPERTIES[authAlgo].blockSize);
        //Current DCTS implementation uses 20 byte constant 
        const1.append(0x01, 20);
        CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[authAlgo].algId, const1, sik, k1);

/* The following is not per spec 
        Do not use K1 as kuid 
        LOG_LODEBUG << "    K1: " << k1;
        //FIXME: open ipmi uses kUid as the k1 for HMAC_MD5_128 - check with others what is correct
        
        if (INTEG_ALGO_HMAC_MD5_128 == integAlgo)
        {
            k1 = kUid;
            k1.append(0x00, 16 - kUid.length());
        }
*/


    }

    // calculate K2 from SIK if necessary
    if (CONFID_ALGO_AES_CBC_128 == confidAlgo)
    {
        ByteVector const2;
        switch (authAlgo)
        {
        case AUTH_ALGO_RAKP_HMAC_SHA1:
        case AUTH_ALGO_RAKP_HMAC_MD5:
            //NOTE: Spec says "HMAC block size" - seems that it means "hash size"
            //const2.append(0x02, HMAC_PROPERTIES[authAlgo].blockSize);
            //DCTS uses 20 byte constant
            const2.append(0x02, 20);
            CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[authAlgo].algId, const2, sik, k2);
            // K2 is always 128-bit for AES (so need to truncate SHA-1 160-bit)
            k2.resize(16);
            break;

        default:
            throw runtime_error("HMAC not configured for AES CBC 128");
        }
    }
}

void
RmcppIpmiSession::rakp34(const ByteVector& randC, const ByteVector& randM, const Guid& guidC, byte_t roleM, const ByteVector& sik)
{
    byte_t messageTag = 0x44;
    ByteVector digest;

    SI_DEBUG_TRACE(SI_THIS_MODULE, "  RAKP 3-4");

    // hashData(RC,SIDM, RoleM, ULengthM, < UNameM >)
    ByteVector hashData;
    hashData += randC;
    hashData.append(sidm, sizeof(SessionId));
    hashData += roleM;
    hashData += static_cast<byte_t>(uNameM.length());
    hashData += uNameM;

    //Key Exchange Authentication Code 9:N (RAKP-NONE = 0 bytes)
    switch (authAlgo)
    {
    case AUTH_ALGO_RAKP_HMAC_SHA1:
    case AUTH_ALGO_RAKP_HMAC_MD5:
        CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[authAlgo].algId, hashData, kUid, digest);
        break;

    case AUTH_ALGO_RAKP_NONE:
    default:
        //keySize = 0;
        break;
    }

    ByteVector rsp;
    sendRecvInt(PAYLOAD_RMCPP_RAKP3, IpmiRakp3(messageTag, sidc, digest),
                PAYLOAD_RMCPP_RAKP4, rsp);

    const rmcpp_rakp4& rakp4 = reinterpret_cast<const rmcpp_rakp4&>(rsp.front());

    if (rakp4.messageTag != messageTag)
    {
        throw runtime_error("Message Tag does not match");
    }
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    RAKP4: Message Tag OK");

    if (rakp4.statusCode != 0x00)
    {
        throw RmcppStatusCodeError("RAKP34", rakp4.statusCode);
    }
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    RAKP4: Status Code OK");

    // compare managed id
    if (memcmp(sidm, rakp4.sidm, sizeof(SessionId)) != 0)
    {
        stringstream s;
        s << "Returned Console ID " << rakp4.sidm << " does not match sent " << sidm;
        throw runtime_error(s.str());
    }
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    RAKP4: Managed ID OK");
    // check integCheckValue
    if (AUTH_ALGO_RAKP_NONE != authAlgo)
    {
        ByteVector hashData;
        hashData += randM;
        hashData.append(sidc, sizeof(SessionId));
        hashData.append(guidC, sizeof(Guid));

        ByteVector digest;
        CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[authAlgo].algId, hashData, sik, digest);
        int digestSize = 0;
        if (AUTH_ALGO_RAKP_HMAC_SHA1 == authAlgo)
        {
            digestSize = 12;
        }
        else if (AUTH_ALGO_RAKP_HMAC_MD5 == authAlgo)
        {
            digestSize = 16;
        }
        else
        {
            assert(0);
        }
        if (memcmp(digest.c_ptr(), rakp4.integCheckValue, digestSize) != 0)
		{
			//LOG_LODEBUG << "RAKP34: managed Digest != local Digest";
            throw runtime_error("RAKP34: managed Digest != local Digest");

		}
        SI_DEBUG_TRACE(SI_THIS_MODULE, "    RAKP4: %d -bytes digest OK", digestSize);
    }
}

void
RmcppIpmiSession::open()
{	
    ipmi_rsp_t rsp;

	SI_DEBUG_TRACE(SI_THIS_MODULE, "Opening session... [authAlgo: %d, integAlgo: %d, confidAlgo: %d]", authAlgo, integAlgo, confidAlgo);
	session_socket = setupSocket(targetIp.c_str(), TARGET_PORT);

    getChannelAuthCap(rsp);

    const get_channel_auth_cap_response& gcacRsp = 
        reinterpret_cast<const get_channel_auth_cap_response&>(*rsp.data);

    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Channel number: 0x%x", (int)gcacRsp.channelNumber);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Authentication Type Support: 0x%x", (int)gcacRsp.authTypeSupport.value);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Authentication status: 0x%x", (int)gcacRsp.authStatus.value);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "    Extended Capabilieties: 0x%x", (int)gcacRsp.extendedCaps.value);

    // check returned Authentication Capabilities with requested interfaceFormat
    if (gcacRsp.authTypeSupport.b.ipmi2Sup == 0 || gcacRsp.extendedCaps.b.ipmi20Supported == 0)
        throw runtime_error("RMCPP: IPMI 2.0 not supported by the target");
    if (gcacRsp.authStatus.b.kgSetToNonZero == 1 && kG.empty())
        throw runtime_error("RMCPP: KG is required but not configured for transport");
    useKg = gcacRsp.authStatus.b.kgSetToNonZero == 1;

    openSession();

    ByteVector randC;
    ByteVector randM;
    Guid guidC;
    ByteVector sik;
    byte_t roleM = 0x04;
    rakp12(randC, randM, guidC, roleM, sik);
    rakp34(randC, randM, guidC, roleM, sik);
    SI_DEBUG_TRACE(SI_THIS_MODULE, "Session established");
    incr4LSB(managedSequence);
	
	
	//Verify Integrity support in Cipher by sending the following command
	//Set the Session Privilege Level (after session is established)
    ipmi_req_t reqData;
    ipmi_rsp_t rspData;

	reqData.rsAddr = 0x20;
	reqData.netFun = 0x06; //application
    reqData.rsLun = 0x00;
    reqData.cmd = 0x3B;//SetSessionPrivilegeLevel
    reqData.data[0] = 0x04; //max requested level = 4, Administrator 
    reqData.len = 1;

	sendRecv(reqData, rspData, true);//true = in session

}

void
RmcppIpmiSession::close()
{
    SI_DEBUG_TRACE(SI_THIS_MODULE, "%s", __FUNCTION__);

    ipmi_rsp_t rsp;
    sendRecv(IpmiDisconnectReq(sidc), rsp, true);

	// shutdown the connection since no more data will be sent
    int iResult = shutdown(session_socket, SD_SEND);
    if (iResult == SOCKET_ERROR)
        throw OsSockError("shutdown");

}

void
RmcppIpmiSession::serializeRequest(ByteVector& out, const ipmi_req_t& req, bool inSession)
{
    // Header
    ipmi_payload_t payloadType;
    
	//jason
	if (this->session_send_sol)
	{
		payloadType.value = PAYLOAD_SOL;
	} else {
		payloadType.value = PAYLOAD_IPMI_MESSAGE;
	}


	
    if (confidAlgo != CONFID_ALGO_NONE)
    {
        payloadType.b.encrypted = 1;
    }
    if (integAlgo != INTEG_ALGO_NONE)
    {
        payloadType.b.authenticated = 1;
    }

    if (inSession)
    {
        RmcppSessionHeader(payloadType, sidc, managedSequence).serialize(out);
        incr4LSB(managedSequence);
    }
    else
    {
        RmcppSessionHeader(payloadType).serialize(out);
    }

	// Payload
	IpmiPayloadData *pPayload;
	IpmiPayloadDataSolData solPayload(req);
	IpmiPayloadDataIpmiMsg ipmiPayload(req, RQ_ADDR, RQ_SEQ, RQ_LUN);
//	IpmiPayloadDataIpmiMsg payload(req, RQ_ADDR, RQ_SEQ, RQ_LUN);
	if (this->session_send_sol)
	{
	
		//this is not IpmiPayloadDataIpmiMsg, 'serialize' only data
		pPayload = &solPayload;
	} else {
		pPayload = &ipmiPayload;
	}

    //   encryption
    switch (confidAlgo)
    {
    case CONFID_ALGO_NONE:
        IpmiPayload::serializeRmcpp(out, *pPayload);
        break;

    case CONFID_ALGO_AES_CBC_128:
        IpmiPayload::serializeEncryptedRmcpp(out, k2, *pPayload);
        break;

    default:
        throw logic_error("RMCPP: Unsupported Confid Algo");
    }
    //   integrity
    if (INTEG_ALGO_NONE != integAlgo)
    {
        serializeRmcppIntegrity(out);
    }
}

void
RmcppIpmiSession::deserializeResponse(ipmi_rsp_t& rsp, const ByteVector& in)
{
    size_t pos = 0;

    // Header
    RmcppSessionHeader header;
    header.deserialize(header, in, pos);
    if (header.authType != AUTH_TYPE_RMCPP)
        throw runtime_error("RMCPP: incorrect auth type in response header");
    
    //   integrity check
    if (header.payload.b.authenticated == 1)
    {
        if (INTEG_ALGO_NONE == integAlgo)
            throw runtime_error("RMCPP: got authenticated payload while Integ Algo is None");
        verifyRmcppIntegrity(in);
    }

	// Payload
	IpmiPayloadData *pPayload;
	IpmiPayloadDataSolData solPayload;
	IpmiPayloadDataIpmiMsg ipmiPayload;
	//MUX payload here
	if (header.payload.b.type == PAYLOAD_SOL)
	{
		pPayload = &solPayload;
	}
	else
	{
		pPayload = &ipmiPayload;
	}
    //   encrypted
    if (header.payload.b.encrypted == 1)
    {
        switch (confidAlgo)
        {
        case CONFID_ALGO_AES_CBC_128:
            IpmiPayload::deserializeEncryptedRmcpp(pPayload->data, k2, in, pos);
            break;

        default:
            throw runtime_error("RMCPP: got encrypted payload while Config Algo is None");
        }
    }
    //   plain
    else
    {
        IpmiPayload::deserializeRmcpp(pPayload->data, in, pos);
    }

    rsp.compCode = pPayload->getCompletionCode();

	 rsp.len = pPayload->getDataLength();
	if (rsp.len > 0)
	{
		memcpy(rsp.data, pPayload->getDataPtr(), rsp.len);
	}

}

void
RmcppIpmiSession::serializeIntRequest(ByteVector& out, ipmi_payload payloadType, const ipmi_req_t& req)
{
    ipmi_payload_t payload;
    payload.value = static_cast<byte_t>(payloadType); //overwrite value to clear b.encrypted and b.authenticated
    RmcppSessionHeader(payload).serialize(out);
    
    IpmiPayloadData pd;
    pd.data.copy(req.data, (int) req.len);
    IpmiPayload::serializeRmcpp(out, pd);
}

void
RmcppIpmiSession::deserializeIntResponse(ByteVector& rsp, ipmi_payload expectedPayloadType,
                                         const ByteVector& in)
{
    size_t pos = 0;
    RmcppSessionHeader header;
    header.deserialize(header, in, pos);
    if (header.authType != AUTH_TYPE_RMCPP || header.payload.b.type != expectedPayloadType)
        throw runtime_error("RMCPP: Unexpected response");
    
    IpmiPayload::deserializeRmcpp(rsp, in, pos);
}

void
RmcppIpmiSession::sendRecvInt(ipmi_payload reqPayloadType, const ipmi_req_t& req, 
                              ipmi_payload rspPayloadType, ByteVector& rsp)
{
    ByteVector reqRaw;
    serializeIntRequest(reqRaw, reqPayloadType, req);
    ByteVector rspRaw;
    sendRecv(reqRaw, rspRaw);
    deserializeIntResponse(rsp, rspPayloadType, rspRaw);
}

void
RmcppIpmiSession::serializeRmcppIntegrity(ByteVector& out)
{
    // add integrity PAD (can use out.length() as
    byte_t padLength = (4- static_cast<byte_t>((out.length() + 2) % 4)) % 4;
    out.append(0xff, padLength);
    out += padLength;
    out += 0x07; // next header;

    ByteVector digest;
    // need to set size separately as not entire digest is copied in some cases
    int digestSize;

    if (INTEG_ALGO_MD5_128 == integAlgo)
    {
        // AuthCode = MD5(password + AuthType/Format + ... + Next_Header + password)
        ByteVector authData;
        
        //use full 20-bytes password
        byte_t pw[20];
        memset(pw, 0, 20);
        kUid.copyto(pw, kUid.length());
        authData.append(pw, 20);
        authData.append(out.c_ptr(4), out.length() - 4);
        authData.append(pw, 20);
        CryptoProxy::getInstance()->md5(authData, digest);
        digestSize = 16;
    }
    else
    {
        // compute hmac
        ByteVector signedData(out.c_ptr(4), out.length() - 4); //start from IPMI Session Header
        CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[integAlgo].algId,
                                         signedData, k1, digest);
        digestSize = HMAC_PROPERTIES[integAlgo].integSize;
    }
    out.append(digest, digestSize);
}

void
RmcppIpmiSession::verifyRmcppIntegrity(const ByteVector& in)
{
    ByteVector digest;

    if (INTEG_ALGO_MD5_128 == integAlgo)
    {
		const int IPMI_SESSION_HEADER_SIZE = 10;
		unsigned int signedDataLength = IPMI_SESSION_HEADER_SIZE + 2 /*payload size field*/ + 2 /*trailing pad length field and 07*/;
		signedDataLength += (in[IPMI_SESSION_HEADER_SIZE+4] + (in[IPMI_SESSION_HEADER_SIZE+5] << 8));
		signedDataLength = (signedDataLength + 3) & 0xfc;
		ByteVector signedData(in.c_ptr(4), signedDataLength);

        ByteVector authData;
        byte_t pw[20];
        memset(pw, 0, 20);
        kUid.copyto(pw, kUid.length());
        authData.append(pw, 20);
        authData += signedData;
        authData.append(pw, 20);
        CryptoProxy::getInstance()->md5(authData, digest);
        if (memcmp(digest.c_ptr(), in.c_ptr(4 + signedDataLength), 16) != 0)
            throw runtime_error("RMCPP: Incorrect MD5 sum on incoming message");
        SI_DEBUG_TRACE(SI_THIS_MODULE, "MD5 sum correct on incoming message");
    }
    else
    {
		// Software Forge: CJP
		// 6/26/12: Revampend this code based on IPMITool code and
		// a server that failed connect based on the original algorithm here

		const int IPMI_LANPLUS_OFFSET_AUTHTYPE = 4;
		const int IPMI_SHA1_AUTHCODE_SIZE = 12;
		unsigned int signedDataLength = in.length() - IPMI_LANPLUS_OFFSET_AUTHTYPE -IPMI_SHA1_AUTHCODE_SIZE;
		ByteVector signedData(in.c_ptr(4), signedDataLength);

		// calculate hmac and compare
		CryptoProxy::getInstance()->hmac(HMAC_PROPERTIES[integAlgo].algId, signedData, k1, digest);

        // check that there is enough data for hmac following calculated signedData range
        if (4 + signedDataLength + HMAC_PROPERTIES[integAlgo].integSize != in.size())
        {
            stringstream s;
			s << "Integrity Padding mismatch in packet. Length received - " << in.length();
			s << " (expected " << 4 + signedDataLength + HMAC_PROPERTIES[integAlgo].integSize << ")";
            throw runtime_error(s.str());
        }

        if (memcmp(digest.c_ptr(), in.c_ptr(4 + signedDataLength), HMAC_PROPERTIES[integAlgo].integSize) != 0)
		{
            //LOG_LODEBUG << "RMCPP: Incorrect hmac on incoming message";
			throw runtime_error("RMCPP: Incorrect hmac on incoming message");
		} else {
			//SI_DEBUG_TRACE(SI_THIS_MODULE, "Hmac on incoming message correct");
		}
    }
}
