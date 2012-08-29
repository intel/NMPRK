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


#include "IpmiProxy.h"
#include "WinExcept.h"
#include "RemoteLogger.h"
#include "OsSpecificSockets.h"
#include "KcsIpmiRequest.h"
#include "KcsIpmiResponse.h"
#include "WinDeviceFunctions.h"

#include <Windows.h>
#include <atlsafe.h>

#include <iostream>

using namespace std;

bool IpmiProxy::GetIpmiProxyInstance(IpmiProxy **proxy, std::string& errorMessage)
{
	bool msDriverInstalled = false, imbDriverInstalled = false;
	bool msDriverEnabled = false, imbDriverEnabled = false;

	try
	{
		*proxy = NULL;
		errorMessage = "No Error";

		const std::wstring msDevName = L"Microsoft Generic IPMI Compliant Device";
		const std::wstring imbDevName = L"Intel Intelligent Management Bus Driver";

		msDriverInstalled = IsDeviceInstalled(msDevName);
		LOG_LODEBUG << "msDriverInstalled: " << msDriverInstalled;
		imbDriverInstalled = IsDeviceInstalled(imbDevName);
		LOG_LODEBUG << "imbDriverInstalled: " << imbDriverInstalled;

		if(msDriverInstalled == false && imbDriverInstalled == false)
			throw IpmiProxyException("Neither the MS Driver or IMB Driver is installed!");

		msDriverEnabled = IsDeviceEnabled(msDevName);
		LOG_LODEBUG << "msDriverEnabled: " << msDriverEnabled;
		imbDriverEnabled = IsDeviceEnabled(imbDevName);
		LOG_LODEBUG << "imbDriverEnabled: " << imbDriverEnabled;

		if(msDriverEnabled == true && imbDriverEnabled == true)
		{
			LOG_LODEBUG << "both drivers enabled!";
			// disable one
			LOG_LODEBUG << "attempt to disable MS driver!";
			ChangeDeviceState(msDevName, false);
			msDriverEnabled = IsDeviceEnabled(msDevName);
			LOG_LODEBUG << "msDriverEnabled: " << msDriverEnabled;
			if(msDriverEnabled != false)
			{
				LOG_LODEBUG << "attempt to disable IMB driver!";
				ChangeDeviceState(imbDevName, false);
				imbDriverEnabled = IsDeviceEnabled(imbDevName);
				LOG_LODEBUG << "imbDriverEnabled: " << imbDriverEnabled;
				if(imbDriverEnabled != false)
				{
					throw IpmiProxyException("Could not disable either the MS Driver or IMB Driver!");
				}
			}
		}
		else if(msDriverEnabled == false && imbDriverEnabled == false)
		{
			LOG_LODEBUG << "both drivers disabled!";

			// enable one
			if(imbDriverInstalled == true)
			{
				LOG_LODEBUG << "attempt to enable IMB driver!";
				ChangeDeviceState(imbDevName, true);
				imbDriverEnabled = IsDeviceEnabled(imbDevName);
				LOG_LODEBUG << "imbDriverEnabled: " << imbDriverEnabled;
			}
		
			if(imbDriverEnabled == false && msDriverInstalled == true)
			{
				LOG_LODEBUG << "attempt to enable MS driver!";
				ChangeDeviceState(msDevName, true);
				msDriverEnabled = IsDeviceEnabled(msDevName);
				LOG_LODEBUG << "msDriverEnabled: " << msDriverEnabled;
			}
		}

		if(imbDriverEnabled == true)
			*proxy = new IntelImbDriverIpmiProxy;
		else if(msDriverEnabled == true)
			*proxy = new MicrosoftDriverIpmiProxy;
		else
			throw IpmiProxyException("Could not enable either the MS Driver or IMB Driver!");

		return true;
	}
	catch(std::exception &ex)
	{
		errorMessage = ex.what();
		if(imbDriverEnabled == true)
			*proxy = new IntelImbDriverIpmiProxy;
		else if(msDriverEnabled == true)
			*proxy = new MicrosoftDriverIpmiProxy;

		return false;
	}

	return false;
}


#define CHECK_HRESULT(hr, funcName) \
   if (FAILED(hr)) \
      throw WinApiError(__FUNCTION__": "##funcName, hr)

static struct CoInit
{
    CoInit() { CoInitialize(NULL); }
    ~CoInit() { CoUninitialize(); }
} coInit;

MicrosoftDriverIpmiProxy::MicrosoftDriverIpmiProxy()
{
    LOG_HIDEBUG << "Initializing COM";
    HRESULT hr = CoInitialize(NULL);
    //CHECK_HRESULT(hr, "CoInitialize");

	// 10/7/2010 - This was removed for use with the NMCT_Server application.  NMCT server also uses the IntelSystemInfoLib which already enables
	// COM security.  We should fix this up in the future so both use COM correctly and can initialize and unitialize COM independently.

    LOG_HIDEBUG << "Initializing COM Security";
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
        throw runtime_error("No Microsoft_IPMI object found");

    // get the path of the first object
    CComVariant comVar;
    hr = ipmiInstance->Get(L"__PATH", 0, &comVar, NULL, NULL);
    CHECK_HRESULT(hr, "IWbemClassObject::Get(__PATH)");

    ipmiInstancePath = comVar.bstrVal;
    LOG_ERR << "WMI object instance: " << ipmiInstancePath;

    comVar.Clear();
    hr = ipmiInstance->Get(L"BMCAddress", 0, &comVar, NULL, NULL);
    CHECK_HRESULT(hr, "IWbemClassObject::Get(BMCAddress");
    bmcAddress = comVar.bVal;
    LOG_ERR << "BMC Address: 0x" << hex << (int)bmcAddress;

    // cache in-params signature
    CComPtr<IWbemClassObject> ipmiClass;
    hr = wbemServices->GetObjectA(ipmiClassName, WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, &ipmiClass, NULL);
    CHECK_HRESULT(hr, "IWbemServices::GetObject(Microsoft_IPMI)");
    hr = ipmiClass->GetMethod(L"RequestResponse", 0, &inParamsDef, NULL);
    CHECK_HRESULT(hr, "IWbemClassObject::GetMethod(RequestResponse)");
}

MicrosoftDriverIpmiProxy::~MicrosoftDriverIpmiProxy()
{
//    CoUninitialize();
}

void
MicrosoftDriverIpmiProxy::SMS_Attention(bool& AttentionSet, unsigned char& StatusRegisterValue)
{
	CComPtr<IWbemClassObject> inParams;
    CComPtr<IWbemClassObject> outParams;

    LOG_HIDEBUG << "MicrosoftDriverIpmiProxy::SMS_Attention() invoked";

	HRESULT hr = inParamsDef->Clone(&inParams);

    CHECK_HRESULT(hr, "IWbemClassObject::SpawnInstance (inParams)");

    // 1. Translate in-params

	// 2. Execute method

	hr = wbemServices->ExecMethod(ipmiInstancePath, L"SMS_Attention", 0, NULL, inParams, &outParams, NULL);
	CHECK_HRESULT(hr, "IWbemServices::ExecMethod");

	// 3. Translate out-params

	CComVariant arg;

	hr = outParams->Get(L"AttentionSet", 0, &arg, NULL, NULL);
	CHECK_HRESULT(hr, "Getting AttentionSet");
	if(arg.boolVal == VARIANT_TRUE)
		AttentionSet = true;
	else
		AttentionSet = false;
	arg.Clear();

	hr = outParams->Get(L"StatusRegisterValue", 0, &arg, NULL, NULL);
	CHECK_HRESULT(hr, "Getting StatusRegisterValue");
	StatusRegisterValue = arg.bVal;
}

KcsIpmiResponse
MicrosoftDriverIpmiProxy::requestResponse(const KcsIpmiRequest& req)
{
    CComPtr<IWbemClassObject> inParams;
    CComPtr<IWbemClassObject> outParams;

    LOG_HIDEBUG << "MicrosoftDriverIpmiProxy::requestResponse() invoked";

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

    CComVariant arg(static_cast<BYTE>(req.getNetFn()));
    hr = inParams->Put(L"NetworkFunction", 0, &arg, 0);
    CHECK_HRESULT(hr, "Setting NetworkFunction");
    arg.Clear();
    LOG_HIDEBUG << "NetworkFunction set to " << hex << (int)req.getNetFn();

    arg = static_cast<BYTE>(req.getLun());
    hr = inParams->Put(L"Lun", 0, &arg, 0);
    CHECK_HRESULT(hr, "Setting Lun");
    arg.Clear();
    LOG_HIDEBUG << "Lun set to " << hex << (int)req.getLun();

    arg = static_cast<BYTE>(req.getCmd());
    hr = inParams->Put(L"Command", 0, &arg, 0);
    CHECK_HRESULT(hr, "Setting Command");
    arg.Clear();
    LOG_HIDEBUG << "Command set to " << hex << (int)req.getCmd();

    arg = bmcAddress;
    hr = inParams->Put(L"ResponderAddress", 0, &arg, 0);
    CHECK_HRESULT(hr, "Setting ResponderAddress");
    arg.Clear();
    LOG_HIDEBUG << "ResponderAddress set to " << hex << 0x20;

    CComSafeArray<BYTE> argArray;
    arg = static_cast<int>(req.getData().size());
    hr = inParams->Put(L"RequestDataSize", 0, &arg, 0);
    CHECK_HRESULT(hr, "Setting RequestDataSize");
    arg.Clear();
    LOG_HIDEBUG << "RequestDataSize set to " << hex << req.getData().size();

    if (req.getData().size() > 0)
    {
        hr = argArray.Add(static_cast<ULONG>(req.getData().size()), &req.getData().front());
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

   KcsIpmiResponse rsp(completionCode, argArray.GetCount(), 
                             reinterpret_cast<BYTE*>(argArray.m_psa->pvData), true);

   argArray.Detach();

   return rsp;
}

#include "imb\imbapi.h"

IntelImbDriverIpmiProxy::IntelImbDriverIpmiProxy()
{
    LOG_HIDEBUG << "Initializing IntelImbDriverIpmiProxy";

	_seqNo = 0;

	int status = open_imb();

	// Get the starting sequece number
	unsigned char rspData[1024];
	unsigned int rspDataLen = 1024;
	(void)GetAsyncImbpMessage((ImbPacket *)rspData, (DWORD *)&rspDataLen, 1000, &_seqNo, 0xFF);
}

IntelImbDriverIpmiProxy::~IntelImbDriverIpmiProxy()
{
}

KcsIpmiResponse
IntelImbDriverIpmiProxy::requestResponse(const KcsIpmiRequest& req)
{
    LOG_HIDEBUG << "IntelImbDriverIpmiProxy::requestResponse() invoked";

	IMBPREQUESTDATA imbreq;
	int status, i;

	imbreq.rsSa	= 0x20;
	imbreq.rsLun	= 0;
	imbreq.busType	= 0;
	imbreq.netFn	= req.getNetFn();
	imbreq.cmdType	= req.getCmd();

	vector<unsigned char> data = req.getData();
	imbreq.dataLength = req.getData().size();
	if(imbreq.dataLength > 0)
		imbreq.data = &data[0];
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

	if(imbreq.cmdType == 0x33) // Get Message
	{
		if(getMessageResponseValid_ == true)
        {
			memcpy(rspData, getMessageResponseData_, getMessageResponseDataLength_);
			rspDataLen = getMessageResponseDataLength_;
			getMessageResponseValid_ = false;
			ccode = 0x00;
		}
		else
			ccode = 0x80;
	}
	else
	{
		for (i=0; i<1; i++)
		{
			status = SendTimedImbpRequest(&imbreq, 5000,
							  rspData, &rspDataLen, &ccode);
			if (status == 0)
				break;

			/* error */
			printf("Error sending IMB request, status=%x ccode=%x\n",
				   status, (int)ccode);
			LOG_HIDEBUG << "Retry: Error sending IMB request, status=" << hex << status << ", ccode=" << hex << (int)ccode << endl;
		}

		if(status == 0 && imbreq.cmdType == 0x34) // If its a Send Message, get the bridged response
		{
			getMessageResponseDataLength_ = 1024;

			int count = 0;
			status = 1;
			while(count < 1000)
			{
				status = GetAsyncImbpMessage((ImbPacket *)getMessageResponseData_, (DWORD *)&getMessageResponseDataLength_, 1000, &_seqNo, req.getData().at(0));
				if(status == 0)
					break;

				Sleep(1);
				count++;
			}

			if(status == 0)
			{
				/* workaround: some IMB driver versions do NOT report the BMC's SA in the packet */
				if (getMessageResponseData_[0] != BMC_SA)
				{
					for (int j = getMessageResponseDataLength_; j > 0; j--)
					{
						getMessageResponseData_[j] = getMessageResponseData_[j-1];
					}
					getMessageResponseDataLength_ = getMessageResponseDataLength_ + 1;
					getMessageResponseData_[0] = BMC_SA;	/* insert the BMC's SA so that the data will look like a real IMB packet */
				}

				getMessageResponseValid_ = true;
			}
		}
	}

	KcsIpmiResponse rsp(ccode, rspDataLen, rspData);
	return rsp;
}