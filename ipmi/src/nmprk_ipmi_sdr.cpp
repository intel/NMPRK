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
#include "../include/nmprk_ipmi_sdr.h"
#include "../include/nmprk_ipmi.h"
#include "../include/nmprk_defines.h"
#include "../include/nmprk_exception.h"
#include "../include/nmprk_errCodes.h"
#include "../include/nmprk_helper.h"

#include <string>
#include <time.h>

// 0x0a 0x20
nmprk::ipmi::repoInfo_t* nmprk::ipmi::sdr::getSdrInfo(nmprk::ipmi::device* d) 
{
  nmprk::ipmi::repoInfo_t* ret;
  if(d != NULL) 
  {
    ret = new nmprk::ipmi::repoInfo_t();
    if(ret != NULL) 
    {
      nmprk::ipmi::commandReq_t req;
      req.data.push_back(IPMI_CMD_STORAGE);
      req.data.push_back("0x20");

      nmprk::ipmi::commandRsp_t rsp;
      if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
      {
        if(rsp.rspCode != 0)
          ipmi_rspCodes(rsp.rspCode);
        else
        {
          ret->repoVersion = nmprk::helper::hexStr2Int(rsp.data[0]);
          std::string e(rsp.data[2]);
          e += rsp.data[1];
          ret->repoEntries = nmprk::helper::hexStr2Int(e);
          e = rsp.data[4];
          e += rsp.data[3];
          ret->repoFreeSpace = nmprk::helper::hexStr2Int(e);
 
          e = rsp.data[8];
          e += rsp.data[7];
          e += rsp.data[6];
          e += rsp.data[5];
          unsigned int un = nmprk::helper::hexStr2Int(e);
          time_t tt = (time_t)un;
          time_t* T = new time_t();
          *T = tt;
          struct tm* ti = NULL;
          ti = localtime(T);
          if(ti == NULL)
          {
            DBG_MSG("ti == NULL\n");
            throw new nmprk::nmprkException(NMPRK_CNVRT_TS_FAILED_CODE,NMPRK_CNVRT_TS_FAILED_MSG);
          }
          ret->mostRecentAddTS  = *ti;
          e = rsp.data[12];
          e += rsp.data[11];
          e += rsp.data[10];
          e += rsp.data[9];
          un = nmprk::helper::hexStr2Int(e);
          tt = (time_t)un;
          *T = tt;
          ti = localtime(T);
          if(ti == NULL){
            DBG_MSG("ti == NULL\n");
            throw new nmprk::nmprkException(NMPRK_CNVRT_TS_FAILED_CODE,NMPRK_CNVRT_TS_FAILED_MSG);
          }
          ret->mostRecentDelTS = *ti;      

          std::string bits = nmprk::helper::int2BinStr(nmprk::helper::hexStr2Int(rsp.data[13]));
          ret->getAllocInfoSup = (bits[0] == '1') ? true : false;
          ret->reserveSup      = (bits[1] == '1') ? true : false;
          ret->parAddSup       = (bits[2] == '1') ? true : false;
          ret->delSup          = (bits[3] == '1') ? true : false;
          if(bits[5] == '0'){
            ret->nonmodalSupported = false;
            ret->modalSupported = (bits[6] == '1') ? true : false;
          }else{
            ret->nonmodalSupported = true;
            ret->modalSupported = (bits[6] == '1') ? true : false;
          }
        }
      }else{
        delete ret;
        ret = NULL;
      }
    }else{
      throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
    }
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }

  return ret;
}

// 0x0a 0x23 0x00 0x00 recordLSB recordMSB offset(0) 0xff
nmprk::ipmi::record_t*   nmprk::ipmi::sdr::getSdrRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId) 
{
  nmprk::ipmi::record_t* ret = NULL;
  nmprk::ipmi::commandReq_t req;
  nmprk::ipmi::commandRsp_t rsp;
  if(d != NULL && recordId != NULL) 
  {
	  ret = new nmprk::ipmi::record_t();
	  if(ret != NULL) 
    {
	    setupSdrCall(&req, &rsp, 0x00, 0x00, recordId->lsb, recordId->msb, 0x00, 0xFF);
	    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
      {	
		    if(rsp.rspCode == 0xCA) // then need to do partial reads
        {
    		  // Reserve SDR repository command
	  	    req.data.clear();
		      req.data.push_back(IPMI_CMD_STORAGE);
  	  	  req.data.push_back("0x22");  // Reserve SDR Repository command
	    	  //rsp.data.clear();
    		  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
          {
			      if(rsp.rspCode != 0x00){ // there was an error
              ipmi_rspCodes(rsp.rspCode);
			      }else{ // command was successful, so get record length to read the rest of the record
      			  // Read SDR record header to get the length
			        setupSdrCall(&req, &rsp, nmprk::helper::hexStr2Int(rsp.data[0]), nmprk::helper::hexStr2Int(rsp.data[1]), recordId->lsb, recordId->msb, 0x00, 0x05);
      			  if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
              {
			       	  if(rsp.rspCode != 0x00){ // there was an error
                  ipmi_rspCodes(rsp.rspCode);
				        }else{ // it was good so read the whole record
        				  int num_bytes = 0;
				          // Save the length
				          int length = nmprk::helper::hexStr2Int(rsp.data[6]);
  				        // Save next record info
				          ret->nextRecord.lsb = nmprk::helper::hexStr2Int(rsp.data[0]);
				          ret->nextRecord.msb = nmprk::helper::hexStr2Int(rsp.data[1]); 
                  // Copy the header (5 bytes) and skip the next record pointer.
        				  int offset = 5;  
	 			          for(int i = 0; i < offset; i++) 
          					ret->data[i] = nmprk::helper::hexStr2Int(rsp.data[i+2]);
				          while (length > 0) // get the rest of the record.
                  {
                    if(length > 0x20)
                      num_bytes = 0x20;
                    else
                      num_bytes = length;
				          	// Reserve SDR repository command
					          req.data.clear();
					          req.data.push_back(IPMI_CMD_STORAGE);
					          req.data.push_back("0x22");  // Reserve SDR Repository command
					          rsp.data.clear();
					          if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
                    {
					            if(rsp.rspCode != 0x00) 
                        ipmi_rspCodes(rsp.rspCode);
					          }
  					        setupSdrCall(&req, &rsp, nmprk::helper::hexStr2Int(rsp.data[0]), nmprk::helper::hexStr2Int(rsp.data[1]), recordId->lsb, recordId->msb, offset, num_bytes);
	  				        if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
                    {
			  		          if(rsp.rspCode != 0x00){ // there was an error
                        ipmi_rspCodes(rsp.rspCode);
					            }else{
          					  	// save the data read
						            for(int j = 0; j < num_bytes; j++)
					        	      ret->data[(j+offset)] = nmprk::helper::hexStr2Int(rsp.data[2+j]);
            						// update next offset & length
	  				          	offset += num_bytes;
            						length -= num_bytes;
			  	        	  }
          					}else{
				          	  delete ret;
        		  			  ret = NULL;
				          	  return ret;
        				  	}
			      	    } // end while loop
      				    ret->len = offset;
				          return ret;
        				}
			        }else{
				        delete ret;
				        ret = NULL;
        				return ret;
		  	      }							
		      	}
    		  }else{	
			      delete ret;
      			ret = NULL;
		      	return ret;
    		  }
    		}else if(rsp.rspCode != 0x00) {
          ipmi_rspCodes(rsp.rspCode);
    		}
		    ret->nextRecord.lsb = nmprk::helper::hexStr2Int(rsp.data[0]);
    		ret->nextRecord.msb = nmprk::helper::hexStr2Int(rsp.data[1]);   
		    for(unsigned int i = 2; i < rsp.data.size(); i++)
		      ret->data[(i-2)] = nmprk::helper::hexStr2Int(rsp.data[i]);
    		ret->len = rsp.data.size() - 2;
	    }else{
		    delete ret;
    		ret = NULL;
	    }
	  }else{
	    throw new nmprk::nmprkException(NMPRK_FAILED_ALLOC_CODE,NMPRK_FAILED_ALLOC_MSG);
	  }
  }else{
	  throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}

void nmprk::ipmi::sdr::setupSdrCall(nmprk::ipmi::commandReq_t * req, nmprk::ipmi::commandRsp_t * rsp, int a, int b, int c, int d, int e, int f)
{
	req->data.clear();
	req->data.push_back(IPMI_CMD_STORAGE);
	req->data.push_back("0x23");
	req->data.push_back(nmprk::helper::int2HexStr(a));
	req->data.push_back(nmprk::helper::int2HexStr(b));
	req->data.push_back(nmprk::helper::int2HexStr(c));
	req->data.push_back(nmprk::helper::int2HexStr(d));
	req->data.push_back(nmprk::helper::int2HexStr(e));
	req->data.push_back(nmprk::helper::int2HexStr(f));
	rsp->data.clear();
	return;
}

// 0x0a 0x24 n+1
bool nmprk::ipmi::sdr::addSdrRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId,nmprk::ipmi::record_t* record) 
{
  bool ret = false;
  if(d != NULL && record != NULL && record->data != NULL && recordId != NULL) 
  {
    nmprk::ipmi::commandReq_t req;
    req.data.push_back(IPMI_CMD_STORAGE);
    req.data.push_back("0x24");                 // Add Sdr
    for(unsigned int i=0; i < record->len; i++) // bytes n+1 = data
      req.data.push_back(nmprk::helper::int2HexStr(record->data[i]));
 
    nmprk::ipmi::commandRsp_t rsp;
    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
    { // was successful
      if(rsp.rspCode > 0)
      {
        ipmi_rspCodes(rsp.rspCode);
      }
      else
      {
        recordId->lsb = nmprk::helper::hexStr2Int(rsp.data[0]);
        recordId->msb = nmprk::helper::hexStr2Int(rsp.data[1]);
        ret = true;
      }
    }else{
      ret = false;
    }
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}

// 0x0a 0x26 0x00 0x00 recorIDLSB recordIDMSB
bool nmprk::ipmi::sdr::delSdrRecord(nmprk::ipmi::device* d,nmprk::ipmi::address_t* recordId) 
{
  bool ret = false;
  if(d != NULL && recordId != NULL) 
  {
    nmprk::ipmi::commandReq_t req;
    nmprk::ipmi::commandRsp_t rsp;
    // Reserve SDR repository command
		req.data.push_back(IPMI_CMD_STORAGE);
  	req.data.push_back("0x22");  // Reserve SDR Repository command

    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
    {
		  if(rsp.rspCode != 0x00){ // there was an error
        ipmi_rspCodes(rsp.rspCode);
		  }else{ // command was successful, so get record length to read the rest of the record
    	  // Read SDR record header to get the length
  	    req.data.clear();
        req.data.push_back(IPMI_CMD_STORAGE);
        req.data.push_back("0x26");      // delete SDR
        req.data.push_back(rsp.data[0]); // reservation id lsb
        req.data.push_back(rsp.data[1]); // reservation id msb
        req.data.push_back(nmprk::helper::int2HexStr(recordId->lsb));
        req.data.push_back(nmprk::helper::int2HexStr(recordId->msb));
        rsp.data.clear();
        if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
        {
  		    if(rsp.rspCode != 0x00) // there was an error
            ipmi_rspCodes(rsp.rspCode);
    		  else // command was successful
            ret = true;
        }
      }
    }  
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}

// 0x0a 0x27 0x00 0x00 0x43 0x4c 0x52 0xaa
bool nmprk::ipmi::sdr::clearSdr(nmprk::ipmi::device* d) 
{
  bool ret = false;
  if(d != NULL) 
  {
    nmprk::ipmi::commandReq_t req;
    nmprk::ipmi::commandRsp_t rsp;
    // Reserve SDR repository command
		req.data.push_back(IPMI_CMD_STORAGE);
  	req.data.push_back("0x22");  // Reserve SDR Repository command

    if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp)) 
    {
		  if(rsp.rspCode != 0x00){ // there was an error
        ipmi_rspCodes(rsp.rspCode);
		  }else{ // command was successful, so get record length to read the rest of the record
    	  // Read SDR record header to get the length
  	    req.data.clear();
        req.data.push_back(IPMI_CMD_STORAGE);
        req.data.push_back("0x27"); // Clear SDR
        req.data.push_back(rsp.data[0]); // reservation id lsb
        req.data.push_back(rsp.data[1]); // reservation id msb
        req.data.push_back("0x43"); // 'C'
        req.data.push_back("0x4c"); // 'L'
        req.data.push_back("0x52"); // 'R'
        req.data.push_back("0xaa"); // initiate erase
        rsp.data.clear();
        if(nmprk::ipmi::runIpmiCommand(d,&req,&rsp) == true) 
        {
  		    if(rsp.rspCode != 0x00) // there was an error
            ipmi_rspCodes(rsp.rspCode);
    		  else // command was successful
            ret = true;
        }
      }
    }  
  }else{
    throw new nmprk::nmprkException(NMPRK_NULL_CODE,NMPRK_NULL_MSG);
  }
  return ret;
}

