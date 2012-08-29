/*
 * Copyright (c) 2003 Sun Microsystems, Inc.  All Rights Reserved.
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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ipmitool_include/ipmitool_ipmi.h"
#include "ipmitool_include/ipmitool_ipmi_intf.h"
#include "ipmitool_include/ipmitool_ipmi_picmg.h"
#include "ipmitool_include/ipmitool_open.h"

// we have to include some things so that this 
// code can throw exceptions
#include <nmprk_exception.h>
#include <nmprk_errCodes.h>

static int ipmi_openipmi_open(struct ipmi_intf* intf) {
	int i = 0;
	struct ipmi_rq req;
	struct ipmi_rs *rsp;
	char msg_data;
	char ipmi_dev[16];
	char ipmi_devfs[16];
	char ipmi_devfs2[16];
	int devnum = 0;

	devnum = intf->devnum;

	sprintf(ipmi_dev, "/dev/ipmi%d", devnum);
	sprintf(ipmi_devfs, "/dev/ipmi/%d", devnum);
	sprintf(ipmi_devfs2, "/dev/ipmidev/%d", devnum);
	
	intf->fd = open(ipmi_dev, O_RDWR);

	if (intf->fd < 0) {
		intf->fd = open(ipmi_devfs, O_RDWR);
		if (intf->fd < 0) {
			intf->fd = open(ipmi_devfs2, O_RDWR);
		}
		if (intf->fd < 0) {
                        throw new nmprk::nmprkException(NMPRK_NM_DCMI_NO_DEV_CODE,NMPRK_NM_DCMI_NO_DEV_MSG);
			return -1;
		}
	}

	if (ioctl(intf->fd, IPMICTL_SET_GETS_EVENTS_CMD, &i) < 0) {
                throw new nmprk::nmprkException(NMPRK_NM_DCMI_NO_EVENT_RCV_CODE,NMPRK_NM_DCMI_NO_EVENT_RCV_MSG);
		return -2;
	}

	if (intf->my_addr != 0) {
		unsigned int a = intf->my_addr;
		if (ioctl(intf->fd, IPMICTL_SET_MY_ADDRESS_CMD, &a) < 0) {
                        throw new nmprk::nmprkException( NMPRK_NM_DCMI_NOSET_IPMB_ADDY_CODE ,NMPRK_NM_DCMI_NOSET_IPMB_ADDY_MSG);
			return -3;
		}
#ifdef  DEBUG_PRINTING
		fprintf(stderr,"DEBUG: Set IPMB address to 0x%x\n",intf->my_addr);
#endif
	}
	intf->opened = 1;

	/* Check if PICMG extension is available to use the function GetDeviceLocator
	 * to retreive i2c address PICMG hack to set right IPMB address, 
	 * If extension is not supported, should not give any problems
         *  PICMG Extension Version 2.0 (PICMG 3.0 Revision 1.0 ATCA) to
         *  PICMG Extension Version 2.3 (PICMG 3.0 Revision 3.0 ATCA)
	 */
	if (intf->my_addr == IPMI_BMC_SLAVE_ADDR) {
      
          /* First, check if PICMG extension is available and supported */
               unsigned char version_accepted = 0;
#ifdef  DEBUG_PRINTING
		fprintf(stderr,"DEBUG: Running PICMG GetDeviceLocator\n" );
#endif
		memset(&req, 0, sizeof(req));
		req.msg.netfn = IPMI_NETFN_PICMG;
		req.msg.cmd = PICMG_GET_PICMG_PROPERTIES_CMD;             
		msg_data    = 0x00;                                       
		req.msg.data = (uint8_t*)  &msg_data; 
		req.msg.data_len = 1;
		msg_data = 0;

		rsp = intf->sendrecv(intf, &req);
		if (rsp && !rsp->ccode) {
			if((rsp->data[0] == 0) && ((rsp->data[1] == 0x02) ||
  			   (rsp->data[1] == 0x12) || (rsp->data[1] == 0x22) ||
 			   (rsp->data[1] == 0x32))){
		      version_accepted = 1;
#ifdef  DEBUG_PRINTING		  
		    fprintf(stderr,"DEBUG: Discovered PICMG Extension %d.%d\n", (rsp->data[1] & 0x0f),(rsp->data[1] >> 4)  );
#endif
		   }
		}
		
		if(version_accepted == 1){
#ifdef  DEBUG_PRINTING
			fprintf(stderr,"DEBUG: Running PICMG GetDeviceLocator\n" );
#endif
			memset(&req, 0, sizeof(req));
			req.msg.netfn = IPMI_NETFN_PICMG;
			req.msg.cmd = PICMG_GET_ADDRESS_INFO_CMD;
			msg_data    = 0x00;
			req.msg.data = (uint8_t*) &msg_data; 
			req.msg.data_len = 1;   
			msg_data = 0;

		   rsp = intf->sendrecv(intf, &req);
		   if (rsp && !rsp->ccode) {
			   intf->my_addr = rsp->data[2];
			   intf->target_addr = intf->my_addr;
#ifdef  DEBUG_PRINTING
			   fprintf(stderr,"DEBUG: Discovered IPMB address = 0x%x\n", intf->my_addr);
#endif
		   }
      }
      else{
#ifdef  DEBUG_PRINTING
         fprintf(stderr,"DEBUG: No PICMG Extenstion discovered, keeping IPMB address 0x20\n");
#endif
      }
     }

 return intf->fd;
}

static void ipmi_openipmi_close(struct ipmi_intf* intf) {
	if (intf->fd >= 0) {
		close(intf->fd);
		intf->fd = -1;
	}

	intf->opened = 0;
}

static struct ipmi_rs* ipmi_openipmi_send_cmd(struct ipmi_intf* intf, struct ipmi_rq* req) {

 struct ipmi_recv recv;
 struct ipmi_addr addr;

 struct ipmi_system_interface_addr bmc_addr;
 bmc_addr.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
 bmc_addr.channel = IPMI_BMC_CHANNEL;

 struct ipmi_ipmb_addr ipmb_addr;
 ipmb_addr.addr_type = IPMI_IPMB_ADDR_TYPE;
 ipmb_addr.channel = intf->target_channel & 0x0f;

 struct ipmi_req _req;
 static struct ipmi_rs rsp;
 static int curr_seq = 0;
 fd_set rset;

 if (intf == NULL || req == NULL)
  throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
 if (intf->opened == 0 && intf->open != NULL)
  intf->open(intf);
 

 /*
  * setup and send message
  */

 memset(&_req, 0, sizeof(struct ipmi_req));
 
 if (intf->target_addr != 0 && intf->target_addr != intf->my_addr) {
  /* use IPMB address if needed */
  ipmb_addr.slave_addr = intf->target_addr;
  ipmb_addr.lun = req->msg.lun;
  //fprintf(stderr, "DEBUG: Sending request to IPMB target @ 0x%x\n", intf->target_addr);
  _req.addr = (unsigned char *) &ipmb_addr;
  _req.addr_len = sizeof(ipmb_addr);
 }else {
   /* otherwise use system interface */
  // fprintf(stderr,"Sending request to System Interface\n");
   bmc_addr.lun = req->msg.lun;
   _req.addr = (unsigned char *) &bmc_addr;
   _req.addr_len = sizeof(bmc_addr);
 }
 
 _req.msgid = curr_seq++;
 _req.msg.netfn = req->msg.netfn;
 _req.msg.cmd = req->msg.cmd;
 _req.msg.data = req->msg.data;
 _req.msg.data_len = req->msg.data_len;

 if (ioctl(intf->fd, IPMICTL_SEND_COMMAND, &_req) < 0) {
  fprintf(stderr,"Unable to send command");
  throw new nmprk::nmprkException(NMPRK_NM_DCMI_UNABLE_SND_CMD_CODE,NMPRK_NM_DCMI_UNABLE_SND_CMD_MSG);
  return NULL;
 }

 /*
  * wait for and retrieve response
  */
  
  FD_ZERO(&rset);
  FD_SET(intf->fd, &rset);

  if(select(intf->fd+1, &rset, NULL, NULL, NULL) < 0) {
   fprintf(stderr, "I/O Error");
   throw new nmprk::nmprkException(NMPRK_NM_DCMI_IO_ERR_SND_CMD_CODE,NMPRK_NM_DCMI_IO_ERR_SND_CMD_MSG);
   return NULL;
  }
  if(FD_ISSET(intf->fd, &rset) == 0) {
   fprintf(stderr, "No data available");
   throw new nmprk::nmprkException(NMPRK_NM_DCMI_NO_DATA_RSP_CODE,NMPRK_NM_DCMI_NO_DATA_RSP_MSG);
   return NULL;
  }
 
  recv.addr = (unsigned char *) &addr;
  recv.addr_len = sizeof(addr);
  recv.msg.data = rsp.data;
  recv.msg.data_len = sizeof(rsp.data);
 
  /* get data */
  if(ioctl(intf->fd, IPMICTL_RECEIVE_MSG_TRUNC, &recv) < 0) {
   fprintf(stderr, "Error receiving message");
    if(errno != EMSGSIZE)
     return NULL;
  }

  //fprintf(stderr, "Got message:");
  //fprintf(stderr, "  type      = %d\n", recv.recv_type);
  //fprintf(stderr, "  channel   = 0x%x\n", addr.channel);
  //fprintf(stderr, "  msgid     = %ld\n", recv.msgid);
  //fprintf(stderr, "  netfn     = 0x%x\n", recv.msg.netfn);
  //fprintf(stderr, "  cmd       = 0x%x\n", recv.msg.cmd);
  //if(recv.msg.data && recv.msg.data_len) {
  // fprintf(stderr, "  data_len  = %d\n", recv.msg.data_len);
  //}

  rsp.ccode = recv.msg.data[0];
  rsp.data_len = recv.msg.data_len - 1;
  
  /* save response data for caller */
  if(rsp.ccode == 0 && rsp.data_len > 0) {
   memmove(rsp.data, rsp.data + 1, rsp.data_len);
   rsp.data[recv.msg.data_len] = 0;
  }

  return &rsp;
}

