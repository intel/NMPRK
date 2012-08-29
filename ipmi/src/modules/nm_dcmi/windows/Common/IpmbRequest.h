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
#ifndef _IPMB_REQUEST_H
#define _IPMB_REQUEST_H

#include "IpmiRequest.h"

class BridgedIpmiRequest : public IpmiRequest
{
public:

	// uses netFn = 0x06, cmd = 0x34, rsAddr = 0x20, rsLun = 0x00
	BridgedIpmiRequest(byte_t channel, IpmiRequest& eReq, std::string bridgedId="", bool lan = false);
	//BridgedIpmiRequest(byte_t channel, byte_t encapNetFn, byte_t encapCmd, byte_t encapRsAddr, byte_t encapRsLun, std::string eId="");

	// uses netFn = 0x06, cmd = 0x34
	BridgedIpmiRequest(byte_t rsAddr, byte_t rsLun, byte_t channel, IpmiRequest& eReq, std::string bridgedId="", bool lan = false);
	//BridgedIpmiRequest(byte_t rsAddr, byte_t rsLun, byte_t channel, byte_t encapNetFn, byte_t encapCmd, byte_t encapRsAddr, byte_t encapRsLun, std::string eId="");

	BridgedIpmiRequest(byte_t netFn, byte_t cmd, byte_t rsAddr, byte_t rsLun, byte_t channel, IpmiRequest& eReq, std::string bridgedId="", bool lan = false);
	//BridgedIpmiRequest(byte_t netFn, byte_t cmd, byte_t rsAddr, byte_t rsLun, byte_t channel, byte_t encapNetFn, byte_t encapCmd, byte_t encapRsAddr, byte_t encapRsLun, std::string eId="");

	virtual const dcmi_req_t& getRequestStruct();

	std::string getId() { return bridgedId_; }
	std::string getEncapId() { return IpmiRequest::getId(); }
        IpmiRequest& getEncapReq() { return encapReq_; }

private:
	void initializeBridgedIpmiRequest(byte_t channel, bool tracking, byte_t encapNetFn, byte_t encapCmd, byte_t encapRsAddr, byte_t encapRsLun, std::string eId);

private:
	static int sequenceNumber_;
	bool checksumAdded_;
	std::string bridgedId_;
        IpmiRequest encapReq_;
};

#endif // _IPMB_REQUEST_H
