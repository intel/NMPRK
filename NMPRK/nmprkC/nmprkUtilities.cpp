/***************************************************************************
 * Copyright 2013 Intel Corporation                                        *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 * http://www.apache.org/licenses/LICENSE-2.0                              *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

#include "stdafx.h"

#ifndef WIN32
#define IN
#define OUT
#endif

#include <string.h>
#include <map>
#include "nmprkUtilities.h"

std::map<nmprk_conn_handle_t, byte_t> gHandleNmVersionMap;

extern "C" nmprk_status_t NMPRK_GetVersion(
	IN nmprk_conn_handle_t h,
	OUT nm_get_version_output_t *output);

void nmprkInitializeIpmiRequest(ipmi_req_t& req, byte_t netFun, byte_t cmd)
{
	memset(&req, 0, sizeof(ipmi_req_t));
	req.netFun = netFun;
	req.cmd = cmd;
	req.rsAddr = BMC_ADDRESS;
}

void nmprkInitializeRequest(ipmi_req_t& req, byte_t netFun, byte_t cmd)
{
	nmprkInitializeIpmiRequest(req, netFun, cmd);
	req.data[req.len++] = 0x57;
	req.data[req.len++] = 0x01;
	req.data[req.len++] = 0x00;
}

byte_t nmprkGetByte(ulong_t value, int index)
{
	return (value >> (8 * index)) & 0x00000000000000FF;
}

nmprk_status_t nmprkGetNodeManagerVersion(nmprk_conn_handle_t h, byte_t *version)
{
	if(version == NULL)
		return NMPRK_INVALID_HANDLE;

	if(gHandleNmVersionMap.find(h) != gHandleNmVersionMap.end())
	{
		*version = gHandleNmVersionMap[h];
		return NMPRK_SUCCESS;
	}

	nm_get_version_output_t verOut;
	nmprk_status_t status = NMPRK_GetVersion(h, &verOut);
	if(status == NMPRK_SUCCESS)
	{
		*version = verOut.version;
	}

	return status;
}

#if 0
void copyRepoInfo(nm_ipmi_repo_info_t *info, nmprk::ipmi::repoInfo_t *retInfo)
{
	info->repoVersion = retInfo->repoVersion;
	info->repoEntries = retInfo->repoEntries;
	info->repoFreeSpace = retInfo->repoFreeSpace;
	info->mostRecentAddTS = retInfo->mostRecentAddTS;
	info->mostRecentDelTS = retInfo->mostRecentDelTS;
	info->getAllocInfoSup = retInfo->getAllocInfoSup;
	info->parAddSup = retInfo->parAddSup;
	info->delSup = retInfo->delSup;
	info->reserveSup = retInfo->reserveSup;
	info->modalSupported = retInfo->modalSupported;
	info->nonmodalSupported = retInfo->nonmodalSupported;
}
#endif
