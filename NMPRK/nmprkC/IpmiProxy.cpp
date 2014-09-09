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
#include "stdafx.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <atlsafe.h>
#include "MicrosoftIpmiProxy.h"
#include "IntelIpmiProxy.h"
#include "DcmiIpmiProxy.h"
#include "WinDeviceFunctions.h"
#endif
#include <iostream>

#include "DebugLog.h"
#include "IpmiProxy.h"
#include "OpenIpmiProxy.h"

using namespace std;

#define SI_THIS_MODULE SI_DEBUG_MODULE_IPMI_PROXY

#define SYSTEM_CLASS_GUID L"{4d36e97d-e325-11ce-bfc1-08002be10318}"
#define DCMI_DRIVER_CLASS L"{c55f8d91-917e-4c21-9cbf-f225da2b4a58}"

IpmiProxy::IpmiProxy()
{

}

IpmiProxy::~IpmiProxy()
{
    
}

#ifdef WIN32
bool IpmiProxy::GetKcsIpmiProxyInstance(IpmiProxy **proxy, std::string& errorMessage)
{
	bool msDriverInstalled = false, imbDriverInstalled = false, dcmiDriverInstalled = false;
	bool msDriverEnabled = false, imbDriverEnabled = false, dcmiDriverEnabled = false;

	try
	{
		*proxy = NULL;
		errorMessage = "No Error";

		const std::wstring msDevName = L"Microsoft Generic IPMI Compliant Device";
		const std::wstring imbDevName = L"Intel Intelligent Management Bus Driver";
		const std::wstring dcmiDevName = L"Intel IDC-HI Driver";

		msDriverInstalled = IsDeviceInstalled(SYSTEM_CLASS_GUID, msDevName);
		SI_DEBUG_TRACE(SI_THIS_MODULE, "msDriverInstalled: %s",(msDriverInstalled)?"true":"false");
		imbDriverInstalled = IsDeviceInstalled(SYSTEM_CLASS_GUID, imbDevName);
		SI_DEBUG_TRACE(SI_THIS_MODULE, "imbDriverInstalled: %s",(imbDriverInstalled)?"true":"false");
		dcmiDriverInstalled = IsDeviceInstalled(DCMI_DRIVER_CLASS, dcmiDevName);
		SI_DEBUG_TRACE(SI_THIS_MODULE, "dcmiDriverInstalled: %s",(dcmiDriverInstalled)?"true":"false");

		if(msDriverInstalled == false && imbDriverInstalled == false && dcmiDriverInstalled == false)
			throw IpmiProxyException(NMPRK_DRIVER_ERROR, "None of the following drivers are installed: Microsoft, Intel IMB or DCMI!");

		msDriverEnabled = IsDeviceEnabled(SYSTEM_CLASS_GUID, msDevName);
		SI_DEBUG_TRACE(SI_THIS_MODULE, "msDriverEnabled: %s",(msDriverEnabled)?"true":"false");
		imbDriverEnabled = IsDeviceEnabled(SYSTEM_CLASS_GUID, imbDevName);
		SI_DEBUG_TRACE(SI_THIS_MODULE, "imbDriverEnabled: %s",(imbDriverEnabled)?"true":"false");
		dcmiDriverEnabled = IsDeviceEnabled(DCMI_DRIVER_CLASS, dcmiDevName);
		SI_DEBUG_TRACE(SI_THIS_MODULE, "dcmiDriverEnabled: %s",(dcmiDriverEnabled)?"true":"false");

		if(msDriverEnabled == true && imbDriverEnabled == true)
		{
			SI_DEBUG_TRACE(SI_THIS_MODULE, "both drivers enabled!");
			// disable one
			SI_DEBUG_TRACE(SI_THIS_MODULE, "attempt to disable MS driver!");
			ChangeDeviceState(SYSTEM_CLASS_GUID, msDevName, false);
			msDriverEnabled = IsDeviceEnabled(SYSTEM_CLASS_GUID, msDevName);
			SI_DEBUG_TRACE(SI_THIS_MODULE, "msDriverEnabled: %s",(msDriverEnabled)?"true":"false");
			if(msDriverEnabled != false)
			{
				SI_DEBUG_TRACE(SI_THIS_MODULE, "attempt to disable IMB driver!");
				ChangeDeviceState(SYSTEM_CLASS_GUID, imbDevName, false);
				imbDriverEnabled = IsDeviceEnabled(SYSTEM_CLASS_GUID, imbDevName);
				SI_DEBUG_TRACE(SI_THIS_MODULE, "imbDriverEnabled: %s",(imbDriverEnabled)?"true":"false");
				if(imbDriverEnabled != false)
				{
					throw IpmiProxyException(NMPRK_DRIVER_ERROR, "Could not disable either the MS Driver or IMB Driver!");
				}
			}
		}
		else if(msDriverEnabled == false && imbDriverEnabled == false && dcmiDriverEnabled == false)
		{
			SI_DEBUG_TRACE(SI_THIS_MODULE, "no drivers enabled!");

			do
			{
				// enable one
				if(dcmiDriverInstalled == true)
				{
					SI_DEBUG_TRACE(SI_THIS_MODULE, "attempt to enable DCMI driver!");
					ChangeDeviceState(DCMI_DRIVER_CLASS, dcmiDevName, true);
					dcmiDriverEnabled = IsDeviceEnabled(DCMI_DRIVER_CLASS, dcmiDevName);
					SI_DEBUG_TRACE(SI_THIS_MODULE, "dcmiDriverEnabled: %s",(dcmiDriverEnabled)?"true":"false");
					if(dcmiDriverEnabled == true)
						break;
				}

				if(imbDriverInstalled == true)
				{
					SI_DEBUG_TRACE(SI_THIS_MODULE, "attempt to enable IMB driver!");
					ChangeDeviceState(SYSTEM_CLASS_GUID, imbDevName, true);
					imbDriverEnabled = IsDeviceEnabled(SYSTEM_CLASS_GUID, imbDevName);
					SI_DEBUG_TRACE(SI_THIS_MODULE, "imbDriverEnabled: %s",(imbDriverEnabled)?"true":"false");
					if(imbDriverEnabled == true)
						break;
				}
		
				if(msDriverInstalled == true)
				{
					SI_DEBUG_TRACE(SI_THIS_MODULE, "attempt to enable MS driver!");
					ChangeDeviceState(SYSTEM_CLASS_GUID, msDevName, true);
					msDriverEnabled = IsDeviceEnabled(SYSTEM_CLASS_GUID, msDevName);
					SI_DEBUG_TRACE(SI_THIS_MODULE, "msDriverEnabled: %s",(msDriverEnabled)?"true":"false");
					if(msDriverEnabled == true)
						break;
				}
			}
			while(false);
		}

		if(dcmiDriverEnabled == true)
			*proxy = new DcmiIpmiProxy;
		else if(imbDriverEnabled == true)
			*proxy = new IntelIpmiProxy;
		else if(msDriverEnabled == true)
			*proxy = new MicrosoftIpmiProxy;
		else
			throw IpmiProxyException(NMPRK_DRIVER_ERROR, "Could not enable either the MS Driver or IMB Driver!");

		return true;
	}
	catch(std::exception &ex)
	{
		errorMessage = ex.what();

		if(imbDriverEnabled == true)
		{
			errorMessage += " Attempting to use Intel IMB Driver.";
			*proxy = new IntelIpmiProxy;	
		}
		else if(msDriverEnabled == true)
		{
			errorMessage += " Attempting to use Microsoft Driver.";
			*proxy = new MicrosoftIpmiProxy;
		}

		if((*proxy) != NULL)
		{
			// drivers may not be correctly configured, but they seem to be working
			ipmi_req_t req;
			req.netFun = 0x06;
			req.cmd = 0x01;
			req.rsAddr = 0x20;
			req.rsLun = 0x00;
			req.len = 0;

			ipmi_rsp_t rsp;
			rsp = (*proxy)->RequestResponse(req);
			if(rsp.compCode == 0x00 && rsp.len > 10)
				return true;
		}

		return false;
	}

	return false;
}
#else
bool IpmiProxy::GetKcsIpmiProxyInstance(IpmiProxy **proxy, std::string& errorMessage)
{
    try
    {
        *proxy = new OpenIpmiProxy;
        return true;
    }
    catch(std::exception& ex)
    {
        errorMessage = ex.what();
        return false;
    }
}
#endif
