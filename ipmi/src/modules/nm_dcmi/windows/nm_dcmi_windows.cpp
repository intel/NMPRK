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
#include <Windows.h>

#include <nm_dcmi.h>
#include <nmprk_ipmi.h>
#include <nmprk_exception.h>
#include <nmprk_errCodes.h>
#include <nmprk_defines.h>
#include <nmprk_helper.h>

#include "TransportPluginDll.h"
#include "TransportPluginLoader.h"
#include "parameters.h"
#include "Common\IpmbRequest.h"
#include "Common\IpmiRequest.h"

static 	TransportPluginLoader transportPluginLoader;

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_connectToDevice(nmprk::ipmi::device* d) {
 bool ret = false;
 if(d != NULL) {
	int iError;

	 try
    {
        PluginId transportPlugins[] =
        {
			TP_KCS, "TransportKCS"
        };
        transportPluginLoader.load(std::vector<PluginId>(transportPlugins,
                        transportPlugins + sizeof(transportPlugins)/sizeof(PluginId)));
    }
    catch (std::exception& e)
    {
		throw new nmprk::nmprkException(NMPRK_FAILED_INIT_DLL_CODE,NMPRK_FAILED_INIT_DLL_MSG); 
    }

	transport_plugin_interface* intf = NULL;
	intf = transportPluginLoader.getPluginById(TP_KCS);
	iError = intf->ipmiConnect(IPMI_SESSION_HANDLE_DEFAULT);
	if(iError != SUCCESS) {
		throw new nmprk::nmprkException(NMPRK_FAILED_OPEN_KCS_CODE,NMPRK_FAILED_OPEN_KCS_MSG);
	}
	ret = true;
	

 }else{
	 throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_connectToDevice(nmprk::ipmi::device* d) {
 bool ret = false;
 if(d != NULL) {
	int iError;

	 try
    {
        PluginId transportPlugins[] =
        {
			TP_RMCPP, "TransportRMCPP"
        };
        transportPluginLoader.load(std::vector<PluginId>(transportPlugins,
                        transportPlugins + sizeof(transportPlugins)/sizeof(PluginId)));
    }
    catch (std::exception& e)
    {
		throw new nmprk::nmprkException(NMPRK_FAILED_INIT_DLL_CODE,NMPRK_FAILED_INIT_DLL_MSG); 
    }

	transport_plugin_interface* intf = NULL;
	intf = transportPluginLoader.getPluginById(TP_RMCPP);
	
	//IPMI_SESSION_HANDLE* h = new IPMI_SESSION_HANDLE();
	//if(h == NULL)
	//	throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
	
	intf->ipmiCreateSession(d->handle);
	// Software Forge Inc. --- Start ------------------------------------------
	// Added cipher suite because the user name and password I was using did
	// not support the basic configuration
	// Convert the cipher enum to a string to set the ciphersuite
	char cipher[8];
	sprintf(cipher, "%d", d->cipher);
	intf->ipmiConfigure(d->handle, "ciphersuite", cipher);
	// Software Forge Inc. --- End --------------------------------------------
	intf->ipmiConfigure(d->handle, "targetip",d->address.c_str());
	intf->ipmiConfigure(d->handle, "username",d->user.c_str());
	intf->ipmiConfigure(d->handle, "password",d->password.c_str());
	
	iError = intf->ipmiConnect(d->handle);
	if(iError != SUCCESS) {
		throw new nmprk::nmprkException(NMPRK_FAILED_OPEN_REMOTE_CODE,NMPRK_FAILED_OPEN_REMOTE_MSG);
	}
	ret = true;
	

 }else{
	 throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_disconnectDevice(nmprk::ipmi::device* d) {
 bool ret = false;
 if(d != NULL) {
	 // Software Forge Inc. --- Start ------------------------------------------
	 // Removed check of d->intf, it was not necessary
	 // Software Forge Inc. --- End --------------------------------------------
	transport_plugin_interface* intf = transportPluginLoader.getPluginById(TP_KCS);
	int iError = intf->ipmiDisconnect(IPMI_SESSION_HANDLE_DEFAULT);
	if(iError == SUCCESS)
		ret = true;
 }else{
	 throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_disconnectDevice(nmprk::ipmi::device* d) {
 bool ret = false;
 if(d != NULL) {
	 // Software Forge Inc. --- Start ------------------------------------------
	 // Removed check of d->intf, it was not necessary
	 // Software Forge Inc. --- End --------------------------------------------
	transport_plugin_interface* intf = transportPluginLoader.getPluginById(TP_RMCPP);
	int iError = intf->ipmiDisconnect(d->handle);
	if(iError == SUCCESS)
		ret = true;
 }else{
	 throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
 return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_runIpmiCommand(nmprk::ipmi::device* d, nmprk::ipmi::commandReq_t* req, nmprk::ipmi::commandRsp_t* rsp) {
 bool ret = false;
 int  iError;
 if(d != NULL && req != NULL && rsp != NULL) {
  transport_plugin_interface* intf = transportPluginLoader.getPluginById(TP_KCS);
  
  if(req->data.size() < 2)
	  throw new nmprk::nmprkException(NMPRK_REQ_NOT_ENOUGH_ARGS_CODE,NMPRK_REQ_NOT_ENOUGH_ARGS_MSG);
  dcmi_req_t _req;
   // this is to support bridged commands
#ifndef WONTBE
  if(d->bridge != nmprk::ipmi::defaultBridge && d->transport != nmprk::ipmi::defaultTrans) {

    IpmiRequest ireq((unsigned char)nmprk::helper::hexStr2Int(req->data[0]),(unsigned char)nmprk::helper::hexStr2Int(req->data[1]),d->transport);
	
	BridgedIpmiRequest bireq(0x20,0x00,d->bridge,ireq,"",false);

	// Software Forge Inc. --- Start ------------------------------------------
	// Last two bytes were being stripped of each message
	//for(int i = 2; i < req->data.size() - 2 ; i++)
	for(int i = 2; i < req->data.size() ; i++)
	// Software Forge Inc. --- End --------------------------------------------
	 bireq.addDataByte(nmprk::helper::hexStr2Int(req->data[i]));

	 _req = bireq.getRequestStruct();
	 dcmi_rsp_t _rsp;
     memset((dcmi_rsp_t*)&_rsp,0x00, sizeof(struct dcmi_rsp_t));

     iError = intf->ipmiSendCmd(d->handle,&_req,&_rsp);
     rsp->rspCode = _rsp.compCode;
     if(iError == SUCCESS) {
	  ret = true;
	  IpmiRequest req(0x06, 0x33, 0x20, 0x00); 
	  bool timeout = false;
	  try
	  {
		struct timeval timeoutTime;
		timeoutTime.tv_sec = 5;
		timeoutTime.tv_usec = 0;

		dcmi_rsp_t theRsp;

		int iError = intf->ipmiRecvData(d->handle, timeoutTime, &timeout, &theRsp);
		if(iError != SUCCESS)
		{
			std::stringstream ss;
			ss << "ipmiRecvData returned an error: " << iError;
			throw new nmprk::nmprkException(NMPRK_BRIDGE_IPMIRECV_ERR_CODE,ss.str());
		}

		if(timeout)
		{
			throw new nmprk::nmprkException(NMPRK_BRIDGE_RECV_TIMEOUT_CODE,NMPRK_BRIDGE_RECV_TIMEOUT_MSG);
		}
		ret = true;
		// Software Forge Inc. --- Start ------------------------------------------
		// This was missing
		rsp->rspCode = theRsp.compCode;
		// Software Forge Inc. --- End --------------------------------------------
		for(int i = 0; i < theRsp.len ; i ++ )
			rsp->data.push_back(nmprk::helper::int2HexStr(theRsp.data[i]));

	  }
	  catch(std::exception& ex)
	  {
		std::stringstream ss;
		ss << "ipmiRecvData exception: " << ex.what();
		throw new nmprk::nmprkException(NMPRK_BRIDGE_RECV_DATA_ERR_CODE,ss.str());
	  } 

    }
  }else{
#endif
	memset((dcmi_req_t*)&_req,0x00,sizeof(struct dcmi_req_t));

	_req.netFun = (unsigned char)nmprk::helper::hexStr2Int(req->data[0]);
	_req.cmd    = (unsigned char)nmprk::helper::hexStr2Int(req->data[1]);
	_req.rsAddr = 0x20;
	_req.rsLun = 0x00;
	_req.len = req->data.size() - 2;

	// Software Forge Inc. --- Start ------------------------------------------
	// Last two bytes were being stripped of each message
	for(int i = 2; i < req->data.size(); i++)
	  _req.data[i-2] = nmprk::helper::hexStr2Int(req->data[i]);
	// Software Forge Inc. --- End --------------------------------------------

	dcmi_rsp_t _rsp;
	memset((dcmi_rsp_t*)&_rsp,0x00, sizeof(struct dcmi_rsp_t));

	iError = intf->ipmiSendCmd(IPMI_SESSION_HANDLE_DEFAULT,&_req,&_rsp);
	rsp->rspCode = _rsp.compCode;
	if(iError == SUCCESS) {
	  ret = true;
	  for(int i = 0; i < _rsp.len ; i ++ )
		  rsp->data.push_back(nmprk::helper::int2HexStr(_rsp.data[i]));
	}
#ifndef WONTBE
  }
#endif
 }else{
	 throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
  return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_runIpmiCommand(nmprk::ipmi::device* d, nmprk::ipmi::commandReq_t* req, nmprk::ipmi::commandRsp_t* rsp) {
  bool ret = false;
  int  iError;
 if(d != NULL && req != NULL && rsp != NULL) {
  transport_plugin_interface* intf = transportPluginLoader.getPluginById(TP_RMCPP);
  
  if(req->data.size() < 2)
	  throw new nmprk::nmprkException(NMPRK_REQ_NOT_ENOUGH_ARGS_CODE,NMPRK_REQ_NOT_ENOUGH_ARGS_MSG);
  
  dcmi_req_t _req;

  // this is to support bridged commands
  if(d->bridge != nmprk::ipmi::defaultBridge && d->transport != nmprk::ipmi::defaultTrans) {

    IpmiRequest ireq((unsigned char)nmprk::helper::hexStr2Int(req->data[0]),(unsigned char)nmprk::helper::hexStr2Int(req->data[1]),d->transport);
	
	BridgedIpmiRequest bireq(0x20,0x00,d->bridge,ireq,"",true);

	// Software Forge Inc. --- Start ------------------------------------------
	// Last two bytes were being stripped of each message
	//for(int i = 2; i < req->data.size() - 2 ; i++)
	for(int i = 2; i < req->data.size() ; i++)
	// Software Forge Inc. --- End --------------------------------------------
	 bireq.addDataByte(nmprk::helper::hexStr2Int(req->data[i]));

	 _req = bireq.getRequestStruct();
	 dcmi_rsp_t _rsp;
     memset((dcmi_rsp_t*)&_rsp,0x00, sizeof(struct dcmi_rsp_t));

     iError = intf->ipmiSendCmd(d->handle,&_req,&_rsp);
     rsp->rspCode = _rsp.compCode;
     if(iError == SUCCESS) {
	  ret = true;
	  IpmiRequest req(0x06, 0x33, 0x20, 0x00); 
	  bool timeout = false;
	  try
	  {
		struct timeval timeoutTime;
		timeoutTime.tv_sec = 5;
		timeoutTime.tv_usec = 0;

		dcmi_rsp_t theRsp;

		int iError = intf->ipmiRecvData(d->handle, timeoutTime, &timeout, &theRsp);
		if(iError != SUCCESS)
		{
			std::stringstream ss;
			ss << "ipmiRecvData returned an error: " << iError;
			throw new nmprk::nmprkException(NMPRK_BRIDGE_IPMIRECV_ERR_CODE,ss.str());
		}

		if(timeout)
		{
			throw new nmprk::nmprkException(NMPRK_BRIDGE_RECV_TIMEOUT_CODE,NMPRK_BRIDGE_RECV_TIMEOUT_MSG);
		}
		ret = true;
		// Software Forge Inc. --- Start ------------------------------------------
		// This was missing
		rsp->rspCode = theRsp.compCode;
		// Software Forge Inc. --- End --------------------------------------------
		for(int i = 0; i < theRsp.len ; i ++ )
			rsp->data.push_back(nmprk::helper::int2HexStr(theRsp.data[i]));

	  }
	  catch(std::exception& ex)
	  {
		std::stringstream ss;
		ss << "ipmiRecvData exception: " << ex.what();
		throw new nmprk::nmprkException(NMPRK_BRIDGE_RECV_DATA_ERR_CODE,ss.str());
	  } 

    }
  }else{
	memset((dcmi_req_t*)&_req,0x00,sizeof(struct dcmi_req_t));
	_req.netFun = (unsigned char)nmprk::helper::hexStr2Int(req->data[0]);
	_req.cmd    = (unsigned char)nmprk::helper::hexStr2Int(req->data[1]);
	_req.rsAddr = 0x20;
	_req.rsLun = 0x00;
	_req.len = req->data.size() - 2;

	// Software Forge Inc. --- Start ------------------------------------------
	// Last two bytes were being stripped of each message
	for(int i = 2; i < req->data.size(); i++)
	  _req.data[i-2] = nmprk::helper::hexStr2Int(req->data[i]);
	// Software Forge Inc. --- End --------------------------------------------

	dcmi_rsp_t _rsp;
    memset((dcmi_rsp_t*)&_rsp,0x00, sizeof(struct dcmi_rsp_t));

    iError = intf->ipmiSendCmd(d->handle,&_req,&_rsp);
    rsp->rspCode = _rsp.compCode;
    if(iError == SUCCESS) {
	 ret = true;
	 for(int i = 0; i < _rsp.len ; i ++ )
	  rsp->data.push_back(nmprk::helper::int2HexStr(_rsp.data[i]));
    }
  }
 }else{
	 throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 }
  return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_initSystemForLocal() {
  bool ret = false;

  return ret;
}

