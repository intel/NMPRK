/***************************************************************************
 * Copyright 2012 Intel Corporation                                        *
 * Licensed under the Apache License, Version 2.0 (the "License");         *
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
 * Updates:							          *
 * 4/30: prep for initial external release                                *
 **************************************************************************/
#include <iostream>

#include <nmprk_ipmi.h>
#include <nmprk_ipmi_sel.h>
#include <nmprk_ipmi_sdr.h>
#include <nmprk_ipmi_fru.h>
#include <nmprk_ipmi_global.h>
#include <nmprk_defines.h>
#include <nmprk_exception.h>
#include <nmprk_helper.h>
#include <nmprk_errCodes.h>

#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <vector>


#define TEST_CHECK_BOOL(x) ( (x == true) ? "Passed" : "Failed" )
#define TEST_CHECK_VOID(x) ( (x != NULL) ? "Passed" : "Failed" )
#define TEST_VOID(x)       ( (x == NULL) ? "Passed" : "Failed" )

int main(int argc, char** argv) {
try {
  nmprk::ipmi::device d;
  d.transport = nmprk::ipmi::defaultTrans;
  d.bridge =    nmprk::ipmi::defaultBridge;
  d.cipher = nmprk::ipmi::cipherSuite_3;
  d.type = nmprk::ipmi::device_nm;

#if 0 // If you want to test in debugger without command line, set to 1
  d.address = "10.20.0.41";
  //d.address = "local";
  d.user = "ipmitest";
  d.password = "password";
  //d.transport = 0x2c;
  //d.bridge = 0x06;
  d.type = nmprk::ipmi::device_nm;
#else
  if(argc < 3) {
USAGE:  
    std::cout << "Invalid arguments\n";
    std::cout << "Usage:\n";
    std::cout << argv[0] << " [ -dcmi -dnm -nm ] [ -local | -remote host username pw ]\n";
    return -1;
  }else{
 
  std::string mode(argv[1]);
  size_t found = mode.find("-dcmi");
  if(found != std::string::npos)
   d.type = nmprk::ipmi::device_dcmi;
  else if((found = mode.find("-dnm")) != std::string::npos)
   d.type = nmprk::ipmi::device_dnm;
  else if((found = mode.find("-nm")) != std::string::npos)
   d.type = nmprk::ipmi::device_nm;
  else
   goto USAGE;
  
  std::string band(argv[2]);
  found = band.find("-local");
  if(found != std::string::npos)
   d.address = nmprk::ipmi::defaultLocal;
  else{ 
   d.address = argv[3];
   d.user = argv[4];
   d.password = argv[5];
   std::cout << "Doing remote test on " << d.address << " using " << d.user << " / " << d.password << std::endl;
  }
  }
#endif 

  nmprk::ipmi::commandReq_t req;
  req.data.push_back("06");
  req.data.push_back("01");  
  
  nmprk::ipmi::commandRsp_t rsp;
  bool result;
  int  i = 0;

  std::cout << "Testing ipmi engine functions:\n";

  //std::cout << "Initing system for local..... ";
  //result = nmprk::ipmi::initSystemForLocal();
  //std::cout << TEST_CHECK_BOOL(result) << std::endl;
  //if(result == false) {
  // std::cout << "initSystemForLocal test failed, skipping rest of tests!!!\n";
  // return -1;
  //}


  std::cout << "Attempting to connect to device..... ";
  try{
   result = nmprk::ipmi::connectDevice(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception Running Command: Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(result) << std::endl;
  if(result == false) {
   std::cout << "Connection test failed, skipping rest of tests!!!\n";
   return -1;
  } 

  std::cout << "Attempting to run basic test command.... ";
  try{ 
   result = nmprk::ipmi::runIpmiCommand(&d,&req,&rsp);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception Running Command: Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  } 
  std::cout << TEST_CHECK_BOOL(result) << std::endl;
  std::cout << "Sent " << req.data.size() << " Bytes.  Expecting back 15 bytes got back " << rsp.data.size() << "..... ";
  result = rsp.data.size() == 15;
  std::cout <<  TEST_CHECK_BOOL(result) << std::endl;
  std::cout << "Resp Bytes:\n";
  std::vector<std::string>::iterator it = rsp.data.begin();
  for(;it != rsp.data.end() ; it++ ) {
    std::cout << *it << " ";
  }
  std::cout << "\n";
  req.data.clear();

  std::cout << "testing nmprk::ipmi::sel function:\n";
  
  std::cout << "Attempting to get SEL Info..... ";
  nmprk::ipmi::repoInfo_t* r;
  try {
    r = nmprk::ipmi::sel::getSelInfo(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception Running getSelInfo(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_VOID(r) << std::endl;
  if(r != NULL) {
  	  std::cout << "Repo Entries: " << r->repoEntries << std::endl;
  	  std::cout << "Repo Free Space: " << r->repoFreeSpace << " Bytes\n";
  }
 
  // Now we attempt and get the first record of the SEL
  nmprk::ipmi::address_t recordId;
  recordId.lsb = 0x00;
  recordId.msb = 0x0;
  nmprk::ipmi::record_t* record;
  std::cout << "Attempting to get first Sel Record..... "; 
  try {
   record = nmprk::ipmi::sel::getSelRecord(&d,&recordId);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception getSelRecord(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout.flush();
  std::cout << TEST_CHECK_VOID(record) << std::endl;
  if(record != NULL) {
   std::cout << "Address of Next Record LSB[" << record->nextRecord.lsb << "] MSB[" << record->nextRecord.msb << "]\n";
   std::cout.flush();   
  }
  std::cout << "\n";

 /*
  * The following section of code is considered "destructive"
  * in that it alters data entries on the system.  This could
  * leave your system in a unsteady state if for example 
  * you were to clear out the SDR or FRU or 
  * other pieces of data which are needed by a wide
  * of things (no sdr means you can't determine
  * bridge and transport to use for NM for example!)
  * To enable this code and these tests 
  * use the make-destructive
  */
#ifdef DESTRUCTIVE_CODE

  // no we write that SEL entry we just got back
  // to the same spot
  bool b;
  std::cout << "Attempting to write record back to SEL.....";
  std::cout.flush();
  try {
   b = nmprk::ipmi::sel::addSelRecord(&d,&recordId,record);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception addSelRecord(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(b) << std::endl;

  std::cout << "Attempting to delete the record from the SEL.....";
  std::cout.flush();
  try {
    b = nmprk::ipmi::sel::delSelRecord(&d,&recordId);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception delSelRecord(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(b) << std::endl;
  
  std::cout << "Attempting to clear out the SEL.....";
  std::cout.flush();
  try {
    b = nmprk::ipmi::sel::clearSel(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception clearSel(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(b) << std::endl;

#endif

  std::cout << "testing nmprk::ipmi::sdr function:\n";

  std::cout << "Attempting to get SDR Info..... ";
  try {
   r = nmprk::ipmi::sdr::getSdrInfo(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception clearSel(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_VOID(r) << std::endl;
  if(r != NULL) {
  	  std::cout << "Repo Entries: " << r->repoEntries << std::endl;
  	  std::cout << "Repo Free Space: " << r->repoFreeSpace << " Bytes\n";
  }

  recordId.lsb = 0x00;
  recordId.msb = 0x0;
  record = NULL;
  std::cout << "Attempting to get first SDR Record..... ";
  try {
   record = nmprk::ipmi::sdr::getSdrRecord(&d,&recordId);
   std::cout.flush();
  std::cout << TEST_CHECK_VOID(record) << std::endl;
  if(record != NULL) {
   std::cout << "Address of Next Record LSB[" << record->nextRecord.lsb << "] MSB[" << record->nextRecord.msb << "]\n";
   std::cout.flush();
  }
  std::cout << "\n";
  }catch(nmprk::nmprkException* e) {
	  if(e->errorCode == NMPRK_CMD_RETURNED_NON_ZERO_CODE)
	  {
		  std::cout << e->errorMsg << ": ********** FAILED **********\n";
	  }
	  else
	  {
		std::cout << "Caught Exception getSdrRecord(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
		return -1;
	  }
  }

#ifdef DESTRUCTIVE_CODE

  // no we write that SEL entry we just got back
  // to the same spot
  std::cout << "Attempting to write record back to SDR.....";
  std::cout.flush();
  try {
   b = nmprk::ipmi::sdr::addSdrRecord(&d,&recordId,record);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception addSdrRecord(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(b) << std::endl;

  std::cout << "Attempting to delete the record from the SDR.....";
  std::cout.flush();
  try {
    b = nmprk::ipmi::sdr::delSdrRecord(&d,&recordId);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception delSdrRecord(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(b) << std::endl;

  std::cout << "Attempting to clear out the SDR.....";
  std::cout.flush();
  try {
    b = nmprk::ipmi::sdr::clearSdr(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception clearSdr(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(b) << std::endl;

#endif

  std::cout << "testing nmprk::ipmi::fru function:\n";

  std::cout << "Attempting to access FRU..... ";
  nmprk::ipmi::fruInfo_t* f;
  try {
   f = nmprk::ipmi::fru::getFruInfo(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception clearSdr(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_VOID(f) << std::endl;
  if(f != NULL) {
   if(f->accessByWord == true) 
    std::cout << "Fru is accessed with word size\n";
   else
    std::cout << "Fru is access by byte\n";
  }
  
  std::cout << "Attempting to read first entry from FRU.....";
  try {
   record = nmprk::ipmi::fru::getFruData(&d,&recordId);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception getFruData(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_VOID(record) << std::endl;
  if(record != NULL) {
   std::cout << "Address of Next Record LSB[" << record->nextRecord.lsb << "] MSB[" << record->nextRecord.msb << "]\n";
   std::cout.flush();
  }
  std::cout << "\n";

 
  std::cout << "testing nmprk::ipmi::global function:\n";

  std::cout << "Attempting to get Device Id.....";
  nmprk::ipmi::getDeviceIdRsp* dID;
  try {
   dID = nmprk::ipmi::global::getDeviceId(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception getDeviceId(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_VOID(dID) << std::endl;
  if(dID != NULL) {
   std::cout << "Device Id: 0x" << nmprk::helper::int2HexStr(dID->deviceId) << std::endl;
   std::cout << "Device Revision: 0x" << nmprk::helper::int2HexStr(dID->deviceRev) << std::endl;
   std::cout << "Firmware Revision: 0x" << nmprk::helper::int2HexStr(dID->firmwareRev) << std::endl;
   std::cout << "Firmware Revision2: 0x" << nmprk::helper::int2HexStr(dID->firmwareRev2) << std::endl;
  }

  std::cout << "Determining ACPI Power State: ";
  std::cout.flush();  
  nmprk::ipmi::acpiPwrState_t p;
  try {
   p = nmprk::ipmi::global::getAcpiPwrState(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Caught Exception getAcpiPwrState(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  } 
  std::cout << "Passed\n" << std::endl;  

  std::cout << "System Power State is: ";
  switch(p.systemState) {
  case nmprk::ipmi::stateSysS0G0:
  	  std::cout << "S0/G0 (Working)";
  	  break;
  case nmprk::ipmi::stateSysS1:
  	  std::cout << "S1 (Hardware Context Maintained)";
  	  break;
  case nmprk::ipmi::stateSysS2:
  	  std::cout << "S2 (Stopped Clocks w/ Processor/Cache Context lost)";
  	  break;
  case nmprk::ipmi::stateSysS3:
  	  std::cout << "S3 (Suspend-to-Ram)";
  	  break;
  case nmprk::ipmi::stateSysS4:
  	  std::cout << "S4 (Suspend-to-Disc)";
  	  break;
  case nmprk::ipmi::stateSysS5G2:
  	  std::cout << "S5/G2 (Soft Off)";
  	  break;
  case nmprk::ipmi::stateSysS4S5:
  	  std::cout << "S4/S5 (Soft Off, Can not tell between S4 and S5)";
  	  break;
  case nmprk::ipmi::stateSysG3:
  	  std::cout << "G3 (Mechanical Off)";
  	  break;
  case nmprk::ipmi::stateSysSleeping:
	  std::cout << "Sleeping";
	  break;
  case nmprk::ipmi::stateSysG1Sleep:
	  std::cout << "G1 Sleeping";
	  break;
  case nmprk::ipmi::stateSysOverRide:
	  std::cout << "OverRide (S5 entered by override)";
	  break;
  case nmprk::ipmi::stateSysLegacyOn:
	  std::cout << "Legacy On";
	  break;
  case nmprk::ipmi::stateSysLegacyOff:
	  std::cout << "Legacy Off";
	  break;
  case nmprk::ipmi::stateSysUnknown:
	  std::cout << "Unknown (Device lost track of power state)";
	  break;
  }

  std::cout << "\nDevice Power State is: ";
  switch(p.deviceState) {
  case nmprk::ipmi::stateDevD0:
 	  std::cout << "D0";
	  break;
  case nmprk::ipmi::stateDevD1:
	  std::cout << "D1";
	  break;
  case nmprk::ipmi::stateDevD2:
	  std::cout << "D2";
	  break;
  case nmprk::ipmi::stateDevD3:
	  std::cout << "D3";
	  break;
  case nmprk::ipmi::stateDevUnknown:
	  std::cout << "Unknown";
	  break;
  }
  std::cout << "\n";
  std::cout.flush();
#ifdef DESTRUCTIVE_CODE

  std::cout << "Attempting to set ACPI Power State.....";
  try{ 
   b = nmprk::ipmi::global::setAcpiPwrState(&d,p);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception setAcpiPwrState(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  // if we get this far we passed cause other wise
  // a exception would have been caught
  std::cout << "Passed!\n";

#endif

  std::cout << "Attempting to disconnect the device..... ";
  try {
   result = nmprk::ipmi::disconnectDevice(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception disconnectDevice(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }

  std::cout << TEST_CHECK_BOOL(result) << std::endl;
}catch(nmprk::nmprkException* e) {
	std::cout << "Caught Exception :" << e->errorCode << " " << e->errorMsg << std::endl;
} 
  puts("\nPress the Enter key to exit");
  getchar();
  return 0;
}

