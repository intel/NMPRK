/*******************************************************************************
********************************************************************************
***                                                                           **
***							INTEL CONFIDENTIAL								  **
***                    COPYRIGHT 2011 INTEL CORPORATION                       **
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

#include "WinDeviceFunctions.h"

#pragma comment(lib, "setupapi.lib")

SP_DEVINFO_DATA *GetDeviceInfo(wchar_t *guidStr, std::wstring deviceName, HDEVINFO &hDevInfo)
{
	std::string deviceNameString(deviceName.begin(), deviceName.end());

	GUID guids[1];
	CLSIDFromString(guidStr, guids);

	// Get the handle to a device information set for all devices matching classGuid that are present
	hDevInfo = SetupDiGetClassDevs(guids, NULL, NULL, 0x02);
	if(hDevInfo == INVALID_HANDLE_VALUE)
	{
		std::stringstream ss;
		ss << deviceNameString << ": Error preparing IPMI drivers!\n";
		return NULL;
	}

	// Get the device info for each device
	int index = 0;
	SP_DEVINFO_DATA *DeviceInfoData = new SP_DEVINFO_DATA;
	DeviceInfoData->cbSize = sizeof(SP_DEVINFO_DATA);
	while (SetupDiEnumDeviceInfo(hDevInfo, index, DeviceInfoData))
	{
		DWORD dataSize = 1000;
		BYTE data[1000];
		if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, DeviceInfoData, 0, 0, data, dataSize, &dataSize))
		{
			wchar_t str[1000];
			wcsncpy(str, (wchar_t *)data, dataSize);
			//wcout << " Device Description : " << str << endl;

			// Compare the descrptions to see if this is the device we are looking for
			if(wcsncmp(str, deviceName.c_str(), deviceName.size()) == 0)
			{
				return DeviceInfoData;
			}
		}

		index++;
	}

	delete DeviceInfoData;
	return NULL;
}

void ChangeDeviceState(wchar_t *guidStr, std::wstring deviceName, bool enable)
{
	std::string deviceNameString(deviceName.begin(), deviceName.end());

	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA *DeviceInfoData = GetDeviceInfo(guidStr, deviceName, hDevInfo);
	if(DeviceInfoData == NULL)
	{
		std::stringstream ss;
		ss << "Could not GetDeviceInfo for " << deviceNameString;
		throw WinDeviceFunctionsException(ss.str());
	}

	SP_PROPCHANGE_PARAMS params;

	if(enable == true)
	{
		params.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
		params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		params.StateChange = DICS_ENABLE;
		params.Scope = DICS_FLAG_GLOBAL;
		params.HwProfile = 0;

		if(SetupDiSetClassInstallParams(hDevInfo, DeviceInfoData, (PSP_CLASSINSTALL_HEADER)&params, sizeof(params)))
		{
			if(SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, DeviceInfoData) != TRUE)
			{
				DWORD err = GetLastError();
				if(err == ERROR_IN_WOW64)
				{
					throw WinDeviceFunctionsException(deviceNameString + ": Cannot access Device from a WOW64 process!");
				}
				else
				{
					std::stringstream ss;
					ss << deviceNameString << ": Other error: " << std::hex << "0x" << err;
					throw WinDeviceFunctionsException(ss.str());
				}
			}
		}
	}

	params.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	enable == true ? params.StateChange = DICS_ENABLE : params.StateChange = DICS_DISABLE;
	params.Scope = DICS_FLAG_CONFIGSPECIFIC;
	params.HwProfile = 0;

	std::string errorMsg = "";

	if(SetupDiSetClassInstallParams(hDevInfo, DeviceInfoData, (PSP_CLASSINSTALL_HEADER)&params, sizeof(params)))
	{
		if(SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, DeviceInfoData) != TRUE)
		{
			DWORD err = GetLastError();
			if (err == ERROR_NOT_DISABLEABLE)
			{
				throw WinDeviceFunctionsException(deviceNameString + ": Device can't be disabled (programmatically or in Device Manager).");
			}
			else if(err == ERROR_IN_WOW64)
			{
				throw WinDeviceFunctionsException(deviceNameString + ": Cannot access Device from a WOW64 process!");
			}
			else if (err <= ERROR_NO_ASSOCIATED_CLASS && err >= ERROR_DRIVER_STORE_DELETE_FAILED)
			{
				std::stringstream ss;
				ss << deviceNameString << ": SetupAPI error: " << std::hex << "0x" << err;
				throw WinDeviceFunctionsException(ss.str());
			}
			else
			{
				std::stringstream ss;
				ss << deviceNameString << ": Other error: " << std::hex << "0x" << err;
				throw WinDeviceFunctionsException(ss.str());
			}
		}
	}
}

bool IsDeviceInstalled(wchar_t *guidStr, std::wstring deviceName)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA *DeviceInfoData = GetDeviceInfo(guidStr, deviceName, hDevInfo);
	if(DeviceInfoData != NULL)
		return true;
	else
		return false;
}

bool IsDeviceEnabled(wchar_t *guidStr, std::wstring deviceName)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA *DeviceInfoData = GetDeviceInfo(guidStr, deviceName, hDevInfo);
	if(DeviceInfoData != NULL)
	{
		ULONG status = 0; 
		ULONG problem = 0;
		if (CM_Get_DevNode_Status(&status, &problem, (int)DeviceInfoData->DevInst, 0) == CR_SUCCESS)
		{
			if ((status & DN_HAS_PROBLEM) == DN_HAS_PROBLEM)
			{
				if (problem == CM_PROB_DISABLED)
					return false;
				else // error
					return false;
			}
			else
			{
				// enabled
				return true;
			}
		}
	}

	return false;
}
