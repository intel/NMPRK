#include "stdafx.h"

#include <Windows.h>
#include <atlsafe.h>
#include <vector>
#include <sstream>
#include "imbapi.h"

#include "nmprkTypes.h"
#include "IntelIpmiProxy.h"

#define SI_THIS_MODULE SI_DEBUG_MODULE_IPMI_PROXY

IntelIpmiProxy::IntelIpmiProxy(void)
{
	SI_DEBUG_TRACE(SI_THIS_MODULE, "Initializing IntelIpmiProxy");

	seqNo_ = 0;

	int status = open_imb();

	// Get the starting sequece number
	unsigned char rspData[1024];
	unsigned int rspDataLen = 1024;
	(void)GetAsyncImbpMessage((ImbPacket *)rspData, (DWORD *)&rspDataLen, 1000, &seqNo_, 0xFF);
}


IntelIpmiProxy::~IntelIpmiProxy(void)
{
}

ipmi_rsp_t IntelIpmiProxy::RequestResponse(const ipmi_req_t& req)
{
	SI_DEBUG_TRACE(SI_THIS_MODULE, "IntelImbDriverIpmiProxy::requestResponse() invoked");

	IMBPREQUESTDATA imbreq;
	int status, i;

	imbreq.rsSa	= 0x20;
	imbreq.rsLun	= 0;
	imbreq.busType	= 0;
	imbreq.netFn	= req.netFun;
	imbreq.cmdType	= req.cmd;

	imbreq.dataLength = req.len;
	if(imbreq.dataLength > 0)
		imbreq.data = (unsigned char *)req.data;
	else
		imbreq.data = NULL;

	//printf("IMB rsSa       : %x\n", imbreq.rsSa);
	//printf("IMB netFn      : %x\n", imbreq.netFn);
	//printf("IMB cmdType    : %x\n", imbreq.cmdType);
	//printf("IMB dataLength : %d\n", imbreq.dataLength);

	unsigned char rspData[1024];
	memset(rspData, 0, 1024);
	int rspDataLen = 0;
	unsigned char ccode = 0xFE;

	if(imbreq.cmdType == 0x34)
	{
		rspDataLen = 1024;

		// clear async messages in case other entities are using driver at same time
		while(GetAsyncImbpMessage((ImbPacket *)rspData, (DWORD *)&rspDataLen, 1000, &seqNo_, req.data[0]) == 0)
		{
			rspDataLen = 1024;
		}

		rspDataLen = 0;
	}

	for (i=0; i<1; i++)
	{
		status = SendTimedImbpRequest(&imbreq, 5000,
							rspData, &rspDataLen, &ccode);
		if (status == 0)
			break;

		/* error */
		SI_DEBUG_TRACE(SI_THIS_MODULE, "Retry: Error sending IMB request, status=0x%x, ccode=0x%x", status, (int)ccode);
	}

	if(status != 0)
	{
		throw IpmiProxyException(NMPRK_PROXY_ERROR_XFER, "Error sending IMB request: " + status);
	}

	if(req.netFun == 0x06 && req.cmd == 0x34)
		lastRequestChannel_ = req.data[0];

	ipmi_rsp_t rsp;
	rsp.compCode = ccode;
	rsp.len = rspDataLen;
	memcpy(rsp.data, rspData, rspDataLen);

	return rsp;
}

ipmi_rsp_t IntelIpmiProxy::GetAsyncResponse(int msTimeout)
{
	int status = -1;
	unsigned char rspData[1024];
	memset(rspData, 0, 1024);
	int rspDataLen = 1024;
	unsigned char ccode = 0xFE;

	// TODO: update timeout mechanism
	int count = 0;
	while(count < 1000)
	{
		status = GetAsyncImbpMessage((ImbPacket *)rspData, (DWORD *)&rspDataLen, 1000, &seqNo_, lastRequestChannel_);
		if(status == 0)
			break;

		Sleep(1);
		count++;
	}

	if(status != 0)
	{
		// TODO: throw
	}

	/* workaround: some IMB driver versions do NOT report the BMC's SA in the packet */
	if (rspData[0] != BMC_SA)
	{
		for (int j = rspDataLen; j > 0; j--)
		{
			rspData[j] = rspData[j-1];
		}
		rspDataLen = rspDataLen + 1;
		rspData[0] = BMC_SA;	/* insert the BMC's SA so that the data will look like a real IMB packet */
	}

	ipmi_rsp_t tRsp;
	tRsp.compCode = 0x00;
	tRsp.len = rspDataLen;
	memcpy(tRsp.data, rspData, rspDataLen);

	if(tRsp.compCode != 0x00)
	{
		std::stringstream ss;
		ss << "Response Completion Code was non-zero: " << (int) tRsp.compCode << std::endl;
		throw IpmiProxyException(NMPRK_PROXY_ERROR_INVALID_ASYNC_RSP, ss.str());
	}

	if(tRsp.len < 7)
	{
		std::stringstream ss;
		ss << "Response data length was less than expected: " << (int) tRsp.len << std::endl;
		throw IpmiProxyException(NMPRK_PROXY_ERROR_INVALID_ASYNC_RSP, ss.str());
	}

	ipmi_rsp_t rsp;
	rsp.compCode = tRsp.data[6];
	rsp.len = tRsp.len - 8; // remove headers and the last byte because it is checksum
	if(rsp.len > 0)
		memcpy(rsp.data, &tRsp.data[7], rsp.len);

	return rsp;
}
