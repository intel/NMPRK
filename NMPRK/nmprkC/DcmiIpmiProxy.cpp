#include "stdafx.h"

#include <Windows.h>
#include <atlsafe.h>
#include <vector>
#include <sstream>
#include "imbapi.h"

#include "nmprkTypes.h"
#include "DcmiIpmiProxy.h"

#define SI_THIS_MODULE SI_DEBUG_MODULE_IPMI_PROXY

//
// Driver IOCTLs
//
#define FILE_DEVICE_DCMI		0x00008010
#define IOCTL_DCMI_BASE			0x00000880

#define IOCTL_DCMI_SEND_GET_MESSAGE		CTL_CODE(FILE_DEVICE_DCMI, IOCTL_DCMI_BASE + 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

DcmiIpmiProxy::DcmiIpmiProxy(void)
{
	SI_DEBUG_TRACE(SI_THIS_MODULE, "Initializing DcmiIpmiProxy");

	SI_DEBUG_TRACE(SI_THIS_MODULE, "Opening DCMI Device");
	// Open the device
	std::wstring DrvName(L"\\\\.\\DCMI");
	h_ipmiDrv = CreateFile(DrvName.c_str(),  // Name of the "device" to open
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if(h_ipmiDrv == INVALID_HANDLE_VALUE)
		throw IpmiProxyException(NMPRK_INVALID_HANDLE, "CreateFile(\\\\.\\DCMI,..) returned INVALID_HANDLE_VALUE");
}


DcmiIpmiProxy::~DcmiIpmiProxy(void)
{
	if (h_ipmiDrv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(h_ipmiDrv);
	}
}

ipmi_rsp_t DcmiIpmiProxy::RequestResponse(const ipmi_req_t& req)
{
	SI_DEBUG_TRACE(SI_THIS_MODULE, "DcmiIpmiProxy::requestResponse() invoked");

	BYTE requestData[1024]  = {0}; //what if total dcmi struct + red.data.size is bigger that this 1024 buffer

	ImbRequestBuffer  *reqBuff  = reinterpret_cast<ImbRequestBuffer *>(requestData);

	reqBuff->req.netFn = req.netFun;
	reqBuff->req.cmd   = req.cmd;
	reqBuff->req.rsLun = req.rsLun;
	reqBuff->req.rsSa  = 0x00;//Dont care
	reqBuff->req.dataLength = static_cast<BYTE>(req.len);

#ifdef _MSC_VER
	memcpy(reqBuff->req.data, req.data, req.len);
	//std::copy(req.data.begin(), req.getData().end(), stdext::checked_array_iterator<byte_t*>(reqBuff->req.data, sizeof(requestData)-sizeof(ImbRequestBuffer)));
#else
	//std::copy(req.getData().begin(), req.getData().end(), reqBuff->req.data);
#endif
	memcpy(reqBuff->req.data, req.data, req.len);

	reqBuff->timeOut = 10 * 1000;
	reqBuff->flags = 0;

	BYTE responseData[1024] = {0};

	ImbResponseBuffer *respBuff = reinterpret_cast<ImbResponseBuffer *>(responseData);
	DWORD respLength = sizeof( responseData );

	BOOL ret = DeviceIoControl(h_ipmiDrv,
				(DWORD)IOCTL_DCMI_SEND_GET_MESSAGE,
				reqBuff,      // Ptr to InBuffer
				sizeof(requestData),
				respBuff,      // Ptr to OutBuffer
				sizeof(responseData),// Length of OutBuffer
				&respLength, // BytesReturned
				0);

	if(ret == FALSE)
		throw IpmiProxyException(NMPRK_INVALID_HANDLE, "DeviceIoControl returned INVALID_HANDLE_VALUE");

	ipmi_rsp_t rsp;
	rsp.compCode = respBuff->cCode;
	rsp.len = respLength;
	if(respLength >= 1024)
	{
		std::stringstream ss;
		ss << "Unexpectedly large data length: " << respLength << ". This sometimes means another application is using the driver";
		throw IpmiProxyException(NMPRK_INVALID_DATA_LENGTH, ss.str());
	}

	if(respLength > 0)
		memcpy(&(rsp.data), &(respBuff->data), respLength);

	return rsp;
}

ipmi_rsp_t DcmiIpmiProxy::GetAsyncResponse(int msTimeout)
{
	int sTimeout = msTimeout / 1000;

	ipmi_req_t req;
	req.netFun = 0x06;
	req.cmd = 0x33;
	req.rsAddr = 0x20;
	req.rsLun = 0x00;
	req.len = 0;

	time_t tStart;
	time(&tStart);

	while(true)
	{
		ipmi_rsp_t tRsp = RequestResponse(req);
		if(tRsp.compCode == 0x80)
		{
			time_t tNow;
			time(&tNow);
			if (tNow - tStart >= (time_t) sTimeout)
				throw IpmiProxyException(NMPRK_PROXY_ERROR_NO_RSP, "No Asynchronous Response received");

			Sleep(1);
			continue;
		}	

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
		rsp.len = tRsp.len - 7 - 1; // minus 7 for header, minus 1 for checksum at end
		if(rsp.len > 0)
			memcpy(rsp.data, &tRsp.data[7], rsp.len);
		return rsp;
	}
}
