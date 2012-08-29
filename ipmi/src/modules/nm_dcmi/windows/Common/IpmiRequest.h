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
#ifndef _IPMI_REQUEST_H
#define _IPMI_REQUEST_H

#include "IpmiCommon.h"

class IpmiRequest
{
public:
	//IpmiRequest();
	IpmiRequest(byte_t netFn, byte_t cmd, byte_t rsAddr = 0x20, byte_t rsLun = 0x00, std::string id="");

	/*
	void setNetFunction(byte_t b);
	void setCommand(byte_t b);
	void setResponderAddr(byte_t b);
	void setResponderLun(byte_t b);
	*/
	byte_t getNetFunction() { return req_.netFun; }
	byte_t getCommand() { return req_.cmd; }
	byte_t getResponderAddr() { return req_.rsAddr; }
	byte_t getResponderLun() { return req_.rsLun; }

	virtual std::string getId() { return id_; }

	void addDataByte(byte_t b);
	byte_t getDataByte(int idx);
	IPMI_STATUS getDataByte(int idx, byte_t &b);
	IPMI_STATUS setDataByte(int idx, byte_t b);

	int getDataLength() { return req_.len; }

	virtual const dcmi_req_t& getRequestStruct() { return req_; }

	bool isBridged() { return isBridged_; }

protected:
	bool isBridged_;

private:
	dcmi_req_t req_;
	std::string id_;
};

void PrintIpmiRequestToLog(IpmiRequest *req);

#endif // _IPMI_REQUEST_H
