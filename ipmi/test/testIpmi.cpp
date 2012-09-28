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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#define TEST_CHECK_BOOL(x) ( (x == true) ? "Passed" : "Failed" )
#define TEST_CHECK_VOID(x) ( (x != NULL) ? "Passed" : "Failed" )
#define TEST_VOID(x)       ( (x == NULL) ? "Passed" : "Failed" )
#define DISPLAY_EXCEPTION(x) ( cout << "\nError: " << x << " Exception - Code: 0x" << nmprk::helper::int2HexStr(e->errorCode) << ", Msg: " << e->errorMsg << endl )
#define CONVERT_LINE(x)    ( (x >= '0' && x <= '9') ? x - '0' : (x >= 'a' && x <= 'f') ? x - 'a' + 10 : (x >= 'A' && x <= 'F') ? x - 'A' + 10 : -1 )

int main(int argc, char** argv) {
try {
  nmprk::ipmi::device d;
  d.transport = nmprk::ipmi::defaultTrans;
  d.bridge    = nmprk::ipmi::defaultBridge;
  d.cipher    = nmprk::ipmi::cipherSuite_3;
  d.type      = nmprk::ipmi::device_nm;

#if 0 // If you want to test in debugger without command line, set to 1
  d.address = "10.4.2.2";
  d.address = "local";
  //d.user = "intel";
  //d.password = "P@ssw0rd";
#else
  // valid are 2 and 5 arguments
  if((argc != 2) && (argc != 5))  
  {
USAGE:  
    cout << "Invalid arguments...\n" << endl;
    cout << "Usage:\n";
    cout << argv[0] << " [ -local | -remote host username pw ]" << endl;
    return -1;
  }else{
    string band(argv[1]);
    size_t found = band.find("-local");
    if(found != string::npos)
      d.address = nmprk::ipmi::defaultLocal;
    else{ 
      found = band.find("-remote");
      if(found != string::npos){
        d.address = argv[2];
        d.user = argv[3];
        d.password = argv[4];
        cout << "Doing remote test on " << d.address << " using " << d.user << " / " << d.password << endl;
      }else goto USAGE;
    }
  }
#endif 

  nmprk::ipmi::commandReq_t req;
  req.data.push_back("06");
  req.data.push_back("01");   
  nmprk::ipmi::commandRsp_t rsp;
  bool result;
  bool exit = false;
  int  i = 0;

  cout << "Attempting to connect to device..... ";
  try{
    result = nmprk::ipmi::connectDevice(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("connectDevice");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;
  if(result == false) {
    cout << "Connection test failed, skipping rest of tests!!!" << endl;
    return -1;
  } 

  cout << "Attempting to run basic test command.... ";
  try{ 
    result = nmprk::ipmi::runIpmiCommand(&d,&req,&rsp);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("runIpmiCommand");
    return -1;
  } 
  cout << TEST_CHECK_BOOL(result) << endl;
  cout << "Sent " << req.data.size() << " Bytes.  Expecting back 15 bytes got back " << rsp.data.size() << "..... ";
  result = rsp.data.size() == 15;
  cout <<  TEST_CHECK_BOOL(result) << endl;
  cout << "Resp Bytes:\n";
  vector<string>::iterator it = rsp.data.begin();
  for(;it != rsp.data.end() ; it++ ) {
    cout << *it << " ";
  }
  cout << endl;
  req.data.clear();

  cout << "testing nmprk::ipmi::sel function:" << endl;
  
  cout << "Attempting to get SEL Info..... ";
  nmprk::ipmi::repoInfo_t* r;
  try {
    r = nmprk::ipmi::sel::getSelInfo(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("getSelInfo");
    return -1;
  }
  cout << TEST_CHECK_VOID(r) << endl;
  if(r != NULL) 
  {
    cout << "Repo Entries: " << r->repoEntries << endl;
    cout << "Repo Free Space: " << r->repoFreeSpace << " Bytes" << endl;
    if(r->delSup == true)
      cout << "Delete SEL command is supported" << endl;
    else
      cout << "Delete SEL command is unsupported" << endl;
    if(r->parAddSup == true)
      cout << "Partial Add SEL command is supported" << endl;
    else 
      cout << "Partial Add SEL command is unsupported" << endl;
    if(r->reserveSup == true)
      cout << "Reserve SEL Repository command is supported" << endl;
    else
      cout << "Reserve SEL Repository command is unsupported" << endl;
    if(r->getAllocInfoSup == true)
      cout << "Get SEL Repository Allocation Information command is supported" << endl;
    else
      cout << "Get SEL Repository Allocation Information command is unsupported" << endl;
    delete r; // parent app must clean up and delete r
  }
 
  // Now we attempt and get the first record of the SEL
  nmprk::ipmi::address_t recordId;
  recordId.lsb = 0x00;
  recordId.msb = 0x00;
  nmprk::ipmi::record_t* record;
  cout << "Attempting to get first SEL Record..... "; 
  try 
  {
    exit = false;
    ofstream myselfile ("testfile.sel");
    if (myselfile.is_open())
    {
      do 
      {
        record = nmprk::ipmi::sel::getSelRecord(&d,&recordId);
        cout.flush();
        cout << TEST_CHECK_VOID(record) << endl;
        if(record != NULL) {
          cout << "Address of Next Record LSB[" << record->nextRecord.lsb << "] MSB[" << record->nextRecord.msb << "] ";
          cout.flush();
	        if(record->nextRecord.lsb == 0xFF) {
            exit = true;
  	  	  }else{
	  	    	recordId.lsb = record->nextRecord.lsb;
            recordId.msb = record->nextRecord.msb;
		      }
          for(unsigned int i = 0; i < record->len; i++)
	    		  myselfile << "0x" << nmprk::helper::int2HexStr(record->data[i]) << endl;	
          delete record; // parent app must clean up and delete record
        }
        else
          exit = true;
      }
      while ((record != NULL) && (exit != true));
      cout << endl;
	    myselfile.close();
	  }
    else 
      cout << "Can't open file" << endl;
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("getSelRecord");
    return -1;   
  }

#ifdef DESTRUCTIVE_CODE 

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

  bool bl = false;
  cout << "Attempting to clear out the SEL.....";
  cout.flush();
  try {
    bl = nmprk::ipmi::sel::clearSel(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("clearSel");
    return -1;
  }
  cout << TEST_CHECK_BOOL(bl) << endl;

  if(r->delSup == true)
  {
    // This command is likely not be supported by Servers that use FLASH to store their SEL events
    cout << "Attempting to delete a record from the SEL.....";
    cout.flush();
    recordId.lsb = 0x00;
    recordId.msb = 0x00;
    try {
      bl = nmprk::ipmi::sel::delSelRecord(&d,&recordId);
    }catch(nmprk::nmprkException* e) {
      DISPLAY_EXCEPTION("delSelRecord");
      return -1;
    }
    cout << TEST_CHECK_BOOL(bl) << endl;
  }
  else
    cout << "Delete SEL command is unsupported" << endl;

  cout << endl;

#endif

  cout << "testing nmprk::ipmi::sdr function:" << endl;

  cout << "Attempting to get SDR Info..... ";
  try {
    r = nmprk::ipmi::sdr::getSdrInfo(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("getSDRInfo");
    return -1;
  }
  cout << TEST_CHECK_VOID(r) << endl;
  if(r != NULL) {
  	cout << "Repo Entries: " << r->repoEntries << endl;
  	cout << "Repo Free Space: " << r->repoFreeSpace << " Bytes" << endl;
    if((r->nonmodalSupported == false) && (r->modalSupported == false))
      cout << "modal/non=modal SDR Repository Update operation unspecified" << endl;
    if((r->nonmodalSupported == true) && (r->modalSupported == false))
      cout << "non=modal SDR Repository Update operation supported" << endl;
    if((r->nonmodalSupported == false) && (r->modalSupported == true))
      cout << "modal SDR Repository Update operation supported" << endl;
    if((r->nonmodalSupported == true) && (r->modalSupported == true))
      cout << "both modal/non=modal SDR Repository Update operations supported" << endl;
    if(r->delSup == true)
      cout << "Delete SDR command is supported" << endl;
    else
      cout << "Delete SDR command is unsupported" << endl;
    if(r->parAddSup == true)
      cout << "Partial Add SDR command is supported" << endl;
    else 
      cout << "Partial Add SDR command is unsupported" << endl;
    if(r->reserveSup == true)
      cout << "Reserve SDR Repository command is supported" << endl;
    else
      cout << "Reserve SDR Repository command is unsupported" << endl;
    if(r->getAllocInfoSup == true)
      cout << "Get SDR Repository Allocation Information command is supported" << endl;
    else
      cout << "Get SDR Repository Allocation Information command is unsupported" << endl;
    delete r; // parent app must clean up and delete r
  }

  recordId.lsb = 0x00;
  recordId.msb = 0x00;
  record = NULL;
  cout << "Attempting to get first SDR Record..... ";
  try 
  {
    bool exit = false;
    ofstream mysdrfile ("testfile.sdr");
    if (mysdrfile.is_open())
    {
      do 
      {
        record = nmprk::ipmi::sdr::getSdrRecord(&d,&recordId);
        cout.flush();
        cout << TEST_CHECK_VOID(record) << endl;
        if(record != NULL) {
          cout << "Address of Next Record LSB[" << record->nextRecord.lsb << "] MSB[" << record->nextRecord.msb << "] ";
          cout.flush();
  	      if(record->nextRecord.lsb == 0xFF) {
            exit = true;
	  	    }else{
		   	    recordId.lsb = record->nextRecord.lsb;
            recordId.msb = record->nextRecord.msb;
    	    }
          for(unsigned int i = 0; i < record->len; i++)
			      mysdrfile << "0x" << nmprk::helper::int2HexStr(record->data[i]) << endl;	
          delete record; // parent app must clean up and delete record
        }
        else
          exit = true;
      }
      while (exit != true);
      cout << endl;
	    mysdrfile.close();
	  }
    else cout << "Can't open file" << endl;
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("getSdrRecord");
		return -1;
  }

#ifdef DESTRUCTIVE_CODE 

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

  bool bs = false;
  // Clear the SDR 
  cout << "Attempting to clear the SDR repository.....";
  cout.flush();
  try {
    bs = nmprk::ipmi::sdr::clearSdr(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("clearSdr");
    return -1;
  }
  cout << TEST_CHECK_BOOL(bs) << endl;

  // Now write the file back
  recordId.lsb = 0x00;
  recordId.msb = 0x00;
  record = new nmprk::ipmi::record_t();
  cout << "Attempting to write the SDR Record file..... ";

  bool bw = true;
  string linesdr;
  //ifstream mysdrfile ("C:\\Users\\Administrator\\Documents\\nmprk\\nmprk-visualstudio\\nmprkipmi\\debug\\aol-e5-05.sdr");
  ifstream mysdrfile ("testfile.sdr");
  if(mysdrfile.is_open())
  {
    int xu;
    // this routine assumes file is correct and all data is either 0x# or 0x##
    while((!mysdrfile.eof()) && (bw != false)) 
    {
      unsigned int i = 0;
      // read in sensor record data from file
      record->len = 5;
      for(; !mysdrfile.eof() && i < record->len; i++)
      {
        getline(mysdrfile,linesdr);
        if((linesdr.length() >= 3) && (linesdr.length() <= 4)) {
          xu = CONVERT_LINE(linesdr[2]);
          if(linesdr.length() == 4)
            xu = (xu * 16) + CONVERT_LINE(linesdr[3]);
        } else xu = -1;
        if(xu >= 0)
          record->data[i] = xu; 
        if(i == 4) 
          record->len = record->data[4] + 5;
      }
      if((i > 5) && (i >= record->len))
      {
        cout.flush();
        try {
          bw = nmprk::ipmi::sdr::addSdrRecord(&d,&recordId,record);
        }catch(nmprk::nmprkException* e) {
          DISPLAY_EXCEPTION("addSdrRecord");
          return -1;
        }
      }
    }
    mysdrfile.close();
	}
  else {
    cout << "Can't open file" << endl;
    return -1;
  }

  delete record;
  cout << TEST_CHECK_BOOL(bw) << endl;   
  cout << endl;
    
  if(r->delSup == true)
  {
    // This command is likely not be supported by Servers that use FLASH to store their SDRs
    bool bt = false;
    cout << "Attempting to delete the record from the SDR.....";
    cout.flush();
    try {
      bt = nmprk::ipmi::sdr::delSdrRecord(&d,&recordId);
    }catch(nmprk::nmprkException* e) {
      DISPLAY_EXCEPTION("delSdrRecord");
      return -1;
    }
    cout << TEST_CHECK_BOOL(bt) << endl;
  }
  else
    cout << "Delete SDR command is unsupported" << endl;

  cout << endl;

#endif

  cout << "testing nmprk::ipmi::fru function:" << endl;
  
  cout << "Attempting to access FRU..... ";
  nmprk::ipmi::fruInfo_t* f;
  try 
  {
    f = nmprk::ipmi::fru::getFruInfo(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("getFruInfo");
    return -1;
  }
  cout << TEST_CHECK_VOID(f) << endl;
  if(f != NULL) {
    if(f->accessByWord == true) {
      cout << "Fru is accessed with word size" << endl;
    }else{
      cout << "Fru is access by byte\n";
 
      // Test to see of the FRU size is greater than zero
      if((f->fruSize[0] > 0) || (f->fruSize[1] > 0))
      {
        cout << "Attempting to read FRU file.....";
        try 
        {
          ofstream myfrufile ("testfile.fru");
          if (myfrufile.is_open())
          {
	          record = nmprk::ipmi::fru::getFruRpry(&d);
            cout.flush();
            cout << TEST_CHECK_VOID(record) << endl;
            if(record != NULL) {
              cout.flush();
              for(unsigned int i = 0; i < record->len; i++)
	  		        myfrufile << "0x" << nmprk::helper::int2HexStr(record->data[i]) << endl;	
            }
            delete record;
            cout << endl;
	          myfrufile.close();
    	    }
          else cout << "Can't open file" << endl;
        }catch(nmprk::nmprkException* e) {
          DISPLAY_EXCEPTION("getFruData");
          return -1;
        }
      }
      else 
        cout << "FRU size is zero, no FRU exists on this system" << endl; 
    }
  }

#ifdef DESTRUCTIVE_CODE 

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

  // Now write the file back
  bool bf = false;
  recordId.lsb = 0x00;
  recordId.msb = 0x00;
  record = new nmprk::ipmi::record_t();
  cout << "Attempting to write the FRU file..... ";

  string linefru;
  //ifstream myfrufile ("C:\\Users\\Administrator\\Documents\\nmprk\\nmprk-visualstudio\\nmprkipmi\\debug\\testfile1.fru");
  ifstream myfrufile ("testfile.fru");
  if(myfrufile.is_open())
  {
    int count = 0;
    int xu = 0;
    // this routine assumes file is correct and all data is either 0x# or 0x##
    while(!myfrufile.eof())
    {
      getline(myfrufile,linefru);
      if((linefru.length() >= 3) && (linefru.length() <= 4)) {
        xu = CONVERT_LINE(linefru[2]);
        if(linefru.length() == 4)
          xu = (xu * 16) + CONVERT_LINE(linefru[3]);
      } else xu = 0;
      if(xu >= 0)
        record->data[count++] = xu; 
    }
    record->len = count;

    if(record->len > 0)
    {
      cout.flush();
      try {
        bf = nmprk::ipmi::fru::setFruData(&d,&recordId,record);
      }catch(nmprk::nmprkException* e) {
        DISPLAY_EXCEPTION("setFruData");
        return -1;
      }
    }
    myfrufile.close();
	}
  else{
    cout << "Can't open file" << endl;
    return -1;
  }

  delete record;
  cout << TEST_CHECK_BOOL(bf) << endl;   
  cout << endl;
 
#endif

  cout << "testing nmprk::ipmi::global function:" << endl;

  cout << "Attempting to get Device Id.....";
  nmprk::ipmi::getDeviceIdRsp* dID;
  try {
    dID = nmprk::ipmi::global::getDeviceId(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("getDeviceId");
    return -1;
  }
  cout << TEST_CHECK_VOID(dID) << endl;
  if(dID != NULL) {
    cout << "Device Id: 0x" << nmprk::helper::int2HexStr(dID->deviceId) << endl;
    cout << "Device Revision: 0x" << nmprk::helper::int2HexStr(dID->deviceRev) << endl;
    cout << "Firmware Revision: 0x" << nmprk::helper::int2HexStr(dID->firmwareRev) << endl;
    cout << "Firmware Revision2: 0x" << nmprk::helper::int2HexStr(dID->firmwareRev2) << endl;
  }

  cout << "Determining ACPI Power State: ";
  cout.flush();  
  nmprk::ipmi::acpiPwrState_t p;
  try {
    p = nmprk::ipmi::global::getAcpiPwrState(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("getAcpiPwrState");
    return -1;
  } 
  cout << "Passed\n" << endl;  

  cout << "System Power State is: ";
  switch(p.systemState) {
  case nmprk::ipmi::stateSysS0G0:
  	  cout << "S0/G0 (Working)";
  	  break;
  case nmprk::ipmi::stateSysS1:
  	  cout << "S1 (Hardware Context Maintained)";
  	  break;
  case nmprk::ipmi::stateSysS2:
  	  cout << "S2 (Stopped Clocks w/ Processor/Cache Context lost)";
  	  break;
  case nmprk::ipmi::stateSysS3:
  	  cout << "S3 (Suspend-to-Ram)";
  	  break;
  case nmprk::ipmi::stateSysS4:
  	  cout << "S4 (Suspend-to-Disc)";
  	  break;
  case nmprk::ipmi::stateSysS5G2:
  	  cout << "S5/G2 (Soft Off)";
  	  break;
  case nmprk::ipmi::stateSysS4S5:
  	  cout << "S4/S5 (Soft Off, Can not tell between S4 and S5)";
  	  break;
  case nmprk::ipmi::stateSysG3:
  	  cout << "G3 (Mechanical Off)";
  	  break;
  case nmprk::ipmi::stateSysSleeping:
	  cout << "Sleeping";
	  break;
  case nmprk::ipmi::stateSysG1Sleep:
	  cout << "G1 Sleeping";
	  break;
  case nmprk::ipmi::stateSysOverRide:
	  cout << "OverRide (S5 entered by override)";
	  break;
  case nmprk::ipmi::stateSysLegacyOn:
	  cout << "Legacy On";
	  break;
  case nmprk::ipmi::stateSysLegacyOff:
	  cout << "Legacy Off";
	  break;
  case nmprk::ipmi::stateSysUnknown:
	  cout << "Unknown (Device lost track of power state)";
	  break;
  }

  cout << "\nDevice Power State is: ";
  switch(p.deviceState) {
  case nmprk::ipmi::stateDevD0:
 	  cout << "D0";
	  break;
  case nmprk::ipmi::stateDevD1:
	  cout << "D1";
	  break;
  case nmprk::ipmi::stateDevD2:
	  cout << "D2";
	  break;
  case nmprk::ipmi::stateDevD3:
	  cout << "D3";
	  break;
  case nmprk::ipmi::stateDevUnknown:
	  cout << "Unknown";
	  break;
  }
  cout << endl;
  cout.flush();

#ifdef DESTRUCTIVE_CODE

  cout << "Attempting to set ACPI Power State.....";
  try{ 
    b = nmprk::ipmi::global::setAcpiPwrState(&d,p);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("setAcpiPwrState");
    return -1;
  }
  // if we get this far we passed cause other wise
  // a exception would have been caught
  cout << "Passed!\n";

#endif

  cout << "Attempting to disconnect the device..... ";
  try {
    result = nmprk::ipmi::disconnectDevice(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("disconnectDevice");
    return -1;
  }

  cout << TEST_CHECK_BOOL(result) << endl;
}catch(nmprk::nmprkException* e) {
	cout << "Caught Exception :" << e->errorCode << " " << e->errorMsg << endl;
} 
  puts("\nPress the Enter key to exit");
  getchar();
  return 0;
}

