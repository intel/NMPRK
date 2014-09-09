#include "stdafx.h"

#include "transportInterfaceSlim.h"
#include "DebugLog.h"
#include <sstream>
#include <iomanip>

#define SI_THIS_MODULE SI_DEBUG_MODULE_INTERFACE

transportInterfaceSlim::transportInterfaceSlim()
{
	handleAllocator = 1;
}

nmprk_status_t transportInterfaceSlim::ConnectLocal(
	OUT nmprk_conn_handle_t *h)
{
	if(h == NULL)
		return NMPRK_INVALID_ARGUMENT;

	if(getProxyByHandle(KCS_DEVICE_HANDLE) == NULL)
	{
		IpmiProxy *p;
		std::string errorMsg;
		if(IpmiProxy::GetKcsIpmiProxyInstance(&p, errorMsg) != true)
		{
			lastErrorDetails = errorMsg;
			return NMPRK_FAILURE;
		}

		proxyMap[KCS_DEVICE_HANDLE] = p;
	}

	*h = KCS_DEVICE_HANDLE;
	return NMPRK_SUCCESS;
}

nmprk_status_t transportInterfaceSlim::ConnectRemote(
	IN nmprk_connect_remote_parameters_t *input, 
	OUT nmprk_conn_handle_t *h)
{
	nmprk_status_t status;
	RmcppIpmiProxy *p = new RmcppIpmiProxy();

	// Added the do/while loop because sometimes certain Intel servers return a wrong message to the connect request.
	// If we just try again the connection is successful.
	int tryAgain = 0;
	do
	{
		try
		{
			// TODO: fix for cipher suite
			p->Connect(input->ipOrHostname, input->username, input->password, AUTH_ALGO_RAKP_HMAC_SHA1, INTEG_ALGO_HMAC_SHA1_96, CONFID_ALGO_AES_CBC_128);
			*h = handleAllocator++;
			proxyMap[*h] = p;
			status = NMPRK_SUCCESS;
			break;
		}
		catch(std::runtime_error& e)
		{
			const char *msg = e.what();
			if(strcmp(msg, "RMCPP: Unexpected response") == 0)
				tryAgain++;
			else
			{
				// TODO: add last error message
				status = NMPRK_PROXY_ERROR;
				break;
			}
		}
	}
	while(tryAgain < 3);

	return status;
}

nmprk_status_t transportInterfaceSlim::Disconnect(
	IN nmprk_conn_handle_t h)
{
	if(h == KCS_DEVICE_HANDLE)
		return NMPRK_INVALID_HANDLE;

	if(proxyMap.find(h) == proxyMap.end())
		return NMPRK_INVALID_HANDLE;

	IpmiProxy *p = proxyMap[h];
	if(p == NULL)
		return NMPRK_INVALID_HANDLE;

	proxyMap.erase(h);
	((RmcppIpmiProxy *)p)->Disconnect();
	delete p;

	return NMPRK_SUCCESS;
}

nmprk_status_t transportInterfaceSlim::SetDefaultNmCommandBridging(
	IN nmprk_conn_handle_t h,
	IN byte_t channel,
	IN byte_t address)
{
	IpmiProxy *p = getProxyByHandle(h);
	if(p == NULL)
		return NMPRK_INVALID_HANDLE;

	nm_bridging_info_t bi;
	bi.channel = channel;
	bi.address = address;
	handleBridgingMap[h] = bi;

	return NMPRK_SUCCESS;
}

void PrintRequestToLog(ipmi_req_t *req)
{
	SI_DEBUG_TRACE(SI_THIS_MODULE, "Request:");
	SI_DEBUG_TRACE(SI_THIS_MODULE, "  NetFn: 0x%x, Cmd: 0x%x, RsAddr: 0x%x, RsLun: 0x%x", req->netFun, req->cmd, req->rsAddr, req->rsLun);

	std::stringstream ss;
	for(int i = 0; i < req->len; i++)
	{
		if(i > 0)
			ss << " ";
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)req->data[i];
	}
	SI_DEBUG_TRACE(SI_THIS_MODULE, "  Data: %s", ss.str().c_str());
	/*int label = 0x00;
	for(int i = 0; i < req->len; i++)
	{
		if(i > 0 && i % 8 == 0)
		{
			SI_DEBUG_TRACE(SI_THIS_MODULE, "    %02x: %s", label, ss.str().c_str());
			ss.str("");
			label += 8;
		}
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)req->data[i] << " ";
	}
	if(ss.str().length() > 0)
		SI_DEBUG_TRACE(SI_THIS_MODULE, "    %02x: %s", label, ss.str().c_str());*/
}

void PrintResponseToLog(ipmi_rsp_t *rsp)
{
	SI_DEBUG_TRACE(SI_THIS_MODULE, "Response:");
	SI_DEBUG_TRACE(SI_THIS_MODULE, "  CompCode: 0x%x", rsp->compCode);

	std::stringstream ss;
	for(int i = 0; i < rsp->len; i++)
	{
		if(i > 0)
			ss << " ";
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)rsp->data[i];
	}
	SI_DEBUG_TRACE(SI_THIS_MODULE, "  Data: %s", ss.str().c_str());
}

nmprk_status_t transportInterfaceSlim::SendCommandInternal(
	IN nmprk_conn_handle_t h, 
	IN bool bridgeIfSet, 
	IN ipmi_req_t *reqA, 
	OUT ipmi_rsp_t *rspA)
{
	int status = NMPRK_FAILURE;

	if(bridgeIfSet == true && handleBridgingMap.find(h) != handleBridgingMap.end())
	{
		nm_bridging_info_t bi = handleBridgingMap[h];
		status = SendBridgedCommand(h, bi.channel, bi.address, reqA, rspA);
	}
	else
	{		
		IpmiProxy *p = getProxyByHandle(h);
		if(p == NULL)
			return NMPRK_INVALID_HANDLE;

		AutoMutex lock(sendRecvLock);

		try
		{
			PrintRequestToLog(reqA);
			saveLastReq(reqA);
			*rspA = p->RequestResponse(*reqA);
			saveLastRsp(rspA);
			PrintResponseToLog(rspA);
			status = rspA->compCode;
		}
		catch(exception& ex)
		{
			SI_DEBUG_ERROR(SI_THIS_MODULE, "Exception: %s", ex.what());
			lastErrorDetails = ex.what();
			status = NMPRK_FAILURE;
		}
		catch(...)
		{
			SI_DEBUG_ERROR(SI_THIS_MODULE, "Exception: Unknown");
			lastErrorDetails = "Unknown";
			status = NMPRK_FAILURE;
		}
	}

	return status;
}

void transportInterfaceSlim::saveLastReq(ipmi_req_t *req)
{
	int count = 0;

	lastReq.data[count++] = req->netFun;
	lastReq.data[count++] = req->cmd;
	for(int i = 0; i < req->len; i++)
		lastReq.data[count++] = req->data[i];
	lastReq.count = count;
}

void transportInterfaceSlim::saveLastRsp(ipmi_rsp_t *rsp)
{
	int count = 0;

	lastRsp.data[count++] = rsp->compCode;
	for(int i = 0; i < rsp->len; i++)
		lastRsp.data[count++] = rsp->data[i];
	lastRsp.count = count;
}

nmprk_status_t transportInterfaceSlim::SendBridgedCommand(
	IN nmprk_conn_handle_t h, 
	IN byte_t channel, 
	IN byte_t address, 
	IN ipmi_req_t *req, 
	OUT ipmi_rsp_t *rsp)
{
	int status = NMPRK_FAILURE;
	ipmi_req_t bReq;
	byte_t chk1 = 0, chk2 = 0;
	byte_t sequenceNumber_ = 0;
	int byteIdx = 0;

	IpmiProxy *p = getProxyByHandle(h);
	if(p == NULL)
		return NMPRK_INVALID_HANDLE;

	bReq.netFun = 0x06;
	bReq.cmd = 0x34;
	bReq.rsAddr = 0x20;
	bReq.rsLun = 0x00;
	bReq.len = 0;

	// The tracking bit used to be set to 1 all the time, but Dell didn't like it so it was changed to 0.  Now only set it when requested (non-KCS)
    // Byte 0 - 0000 0110 (00 = track request, 0 = encryption, 0 = authentication, 0110 = channel)
	if (h == KCS_DEVICE_HANDLE)
		bReq.data[bReq.len++] = (0x00 | (channel & 0xF));
	else
		bReq.data[bReq.len++] = (0x40 | (channel & 0xF));

    // Byte 1 - Target Address
	byte_t hdr1 = address;
    chk1 += hdr1 % 256;
    bReq.data[bReq.len++] = hdr1;

    // Byte 2 - 1011 1000 (101110 = NetFn, 00 = Target LUN)
	byte_t hdr2 = ((req->netFun << 2) & 0xFC) | (req->rsLun & 0x03);
    chk1 += hdr2 % 256;
	bReq.data[bReq.len++] = hdr2;

    // Byte 3 - Checksum
	bReq.data[bReq.len++] = -chk1;

    // Byte 4 - Source Address
    bReq.data[bReq.len++] = 0x20;
	chk2 += bReq.data[bReq.len-1] % 256;

    // increment sequence number
    sequenceNumber_ = (sequenceNumber_ + 1) % 0x3F;

    // Byte 5 - XXXX XX00 (XXXXXX = Seq Number, 10 = Source LUN)
	// The Source LUN used to always be set to 0x02, but SuperMicro had a problem with this over the lan
	// Now we set it to 0x00 when using a lan connection
	if (h == KCS_DEVICE_HANDLE)
		bReq.data[bReq.len++] = ((sequenceNumber_ << 2) & 0xFC) | 0x02;
	else
		bReq.data[bReq.len++] = ((sequenceNumber_ << 2) & 0xFC) | 0x00;
	chk2 += bReq.data[bReq.len-1] % 256;

    // Byte 6 - Command
    bReq.data[bReq.len++] = req->cmd;
	chk2 += bReq.data[bReq.len-1] % 256;

	for(size_t i = 0; i < req->len; i++)
	{
		bReq.data[bReq.len++] = req->data[i];
		chk2 += bReq.data[bReq.len-1] % 256;
	}

	chk2 = -chk2;
	bReq.data[bReq.len++] = chk2;

	AutoMutex lock(sendRecvLock);

	try
	{
		PrintRequestToLog(&bReq);
		saveLastReq(req);
		ipmi_rsp_t bRsp = p->RequestResponse(bReq);
		saveLastRsp(&bRsp);
		PrintResponseToLog(&bRsp);
		if(bRsp.compCode != 0x00)
		{
			status = bRsp.compCode;
		}
		else
		{
			*rsp = p->GetAsyncResponse(5000);
			saveLastRsp(rsp);
			PrintResponseToLog(rsp);
			status = rsp->compCode;
		}
	}
	catch(exception& ex)
	{
		SI_DEBUG_ERROR(SI_THIS_MODULE, "Exception: %s", ex.what());
		lastErrorDetails = ex.what();
		status = NMPRK_FAILURE;
	}
	catch(...)
	{
		SI_DEBUG_ERROR(SI_THIS_MODULE, "Exception: Unknown");
		lastErrorDetails = "Unknown";
		status = NMPRK_FAILURE;
	}

	return status;
}

nmprk_status_t transportInterfaceSlim::GetLastRequestResponse(
	IN nmprk_conn_handle_t h, 
	OUT ipmi_capture_req_t *req, 
	OUT ipmi_capture_rsp_t *rsp)
{
	memcpy(req, &lastReq, sizeof(ipmi_capture_req_t));
	memcpy(rsp, &lastRsp, sizeof(ipmi_capture_rsp_t));
	return 0;
}

nmprk_status_t transportInterfaceSlim::GetErrorString(
	IN nmprk_status_t code, 
	IN OUT char *str, IN int strsize)
{
	if(str == NULL || strsize <= 0)
		return NMPRK_INVALID_ARGUMENT;

	strncpy(str, lastErrorDetails.c_str(), strsize-1);
	return NMPRK_SUCCESS;
}

IpmiProxy *transportInterfaceSlim::getProxyByHandle(nmprk_conn_handle_t h)
{
	AutoMutex lock(proxyMapLock);

	return proxyMap[h];
}
