
#include "stdafx.h"

#include "transportInterfaceOriginal.h"

#ifdef USE_TI_ORIGINAL
extern nmprk_conn_handle_t gHandleAllocator;

nmprk_status_t transportInterfaceOriginal::ConnectLocal(
	OUT nmprk_conn_handle_t *h)
{
	if(h == NULL)
		return NMPRK_NULL_CODE;

	if(getDeviceByHandle(KCS_DEVICE_HANDLE) == NULL)
	{
		try
		{
			nmprk::ipmi::device *d = new nmprk::ipmi::device();
			if(nmprk::translation::swSubSystemSetup(nmprk::translation::initDevice,d) == true)
			{
				*h = KCS_DEVICE_HANDLE;
				addDevice(*h, d);

				try
				{
					// clear the msg queue
					ipmi_req_t req;
					nmprkInitializeIpmiRequest(req, 0x06, 0x30);
					req.data[req.len++] = 0x03; // clear recv msg queue and event message buffer

					ipmi_rsp_t rsp;
					NMPRK_SendNonBridgedCommand(*h, &req, &rsp); // ignore response
				}
				catch(...) { }

				return NMPRK_SUCCESS;
			}
			else
				return NMPRK_FAILED_OPEN_KCS_CODE;
		}
		catch(nmprk::nmprkException *e)
		{
			return e->errorCode;
		}
	}

	*h = KCS_DEVICE_HANDLE;
	return NMPRK_SUCCESS;
}

nmprk_status_t transportInterfaceOriginal::ConnectRemote(
	IN nmprk_connect_remote_parameters_t *input, 
	OUT nmprk_conn_handle_t *h)
{
	if(h == NULL)
		return NMPRK_NULL_CODE;

	try
	{
		nmprk::ipmi::device *d = new nmprk::ipmi::device();
		d->type = nmprk::ipmi::device_nm;
		d->address = input->ipOrHostname;
		d->user = input->username;
		d->password = input->password;
		if(input->useCustomCipherSuite == true)
			d->cipher = (nmprk::ipmi::ipmiCipherSuite_t)input->customCipherSuite;
		else
			d->cipher = nmprk::ipmi::defaultCipherSuite;

		if(nmprk::translation::swSubSystemSetup(nmprk::translation::initDevice,d) == true)
		{
			*h = gHandleAllocator++;
			addDevice(*h, d);
			return NMPRK_SUCCESS;
		}
		else
			return NMPRK_FAILED_OPEN_REMOTE_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		return NMPRK_FAILED_OPEN_REMOTE_CODE;
	}
	catch(...)
	{
		return NMPRK_FAILED_OPEN_REMOTE_CODE;
	}
}

nmprk_status_t transportInterfaceOriginal::Disconnect(
	IN nmprk_conn_handle_t h)
{
	try
	{
		nmprk::ipmi::device *d = getDeviceByHandle(h);
		if(d == NULL)
			return NMPRK_INVALID_HANDLE_CODE;

		std::map<nmprk_conn_handle_t, nmprk::ipmi::device *>::iterator it = 
			gRemoteDeviceMap.find(h);
		gRemoteDeviceMap.erase(it);

		if(nmprk::ipmi::disconnectDevice(d) == true)
			return NMPRK_SUCCESS;
		else
			return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		return e->errorCode;
	}
	catch(...)
	{
		return NMPRK_CMD_FAILED_CODE;
	}
}

nmprk_status_t transportInterfaceOriginal::SetDefaultNmCommandBridging(
	IN nmprk_conn_handle_t h,
	IN byte_t channel,
	IN byte_t address)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	nm_bridging_info_t bi;
	bi.channel = channel;
	bi.address = address;
	gHandleBridgingMap[h] = bi;

	return NMPRK_SUCCESS;
}

nmprk_status_t transportInterfaceOriginal::SendCommandInternal(
	IN nmprk_conn_handle_t h, 
	IN bool bridgeIfSet, 
	IN ipmi_req_t *reqA, 
	OUT ipmi_rsp_t *rspA)
{
	nmprk::ipmi::commandReq_t req;
	nmprk::ipmi::commandRsp_t rsp;

	try
	{
		nmprk::ipmi::device *d = getDeviceByHandle(h);
		if(d == NULL)
			return NMPRK_INVALID_HANDLE_CODE;

		req.data.push_back(nmprk::helper::int2HexStr(reqA->netFun));
		req.data.push_back(nmprk::helper::int2HexStr(reqA->cmd));
		for(int i = 0; i < reqA->len && i < MAX_IPMI_SIZE; i++)
			req.data.push_back(nmprk::helper::int2HexStr(reqA->data[i]));
    
		byte_t bridge = d->bridge;
		byte_t transport = d->transport;

		// if NM command is supposed to be bridged
		if(bridgeIfSet == true && gHandleBridgingMap.find(h) != gHandleBridgingMap.end())
		{
			d->bridge = gHandleBridgingMap[h].channel;
			d->transport = gHandleBridgingMap[h].address;
		}

		bool status = runIpmiCommandWrapper(d,&req,&rsp);

		if(bridgeIfSet == true && gHandleBridgingMap.find(h) != gHandleBridgingMap.end())
		{
			d->bridge = bridge;
			d->transport = transport;
		}

		if(status == true)
		{
			rspA->compCode = rsp.rspCode;

			std::vector<std::string>::iterator it;
			for(it = rsp.data.begin(), rspA->len = 0; rspA->len < MAX_IPMI_SIZE && it != rsp.data.end(); it++, rspA->len++)
				rspA->data[rspA->len] = nmprk::helper::hexStr2Int(*it);

			return NMPRK_SUCCESS;
		}
		else
			return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		return e->errorCode;
	}
	catch(...)
	{
		return NMPRK_CMD_FAILED_CODE;
	}
}

nmprk_status_t transportInterfaceOriginal::SendBridgedCommand(
	IN nmprk_conn_handle_t h, 
	IN byte_t channel, 
	IN byte_t address, 
	IN ipmi_req_t *req, 
	OUT ipmi_rsp_t *rsp)
{
	nmprk_status_t status;

	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	byte_t bridge, transport;

	bridge = d->bridge;
	transport = d->transport;

	d->bridge = channel;
	d->transport = address;

	status = SendCommandInternal(h, false, req, rsp);

	d->bridge = bridge;
	d->transport = transport;

	return status;
}

nmprk_status_t transportInterfaceOriginal::GetLastRequestResponse(
	IN nmprk_conn_handle_t h, 
	OUT ipmi_capture_req_t *req, 
	OUT ipmi_capture_rsp_t *rsp)
{
	try
	{
		nmprk::ipmi::device *d = getDeviceByHandle(h);
		if(d == NULL)
			return NMPRK_INVALID_HANDLE_CODE;

		if(req == NULL || rsp == NULL)
			return NMPRK_NULL_CODE;

		if(gHandleLastReqRspMap.find(d) == gHandleLastReqRspMap.end())
			return NMPRK_NOT_FOUND_CODE;

		req->count = gHandleLastReqRspMap[d].req.count;
		memcpy(req->data, &gHandleLastReqRspMap[d].req.data, req->count);

		rsp->count = gHandleLastReqRspMap[d].rsp.count;
		memcpy(rsp->data, &gHandleLastReqRspMap[d].rsp.data, rsp->count);

		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		return e->errorCode;
	}
	catch(...)
	{
		return NMPRK_CMD_FAILED_CODE;
	}
}

nmprk_status_t transportInterfaceOriginal::GetErrorString(
	IN nmprk_status_t code, 
	IN OUT char *str, IN int strsize)
{
	if(str == NULL || strsize <= 0)
		return NMPRK_NULL_CODE;

	try
	{
		nmprk::ipmi::ipmi_rspCodes(code);
		return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		strncpy(str, e->errorMsg.c_str(), strsize-1);
		return NMPRK_SUCCESS;
	}
	catch(...)
	{
		return NMPRK_CMD_FAILED_CODE;
	}
}

void transportInterfaceOriginal::addDevice(nmprk_conn_handle_t h, nmprk::ipmi::device *d)
{
	nmprk::auto_mutex lock(gRemoveDeviceMapLock);

	gRemoteDeviceMap[h] = d;
}

void transportInterfaceOriginal::removeDeviceByHandle(nmprk_conn_handle_t h)
{
	nmprk::auto_mutex lock(gRemoveDeviceMapLock);

	std::map<nmprk_conn_handle_t, nmprk::ipmi::device *>::iterator it = 
		gRemoteDeviceMap.find(h);
	gRemoteDeviceMap.erase(it);
}

nmprk::ipmi::device *transportInterfaceOriginal::getDeviceByHandle(nmprk_conn_handle_t h)
{
	nmprk::auto_mutex lock(gRemoveDeviceMapLock);

	return gRemoteDeviceMap[h];
}

void transportInterfaceOriginal::saveLastReqRsp(nmprk::ipmi::device *d)	
{
	if(d != NULL)
	{
		if(d->lastReq.data.size() > 0 && d->lastRsp.rspCode >= 0)
		{
			ipmi_capture_req_rsp_t capture;
			memset(&capture, 0, sizeof(capture));

			capture.req.count = d->lastReq.data.size();
			for(int i = 0; i < d->lastReq.data.size(); i++)
				capture.req.data[i] = nmprk::helper::hexStr2Int(d->lastReq.data[i]);

			capture.rsp.count = d->lastRsp.data.size() + 1;
			capture.rsp.data[0] = d->lastRsp.rspCode;
			for(int i = 0; i < d->lastRsp.data.size(); i++)
				capture.rsp.data[i+1] = nmprk::helper::hexStr2Int(d->lastRsp.data[i]);

			gHandleLastReqRspMap[d] = capture;
		}
	}
}

void transportInterfaceOriginal::clearLastReqRsp(nmprk::ipmi::device *d)
{
	std::map<nmprk::ipmi::device *, ipmi_capture_req_rsp_t>::iterator it;
	it = gHandleLastReqRspMap.find(d);
	if(it != gHandleLastReqRspMap.end())
		gHandleLastReqRspMap.erase(it);
}

bool transportInterfaceOriginal::runIpmiCommandWrapper(nmprk::ipmi::device *d, nmprk::ipmi::commandReq_t *req, nmprk::ipmi::commandRsp_t *rsp)
{
	bool ret = false;
	try
	{
		ret = nmprk::ipmi::runIpmiCommand(d,req,rsp);
		saveLastReqRsp(d);
	}
	catch (...)
	{
		saveLastReqRsp(d);
		throw;
	}

	return ret;
}

nmprk_status_t transportInterfaceOriginal::NMPRK_GetDiscoveryParameters(
	IN nmprk_conn_handle_t h, 
	OUT nm_discovery_parameters_t *params)
{
	nmprk_status_t status;
	bool foundDiscovery = false;
	ushort_t nextRecord = 0;

	if(params == NULL)
		return NMPRK_NULL_CODE;

	do
	{
		nm_ipmi_record_t record;
		ushort_t recordId = nextRecord;
		byte_t type = 0;

		status = NMPRK_IPMI_GetSdrRecord(h, 0, &record);
		if(status != NMPRK_SUCCESS)
			return status;

		nextRecord = record.nextRecord;

		//if(record->len >= 2)
		//	recordId = record->data[0] | (record->data[1] << 8);
		//if(record->len >= 3)
			//version = record->data[2];
		if(record.len >= 4)
			type = record.data[3];
		//if(record->len >= 5)
			//len = record->data[4];
		if (type == 0xC0) // OEM record
		{
			// first 3 bytes are Intel ID 0x570100 (5,6,7)
			byte_t recordSubtype = record.data[8];

			//LOG_HIDEBUG << "Record SubType: 0x" << std::hex << (int) recordSubtype << std::endl;

			// Node Manager Discovery SubType
			if (recordSubtype == 0x0D)
			{
				// version - 1 byte
				// I2C Slave address of NM
				params->address = record.data[10];
				params->channel = record.data[11] >> 4;
				params->nmHealthEvSensor = record.data[12];
				params->nmExceptionEvSensor = record.data[13];
				params->nmOperationalCapSensor = record.data[14];
				params->nmAlertThresExcdSensor = record.data[15];

				//LOG_HIDEBUG << "NM Address: 0x" << std::hex << (int) nmAddress_ << std::endl;
				//LOG_HIDEBUG << "NM Channel: 0x" << std::hex << (int) nmChannel_ << std::endl;
				foundDiscovery = true;

				// this is the only record we want right now, break out of loop
				nextRecord = 0xFFFF;
			}
		}
	}
	while (nextRecord != 0xFFFF);

	if(foundDiscovery == true)
		return NMPRK_SUCCESS;
	else
		return NMPRK_NOT_FOUND_CODE;
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_GetDeviceId(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_device_id_t *devId)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(devId == NULL)
		return NMPRK_NULL_CODE;

	try
	{
		nmprk::ipmi::getDeviceIdRsp *retInfo = nmprk::ipmi::global::getDeviceId(d);
		if(retInfo == NULL)
			return NMPRK_CMD_FAILED_CODE;

		saveLastReqRsp(d);

		devId->deviceId = retInfo->deviceId;
		devId->deviceProvidesSdr = retInfo->deviceProvidesSdr;
		devId->deviceRev = retInfo->deviceRev;
		devId->devNormOp = retInfo->devNormOp;
		devId->firmwareRev = retInfo->firmwareRev;
		devId->firmwareRev2 = retInfo->firmwareRev2;
		devId->ipmiVersion = retInfo->ipmiVersion;
		devId->isBridgeDev = retInfo->isBridgeDev;
		devId->isChassisDev = retInfo->isChassisDev;
		devId->isFruInvDev = retInfo->isFruInvDev;
		devId->isSelDev = retInfo->isSelDev;
		devId->isIpmbGenDev = retInfo->isIpmbGenDev;
		devId->isIpmbRevDev = retInfo->isIpmbRevDev;
		devId->isSdrRepoDev = retInfo->isSdrRepoDev;
		devId->isSensorDev = retInfo->isSensorDev;
		int manId = nmprk::helper::hexStr2Int(retInfo->manufId);
		devId->manufId[0] = manId & 0xFF;
		devId->manufId[1] = (manId >> 8) & 0xFF;
		devId->manufId[2] = (manId >> 16) & 0xFF;
		int proId = nmprk::helper::hexStr2Int(retInfo->productId);
		devId->productId[0] = proId & 0xFF;
		devId->productId[1] = (proId >> 8) & 0xFF;

		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_GetSdrInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_repo_info_t *info)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(info == NULL)
		return NMPRK_NULL_CODE;

	try
	{
		nmprk::ipmi::repoInfo_t *retInfo = nmprk::ipmi::sdr::getSdrInfo(d);
		if(retInfo == NULL)
			return NMPRK_CMD_FAILED_CODE;

		saveLastReqRsp(d);

		copyRepoInfo(info, retInfo);

		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_GetSdrRecord(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId,
	OUT nm_ipmi_record_t *record)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(record == NULL)
		return NMPRK_NULL_CODE;

	nmprk::ipmi::record_t* tmpRecord;
	nmprk::ipmi::address_t tmpRecordId;
	tmpRecordId.msb = (recordId >> 8) & 0xFF;
	tmpRecordId.lsb = recordId & 0xFF;

	try
	{
		tmpRecord = nmprk::ipmi::sdr::getSdrRecord(d, &tmpRecordId);
		if(tmpRecord == NULL)
			return NMPRK_CMD_FAILED_CODE;

		saveLastReqRsp(d);

		record->nextRecord = ((tmpRecord->nextRecord.msb << 8) & 0xFF00) | tmpRecord->nextRecord.lsb;
		record->len = tmpRecord->len;
		for(int i = 0; i < tmpRecord->len; i++)
			record->data[i] = (unsigned char)tmpRecord->data[i];
		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_AddSdrRecord(
	IN nmprk_conn_handle_t h,
	IN nm_ipmi_record_t *record,
	OUT ushort_t *recordId)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(record == NULL || recordId == NULL)
		return NMPRK_NULL_CODE;

	nmprk::ipmi::address_t tmpRecordId;
	nmprk::ipmi::record_t tmpRecord;

	tmpRecord.len = record->len;
	memcpy(tmpRecord.data, record->data, tmpRecord.len);
	
	try
	{
		if(nmprk::ipmi::sdr::addSdrRecord(d, &tmpRecordId, &tmpRecord) != true)
			return NMPRK_CMD_FAILED_CODE;

		saveLastReqRsp(d);

		*recordId = ((tmpRecordId.msb << 8) & 0xFF00) | tmpRecordId.lsb;

		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_DeleteSdrRecord(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	nmprk::ipmi::address_t tmpRecordId;
	tmpRecordId.msb = (recordId >> 8) & 0xFF;
	tmpRecordId.lsb = recordId & 0xFF;

	try
	{
		if(nmprk::ipmi::sdr::delSdrRecord(d, &tmpRecordId) == true)
		{
			saveLastReqRsp(d);
			return NMPRK_SUCCESS;
		}
		else
			return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_ClearSdrRepository(
	IN nmprk_conn_handle_t h)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	try
	{
		if(nmprk::ipmi::sdr::clearSdr(d) != true)
		{
			saveLastReqRsp(d);
			return NMPRK_SUCCESS;
		}
		else
			return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_GetSelInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_repo_info_t *info)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(info == NULL)
		return NMPRK_NULL_CODE;

	try
	{
		nmprk::ipmi::repoInfo_t *retInfo = nmprk::ipmi::sel::getSelInfo(d);
		if(retInfo == NULL)
			return NMPRK_CMD_FAILED_CODE;

		saveLastReqRsp(d);

		copyRepoInfo(info, retInfo);

		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_GetSelEntry(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId,
	OUT nm_ipmi_sel_entry_t *entry)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(entry == NULL)
		return NMPRK_NULL_CODE;

	nmprk::ipmi::record_t* tmpRecord;
	nmprk::ipmi::address_t tmpRecordId;
	tmpRecordId.msb = (recordId >> 8) & 0xFF;
	tmpRecordId.lsb = recordId & 0xFF;

	try
	{
		tmpRecord = nmprk::ipmi::sel::getSelRecord(d, &tmpRecordId);
		if(tmpRecord == NULL)
			return NMPRK_CMD_FAILED_CODE;

		saveLastReqRsp(d);

		entry->nextEntry = ((tmpRecord->nextRecord.msb << 8) & 0xFF00) | tmpRecord->nextRecord.lsb;
		entry->len = tmpRecord->len;
		memcpy(entry->data, tmpRecord->data, entry->len);
		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_DeleteSelEntry(
	IN nmprk_conn_handle_t h,
	IN ushort_t recordId)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	nmprk::ipmi::address_t tmpRecordId;
	tmpRecordId.msb = (recordId >> 8) & 0xFF;
	tmpRecordId.lsb = recordId & 0xFF;

	try
	{
		if(nmprk::ipmi::sel::delSelRecord(d, &tmpRecordId) == true)
		{
			saveLastReqRsp(d);
			return NMPRK_SUCCESS;
		}
		else
			return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_ClearSel(
	IN nmprk_conn_handle_t h)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	try
	{
		if(nmprk::ipmi::sel::clearSel(d) != true)
		{
			saveLastReqRsp(d);
			return NMPRK_SUCCESS;
		}
		else
			return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_GetFruInfo(
	IN nmprk_conn_handle_t h,
	OUT nm_ipmi_fru_info_t *output)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(output == NULL)
		return NMPRK_NULL_CODE;

	try
	{
		nmprk::ipmi::fruInfo_t *info = nmprk::ipmi::fru::getFruInfo(d);
		if(info == NULL)
			return NMPRK_CMD_FAILED_CODE;

		saveLastReqRsp(d);
			
		output->fruSize = info->fruSize[0] | (info->fruSize[1] << 8);
		output->accessByWord = info->accessByWord;

		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_ReadFruData(
	IN nmprk_conn_handle_t h,
	IN int_t offset,
	IN OUT int_t *length,
	OUT byte_t *data)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(length == NULL || data == NULL)
		return NMPRK_NULL_CODE;

	if(*length <= 0)
		return NMPRK_INVALID_ARGUMENT_CODE;

	try
	{
		nmprk::ipmi::record_t *rec = nmprk::ipmi::fru::getFruData(d, offset, *length);
		if(rec == NULL)
			return NMPRK_CMD_FAILED_CODE;

		saveLastReqRsp(d);
		
		int lengthToCopy = (rec->len < *length) ? rec->len : *length;
		if(lengthToCopy > 0)
			memcpy(data, rec->data, lengthToCopy);
		*length = lengthToCopy;

		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_WriteFruData(
	IN nmprk_conn_handle_t h,
	IN int_t offset,
	IN OUT int_t *length,
	IN byte_t *data)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(length == NULL || data == NULL)
		return NMPRK_NULL_CODE;

	if(*length <= 0)
		return NMPRK_INVALID_ARGUMENT_CODE;

	try
	{
		nmprk::ipmi::address_t off;
		nmprk::ipmi::record_t rec;

		off.lsb = offset & 0xFF;
		off.msb = (offset >> 8) & 0xFF;
		int lengthToCopy = (sizeof(rec.data) < *length) ? sizeof(rec.data) : *length;
		if(lengthToCopy > 0)
			memcpy(rec.data, data, lengthToCopy);
		rec.len = lengthToCopy;

		if(nmprk::ipmi::fru::setFruData(d, &off, &rec) == true)
		{
			saveLastReqRsp(d);
			return NMPRK_SUCCESS;
		}
		else
			return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_SetAcpiPowerState(
	IN nmprk_conn_handle_t h,
	IN ipmi_acpi_power_state_t *input)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(input == NULL)
		return NMPRK_NULL_CODE;

	try
	{
		nmprk::ipmi::acpiPwrState_t state;
		state.deviceState = (nmprk::ipmi::acpiDevicePwrState_t)input->deviceState;
		state.systemState = (nmprk::ipmi::acpiSystemPwrState_t)input->systemState;
		if(nmprk::ipmi::global::setAcpiPwrState(d, state) == true)
		{
			saveLastReqRsp(d);
			return NMPRK_SUCCESS;
		}
		else
			return NMPRK_CMD_FAILED_CODE;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}

nmprk_status_t transportInterfaceOriginal::NMPRK_IPMI_GetAcpiPowerState(
	IN nmprk_conn_handle_t h,
	OUT ipmi_acpi_power_state_t *output)
{
	nmprk::ipmi::device *d = getDeviceByHandle(h);
	if(d == NULL)
		return NMPRK_INVALID_HANDLE_CODE;

	clearLastReqRsp(d);

	if(output == NULL)
		return NMPRK_NULL_CODE;

	try
	{
		nmprk::ipmi::acpiPwrState_t state = nmprk::ipmi::global::getAcpiPwrState(d);
		saveLastReqRsp(d);
		output->systemState = (ipmi_acpi_system_power_state_t)state.systemState;
		output->deviceState = (ipmi_acpi_device_power_state_t)state.deviceState;
		return NMPRK_SUCCESS;
	}
	catch(nmprk::nmprkException *e)
	{
		saveLastReqRsp(d);
		return e->errorCode;
	}
}
#endif
