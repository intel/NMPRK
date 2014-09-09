#include "stdafx.h"

#include <Windows.h>
#include <atlsafe.h>
#include <stdexcept>
#include <sstream>

#include "nmprkTypes.h"
#include "MicrosoftIpmiProxy.h"

#define SI_THIS_MODULE SI_DEBUG_MODULE_IPMI_PROXY

#define CHECK_HRESULT(hr, funcName) \
   if (FAILED(hr)) \
	  throw IpmiProxyException(NMPRK_PROXY_ERROR, __FUNCTION__": "##funcName)

static struct CoInit
{
    CoInit() { CoInitialize(NULL); }
    ~CoInit() { CoUninitialize(); }
} coInit;

MicrosoftIpmiProxy::MicrosoftIpmiProxy()
{
	InitializeCriticalSectionAndSpinCount(&m_reqMutex, 4000);
	InitializeCriticalSectionAndSpinCount(&m_rspMutex, 4000);

	m_reqEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_rspEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_exitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_ExecutionThreadExited = false;
	m_ExecutionThreadHandle = CreateThread(NULL, 0, ExecutionThread, this, 0, NULL);
}

void MicrosoftIpmiProxy::ExecutionThread()
{
	try
	{
		initialize();

		HANDLE aEvents[] = {m_exitEvent, m_reqEvent};
		int ret = 0;
		while(WaitForMultipleObjects(sizeof aEvents / sizeof (HANDLE), aEvents, 
									 FALSE, INFINITE) != WAIT_OBJECT_0)
		{
			postRsp(RequestResponse_internal(getReq()));
		}
	}
	catch(...)
	{
		m_ExecutionThreadExited = true;
	}

	SI_DEBUG_TRACE(SI_THIS_MODULE, "Microsoft Proxy Execution Thread Stopped");
}

MicrosoftIpmiProxy::~MicrosoftIpmiProxy()
{
	SetEvent(m_exitEvent);

	WaitForSingleObject(m_ExecutionThreadHandle, 1000);
}

void MicrosoftIpmiProxy::initialize()
{
		HRESULT hr = CoInitialize(NULL);
    //CHECK_HRESULT(hr, "CoInitialize");

	// 10/7/2010 - This was removed for use with the NMCT_Server application.  NMCT server also uses the IntelSystemInfoLib which already enables
	// COM security.  We should fix this up in the future so both use COM correctly and can initialize and unitialize COM independently.

    SI_DEBUG_TRACE(SI_THIS_MODULE, "Initializing COM Security");
    hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT,
                              RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    //CHECK_HRESULT(hr, "CoInitializeSecurity");

    CComPtr<IWbemLocator> wbemLocator;
    hr = wbemLocator.CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER);
    CHECK_HRESULT(hr, "CoCreateInstance(CLSID_WbemLocator)");

    // obtain a pointer to the services
    hr = wbemLocator->ConnectServer(L"root\\wmi",
                                    NULL /*Username*/,
                                    NULL /*Password*/,
                                    NULL /*Locale*/,
                                    0 /*Flags*/, 
                                    NULL /*Authority*/, 
                                    NULL /*Context*/,
                                    &wbemServices);
    CHECK_HRESULT(hr, "IWbemLocator::ConnectServer(root\\wmi)");

    wbemLocator.Release();

    // configure the proxy
    hr = CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                           RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 
                           EOAC_NONE);
    CHECK_HRESULT(hr, "CoSetProxyBlanket");

    bstr_t ipmiClassName("Microsoft_IPMI");

    // enumerate the Microsoft_IPMI instances available locally
    // (only one is expected)
    CComPtr<IEnumWbemClassObject> ipmiInstanceEnum;
    hr = wbemServices->CreateInstanceEnum(ipmiClassName, WBEM_FLAG_SHALLOW | WBEM_FLAG_FORWARD_ONLY,
                                          NULL, &ipmiInstanceEnum);
    CHECK_HRESULT(hr, "IWbemServices::CreateInstanceEnum(Microsoft_IPMI)");

	ULONG retObjectCount = 0;
    CComPtr<IWbemClassObject> ipmiInstance;
    hr = ipmiInstanceEnum->Next(WBEM_NO_WAIT, 1, &ipmiInstance, &retObjectCount);
    CHECK_HRESULT(hr, "IEnumWbemClassObject::Next");
    if (1 != retObjectCount)
        throw std::runtime_error("No Microsoft_IPMI object found");

    // get the path of the first object
    CComVariant comVar;
    hr = ipmiInstance->Get(L"__PATH", 0, &comVar, NULL, NULL);
    CHECK_HRESULT(hr, "IWbemClassObject::Get(__PATH)");

    ipmiInstancePath = comVar.bstrVal;
    SI_DEBUG_TRACE(SI_THIS_MODULE, "WMI object instance: %s", ipmiInstancePath);

    comVar.Clear();
    hr = ipmiInstance->Get(L"BMCAddress", 0, &comVar, NULL, NULL);
    CHECK_HRESULT(hr, "IWbemClassObject::Get(BMCAddress");
    bmcAddress = comVar.bVal;
    SI_DEBUG_TRACE(SI_THIS_MODULE, "BMC Address: 0x%x", (int)bmcAddress);

    // cache in-params signature
    CComPtr<IWbemClassObject> ipmiClass;
    hr = wbemServices->GetObject(ipmiClassName, WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, &ipmiClass, NULL);
    CHECK_HRESULT(hr, "IWbemServices::GetObject(Microsoft_IPMI)");
    hr = ipmiClass->GetMethod(L"RequestResponse", 0, &inParamsDef, NULL);
    CHECK_HRESULT(hr, "IWbemClassObject::GetMethod(RequestResponse)");
}

void MicrosoftIpmiProxy::cleanup()
{
}

ipmi_rsp_t MicrosoftIpmiProxy::RequestResponse(const ipmi_req_t& req)
{
	postReq(req);

	if(m_ExecutionThreadExited == false && WaitForSingleObject(m_rspEvent, 5000) == WAIT_OBJECT_0)
	{
		return getRsp();
	}
	else
	{
		std::stringstream ss;
		ss << "Error receiving response. Request - NetFn: " << (int)req.netFun << ", Cmd: " << (int)req.cmd;
		throw IpmiProxyException(NMPRK_PROXY_ERROR, ss.str());
	}
}

ipmi_rsp_t MicrosoftIpmiProxy::RequestResponse_internal(const ipmi_req_t& req)
{
	CComPtr<IWbemClassObject> inParams;
	CComPtr<IWbemClassObject> outParams;

	SI_DEBUG_TRACE(SI_THIS_MODULE, "MicrosoftDriverIpmiProxy::requestResponse() invoked");

	//NOTE: 
	//
	// All MS coding examples use SpawnInstance() but this
	// method looses all qualifiers except CIMTYPE and ID.
	// Clone() copies them all including WmiSizeIs
	// of RequestData, so we do not receive INVALID_PARAMETER.
	//
	// WinRM seems to be affected by this ?bug? as well.
	//
	//HRESULT hr = inParamsDef->SpawnInstance(0, &inParams);
	HRESULT hr = inParamsDef->Clone(&inParams);

	CHECK_HRESULT(hr, "IWbemClassObject::SpawnInstance (inParams)");

	// 1. Translate in-params

	CComVariant arg(static_cast<BYTE>(req.netFun));
	hr = inParams->Put(L"NetworkFunction", 0, &arg, 0);
	CHECK_HRESULT(hr, "Setting NetworkFunction");
	arg.Clear();
	SI_DEBUG_TRACE(SI_THIS_MODULE, "NetworkFunction set to 0x%x", (int)req.netFun);

	arg = static_cast<BYTE>(req.rsLun);
	hr = inParams->Put(L"Lun", 0, &arg, 0);
	CHECK_HRESULT(hr, "Setting Lun");
	arg.Clear();
	SI_DEBUG_TRACE(SI_THIS_MODULE, "Lun set to 0x%x", (int)req.rsLun);

	arg = static_cast<BYTE>(req.cmd);
	hr = inParams->Put(L"Command", 0, &arg, 0);
	CHECK_HRESULT(hr, "Setting Command");
	arg.Clear();
	SI_DEBUG_TRACE(SI_THIS_MODULE, "Command set to 0x%x", (int)req.cmd);

	arg = bmcAddress;
	hr = inParams->Put(L"ResponderAddress", 0, &arg, 0);
	CHECK_HRESULT(hr, "Setting ResponderAddress");
	arg.Clear();
	SI_DEBUG_TRACE(SI_THIS_MODULE, "ResponderAddress set to 0x%x", (int)bmcAddress);

	CComSafeArray<BYTE> argArray;
	arg = static_cast<int>(req.len);
	hr = inParams->Put(L"RequestDataSize", 0, &arg, 0);
	CHECK_HRESULT(hr, "Setting RequestDataSize");
	arg.Clear();
	SI_DEBUG_TRACE(SI_THIS_MODULE, "RequestDataSize set to 0x%x", req.len);

	if (req.len > 0)
	{
		hr = argArray.Add(static_cast<ULONG>(req.len), req.data);
		CHECK_HRESULT(hr, "Creating RequestData array");
	}
	else
	{
		hr = argArray.Create();
		CHECK_HRESULT(hr, "Creating empty RequestData array");
	}
	arg = argArray;
	argArray.Destroy();

	hr = inParams->Put(L"RequestData", 0, &arg, 0);
	CHECK_HRESULT(hr, "Setting RequestData");
	arg.Clear();

	// 2. Execute method

	hr = wbemServices->ExecMethod(ipmiInstancePath, L"RequestResponse", 0, NULL,inParams, &outParams, NULL);
	CHECK_HRESULT(hr, "IWbemServices::ExecMethod");

	// 3. Translate out-params

	hr = outParams->Get(L"CompletionCode", 0, &arg, NULL, NULL);
	CHECK_HRESULT(hr, "Getting CompletionCode");
	unsigned char completionCode = arg.bVal;
	arg.Clear();

	hr = outParams->Get(L"ResponseData", 0, &arg, NULL, NULL);
	CHECK_HRESULT(hr, "Getting ResponseData");
	argArray.Attach(static_cast<SAFEARRAY*>(arg.byref));

	ipmi_rsp_t rsp;
	rsp.compCode = completionCode;
	rsp.len = argArray.GetCount()-1;
	if(rsp.len > 0)
		memcpy(rsp.data, reinterpret_cast<BYTE*>(argArray.m_psa->pvData)+1, argArray.GetCount()-1); // strip completion code

	argArray.Detach();

	return rsp;
}

ipmi_rsp_t MicrosoftIpmiProxy::GetAsyncResponse(int msTimeout)
{
	int sTimeout = msTimeout / 1000;

	ipmi_req_t req;
	req.netFun = 0x06;
	req.cmd = 0x33;
	req.rsAddr = bmcAddress;
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
