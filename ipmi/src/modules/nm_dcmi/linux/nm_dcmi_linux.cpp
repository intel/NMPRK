// This file has a lot of code pieces taken from ipmitool 
// so that our code can call their code
// so two licenses just incase
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
/*
 * Copyright (c) 2004 Sun Microsystems, Inc.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistribution of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistribution in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Sun Microsystems, Inc. or the names of
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * This software is provided "AS IS," without a warranty of any kind.
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * SUN MICROSYSTEMS, INC. ("SUN") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */


/*
 * interface routines between ipmitool and the bmc kernel driver
 */

#include <stdio.h>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include "../nm_dcmi.h"
#include <nmprk_exception.h>
#include <nmprk_errCodes.h>
#include <nmprk_defines.h>
#include <nmprk_helper.h>
#include "ipmitool_include/ipmitool_ipmi_intf.h"
#include "ipmitool_include/ipmitool_open.h"
#include "ipmitool_include/ipmitool_lan.h"
#include "ipmitool_include/ipmitool_helper.h"
#include "open.cpp"
#include "lan.c"

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_connectToDevice(nmprk::ipmi::device* d) 
{
  bool ret = false;
  if(d != NULL) 
  {
    struct ipmi_intf* intf = new struct ipmi_intf();
    if(intf == NULL)
      throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
    else 
    {
      //intf->name = "nmprk";
      //intf->desc = "nmprk interface to use with ipmitool open module";
      intf->open = ipmi_openipmi_open;
      intf->close = ipmi_openipmi_close;
      intf->sendrecv = ipmi_openipmi_send_cmd;
      intf->my_addr = 0x20;
      intf->target_addr = intf->my_addr;
      intf->target_lun = 0x00;
      intf->devnum = 0;
      if(d->transport != nmprk::ipmi::defaultTrans) 
        intf->target_addr = d->transport;
      if(d->bridge    != nmprk::ipmi::defaultBridge)
        intf->target_channel = d->bridge;
      d->intf = intf;
      int fd = intf->open(intf);
      if(fd < 0) 
      {
        switch(fd) {
        case -1:
          throw new nmprk::nmprkException(NMPRK_NM_DCMI_NO_DEV_CODE,NMPRK_NM_DCMI_NO_DEV_MSG);
          break;
        case -2:
          throw new nmprk::nmprkException(NMPRK_NM_DCMI_NO_EVENT_RCV_CODE,NMPRK_NM_DCMI_NO_EVENT_RCV_MSG);
          break;
        case -3:
          throw new nmprk::nmprkException( NMPRK_NM_DCMI_NOSET_IPMB_ADDY_CODE ,NMPRK_NM_DCMI_NOSET_IPMB_ADDY_MSG);
          break;
        }
      }
      ret = true; 
    }
  }else 
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG); 
  return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_connectToDevice(nmprk::ipmi::device* d) 
{
  bool ret = false;
  if(d != NULL) 
  {
    struct ipmi_intf* intf = new struct ipmi_intf();
    if(intf == NULL)
      throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
    else
    {
      //intf->name =           "lan";
      //intf->desc =           "IPMI v1.5 LAN Interface",
      intf->setup =          ipmi_lan_setup;
      intf->open =           ipmi_lan_open;
      intf->close =          ipmi_lan_close;
      intf->sendrecv =       ipmi_lan_send_cmd;
      intf->sendrsp =        ipmi_lan_send_rsp;
      intf->recv_sol =       ipmi_lan_recv_sol;
      intf->send_sol =       ipmi_lan_send_sol;
      intf->keepalive =      ipmi_lan_keepalive;
      intf->target_addr =    IPMI_BMC_SLAVE_ADDR; 
      intf->devnum = 0;
      // Software Forge Inc. --- Start ------------------------------------------
      // This code to set the bridged address and channel was not here, but it
      // was in the inband version. Once this is added bridging works
      if(d->transport != nmprk::ipmi::defaultTrans) 
        intf->target_addr = d->transport;
      if(d->bridge    != nmprk::ipmi::defaultBridge)
        intf->target_channel = d->bridge;
      // Software Forge Inc. --- End --------------------------------------------
      d->intf = intf;
      intf->setup(intf);

      if(intf->session != NULL) 
      {
#ifdef DEBUG_PRINTING
        fprintf(stderr,"session != NULL\n");
#endif
        // set some interface variables
        // set the hostname
        memset(intf->session->hostname, 0, 16);
        memcpy(intf->session->hostname, d->address.c_str(), __min(strlen(d->address.c_str()), 64));
        // set the user
        memset(intf->session->username, 0, 17);
        memcpy(intf->session->username, d->user.c_str(), __min(strlen(d->user.c_str()), 16));
        // set the password
        memset(intf->session->authcode, 0, IPMI_AUTHCODE_BUFFER_SIZE);
        intf->session->password = 1;
        memcpy(intf->session->authcode, d->password.c_str(),__min(strlen(d->password.c_str()), IPMI_AUTHCODE_BUFFER_SIZE));  
        // set ADMIN privledge level
        intf->session->privlvl = (uint8_t) IPMI_SESSION_PRIV_ADMIN;
        // set lookup bit
        intf->session->v2_data.lookupbit = (uint8_t) 0x10; // use name-only lookup by default
        // SOL escape character
        intf->session->sol_escape_char = '~'; // SOL_ESCAPE_CHARACTER_DEFAULT;
        // Software Forge Inc. --- Start ------------------------------------------
        // set cypher suite
        intf->session->cipher_suite_id = (uint8_t)d->cipher; /* See table 22-19 of the IPMIv2 spec */
        // Software Forge Inc. --- End --------------------------------------------
        // RETURN HERE 
      
        int fd = intf->open(intf);
        if(fd < 0) 
        {
          switch(fd) {
          case -1:
            throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
            break;
          case -2:
            throw new nmprk::nmprkException(NMPRK_NM_DCMI_NO_EVENT_RCV_CODE,NMPRK_NM_DCMI_NO_EVENT_RCV_MSG);
            break;
          case -3:
            throw new nmprk::nmprkException( NMPRK_NM_DCMI_NOSET_IPMB_ADDY_CODE ,NMPRK_NM_DCMI_NOSET_IPMB_ADDY_MSG);
            break;
          }
        }
        ret = true;
      }
    }
  }else 
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_disconnectDevice(nmprk::ipmi::device* d) 
{
  bool ret = false;

  if(d != NULL) 
  {
    if(d->intf != NULL) 
    {
      struct ipmi_intf* intf = (struct ipmi_intf*)d->intf;
      intf->close(intf);
    }
    ret = true;
 }else 
   throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);

 return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_disconnectDevice(nmprk::ipmi::device* d) 
{
  bool ret = false;

  if(d != NULL) 
  {
    if(d->intf != NULL) 
    {
      struct ipmi_intf* intf = (struct ipmi_intf*)d->intf;
      intf->close(intf);
    }
    ret = true;
  }else 
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);

  return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_local_runIpmiCommand(nmprk::ipmi::device* d, nmprk::ipmi::commandReq_t* req, nmprk::ipmi::commandRsp_t* rsp) 
{
  bool ret = false;
  if(d != NULL && req != NULL && rsp != NULL) 
  {
    struct ipmi_intf* intf = (struct ipmi_intf*) d->intf;
    struct ipmi_rs* _rsp;
    struct ipmi_rq  _req;
    uint8_t netfn, cmd, lun;
    int i;
    uint8_t data[256];
    lun = intf->target_lun;
    netfn =(uint8_t) nmprk::helper::hexStr2Int(req->data[0]);
    cmd =  (uint8_t) nmprk::helper::hexStr2Int(req->data[1]);
    memset(data, 0, sizeof(data));
    memset(&_req, 0, sizeof(_req));
 
    _req.msg.netfn = netfn;
    _req.msg.lun = lun;
    _req.msg.cmd = cmd;
    _req.msg.data = data;
    int l = req->data.size();  
    for(int i=2;i<l;i++) 
    {
      uint8_t val = (uint8_t) nmprk::helper::hexStr2Int(req->data[i]);
      _req.msg.data[i-2] = val;
      _req.msg.data_len++;
    }

#ifdef DEBUG_PRINTING
    fprintf(stderr,"DEBUG: RAW REQ (channel=0x%x netfn=0x%x lun=0x%x cmd=0x%x data_len=%d)",intf->target_channel & 0x0f, _req.msg.netfn,_req.msg.lun , _req.msg.cmd, _req.msg.data_len);  
    for(int i=0;i<_req.msg.data_len;i++) 
    {
      if (((i%16) == 0) && (i != 0))
  	    fprintf(stderr, "\r\n");
      fprintf(stderr, " %2.2x", _req.msg.data[i]);
    }
#endif
    _rsp = intf->sendrecv(intf,&_req);

    if(_rsp == NULL) // we should never hit this code because if rsp == NULL we should have already thwo a exception
    {
#ifdef DEBUG_PRINTING
      fprintf(stderr, "Unable to send RAW command (channel=0x%x netfn=0x%x lun=0x%x cmd=0x%x rsp=0x%x):",
			  intf->target_channel & 0x0f, _req.msg.netfn, _req.msg.lun, _req.msg.cmd, _rsp->ccode);   
#endif
      ;  
    }else{

      for(int i=0;i<_rsp->data_len;i++) {
        rsp->data.push_back(nmprk::helper::int2HexStr(_rsp->data[i])); 
      }
      rsp->rspCode = _rsp->ccode;
#ifdef DEBUG_PRINTING
      fprintf(stderr,"DEBUG: rspCode == %u, ccode == %u\n",rsp->rspCode,_rsp->ccode);
#endif
      ret = true;
    }
  }else 
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 
  return ret;
}

bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_remote_runIpmiCommand(nmprk::ipmi::device* d, nmprk::ipmi::commandReq_t* req, nmprk::ipmi::commandRsp_t* rsp) 
{
  bool ret = false;

  if(d != NULL && req != NULL && rsp != NULL) 
  {
    struct ipmi_intf* intf = (struct ipmi_intf*) d->intf;
    struct ipmi_rs* _rsp;
    struct ipmi_rq  _req;
    uint8_t netfn, cmd, lun;
    int i;
    uint8_t data[256];
    lun = intf->target_lun;
    netfn =(uint8_t) nmprk::helper::hexStr2Int(req->data[0]);
    cmd =  (uint8_t) nmprk::helper::hexStr2Int(req->data[1]);

    memset(data, 0, sizeof(data));
    memset(&_req, 0, sizeof(_req));

    _req.msg.netfn = netfn;
    _req.msg.lun = lun;
    _req.msg.cmd = cmd;
    _req.msg.data = data;
    int l = req->data.size();
    for(int i=2;i<l;i++) {
    uint8_t val = (uint8_t) nmprk::helper::hexStr2Int(req->data[i]);
    _req.msg.data[i-2] = val;
    _req.msg.data_len++;
  }

  //fprintf(stderr,"DEBUG: RAW REQ (channel=0x%x netfn=0x%x lun=0x%x cmd=0x%x data_len=%d)",intf->target_channel & 0x0f, _req.msg.netfn,_req.msg.lun , _req.msg.cmd, _req.msg.data_len);
  //for(int i=0;i<_req.msg.data_len;i++) {
  // if (((i%16) == 0) && (i != 0))
  //    fprintf(stderr, "\r\n");
  // fprintf(stderr, " %2.2x", _req.msg.data[i]);
  //}

  _rsp = intf->sendrecv(intf,&_req);

  if(_rsp == NULL) {  // we should never hit this code because if rsp == NULL we should have already thwo a exception
#ifdef DEBUG_PRINTING
   fprintf(stderr, "Unable to send RAW command (channel=0x%x netfn=0x%x lun=0x%x cmd=0x%x rsp=0x%x):",
                        intf->target_channel & 0x0f, _req.msg.netfn, _req.msg.lun, _req.msg.cmd, _rsp->ccode);
#endif
   ;
  }else{

    for(int i=0;i<_rsp->data_len;i++) {

     rsp->data.push_back(nmprk::helper::int2HexStr(_rsp->data[i]));
    }
    rsp->rspCode = _rsp->ccode;
#ifdef DEBUG_PRINTING
    fprintf(stderr,"DEBUG: rspCode == %u, ccode == %u\n",rsp->rspCode,_rsp->ccode);
#endif
    ret = true;
  }
  }else 
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);

  return ret;
}


bool fileExeCheck(std::string fileName) 
{
  return (access(fileName.c_str(), F_OK|X_OK) == 0) ? true : false;
}

bool runInitScript(std::string fileName) 
{
  bool ret = false;
  fileName += " start";
  std::cout << "Running : " << fileName << std::endl;
  FILE * f = popen( fileName.c_str(), "r" );
  if ( f != 0 ) 
  {
    const int BUFSIZE = 1000;
    char buf[ BUFSIZE ];
    while( fgets( buf, BUFSIZE,  f ) ) 
    {
      std::string line(buf);
      size_t found = line.find("OK");
      if(found!=std::string::npos)
        ret = true;
    }
    pclose( f );
  }
  return ret;
}

// Pretty basic function, 
// checks to make sure the system
// has the ipmi sub system installed 
// by either checking /etc/init.d/ipmi (red hat derived systems)
// or /etc/rc.d/rc.ipmi (! red hat derived)
// and then starting the script and checking for the output
// 
bool  nmprk::ipmi::modules::nm_dcmi::nm_dcmi_initSystemForLocal() 
{
  bool ret = false;;
  char* files[] =  {"/etc/init.d/ipmi", "/etc/rc.d/rc.ipmi",NULL};
  for(int i =0;files[i] != NULL; i++) 
  {
    if(fileExeCheck(files[i]))
    {
      if(runInitScript(files[i])) 
      {
        ret = true;
        break;
      }
    }
  }
  return ret;
}

