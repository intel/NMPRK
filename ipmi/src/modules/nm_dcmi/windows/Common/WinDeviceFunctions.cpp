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

#include "WinDeviceFunctions.h"

#pragma comment(lib, "setupapi.lib")

SP_DEVINFO_DATA *GetDeviceInfo(std::wstring deviceName, HDEVINFO &hDevInfo)
{
	// Get the class GUID
	GUID guids[1];
	DWORD requiredSize = 1; 
	if(SetupDiClassGuidsFromName("System", guids, requiredSize, &requiredSize) != TRUE)
	{
		std::stringstream ss;
		ss << "Error preparing IPMI drivers!\n";
		return NULL;
	}

	// Get the handle to a device information set for all devices matching classGuid that are present
	hDevInfo = SetupDiGetClassDevs(guids, NULL, NULL, 0x02);
	if(hDevInfo == INVALID_HANDLE_VALUE)
	{
		std::stringstream ss;
		ss << "Error preparing IPMI drivers!\n";
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

void ChangeDeviceState(std::wstring deviceName, bool enable)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA *DeviceInfoData = GetDeviceInfo(deviceName, hDevInfo);
	if(DeviceInfoData == NULL)
	{
		std::string str(deviceName.begin(), deviceName.end());
		std::stringstream ss;
		ss << "Could not GetDeviceInfo for " << str;
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
					throw WinDeviceFunctionsException("Cannot access Device from a WOW64 process!");
				}
				else
				{
					std::stringstream ss;
					ss << "Other error: " << std::hex << "0x" << err;
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
				throw WinDeviceFunctionsException("Device can't be disabled (programmatically or in Device Manager).");
			}
			else if(err == ERROR_IN_WOW64)
			{
				throw WinDeviceFunctionsException("Cannot access Device from a WOW64 process!");
			}
			else if (err <= ERROR_NO_ASSOCIATED_CLASS && err >= ERROR_DRIVER_STORE_DELETE_FAILED)
			{
				std::stringstream ss;
				ss << "SetupAPI error: " << std::hex << "0x" << err;
				throw WinDeviceFunctionsException(ss.str());
			}
			else
			{
				std::stringstream ss;
				ss << "Other error: " << std::hex << "0x" << err;
				throw WinDeviceFunctionsException(ss.str());
			}
		}
	}
}

bool IsDeviceInstalled(std::wstring deviceName)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA *DeviceInfoData = GetDeviceInfo(deviceName, hDevInfo);
	if(DeviceInfoData != NULL)
		return true;
	else
		return false;
}

bool IsDeviceEnabled(std::wstring deviceName)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA *DeviceInfoData = GetDeviceInfo(deviceName, hDevInfo);
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
