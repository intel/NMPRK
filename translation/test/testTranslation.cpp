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
 #include <iostream>

#include "../../ipmi/include/nmprk_ipmi.h"
#include "../../ipmi/include/nmprk_exception.h"
#include "../../ipmi/include/nmprk_errCodes.h"
#include "../../ipmi/include/nmprk_helper.h"
#include "../../translation/include/nmprk_translation.h"
#include "../../translation/include/nmprk_translation_defines.h"

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

#ifdef WIN32
#include <Windows.h>
#endif


#define TEST_CHECK_BOOL(x) ( (x == true) ? "Passed" : "Failed" )
#define TEST_CHECK_VOID(x) ( (x != NULL) ? "Passed" : "Failed" )
#define TEST_VOID(x)       ( (x == NULL) ? "Passed" : "Failed" )

int main(int argc, char** argv) {

  nmprk::ipmi::device d;
  d.bridge = nmprk::ipmi::defaultBridge;
  d.transport = nmprk::ipmi::defaultTrans;
  d.type = nmprk::ipmi::device_nm;

#if 0 // If you want to test in debugger without command line, set to 1
  d.address = "10.20.0.41";
  //d.address = "local";
  d.user = "ipmitest";
  d.password = "password";
  d.transport = 0x2c;
  d.bridge = 0x06;
  d.type = nmprk::ipmi::device_nm;
#else
  if(argc < 3) {
USAGE:  
    std::cout << "Invalid arguments\n";
    std::cout << "Usage:\n";
    std::cout << argv[0] << " [ -dcmi -dnm -nm ] [ -local | -remote host username pw ]\n";
    return -1;
  }
 
  std::string mode(argv[1]);
  size_t found = mode.find("-dcmi");
  if(found != std::string::npos)
   d.type = nmprk::ipmi::device_dcmi;
  else if((found = mode.find("-dnm")) != std::string::npos)
   d.type = nmprk::ipmi::device_dnm;
  else if((found = mode.find("-nm")) != std::string::npos)
  {
   d.type = nmprk::ipmi::device_nm;
   d.transport = 0x2c;
   d.bridge = 0x06;
  }
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
#endif

  nmprk::ipmi::commandReq_t req;
  req.data.push_back("06");
  req.data.push_back("01");  

  nmprk::ipmi::commandRsp_t rsp;
  bool result;
  int  i = 0;

  std::cout << "Testing translation engine functions:\n";


  std::cout << "Attempting to init the library.....";
  nmprk::translation::initType_t init = nmprk::translation::initLibrary;
  std::cout.flush(); 
  try {
    result = nmprk::translation::swSubSystemSetup(init,NULL);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception swSubSystemSetup(library): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(result) << std::endl;
  if(result == false) {
   std::cout << "Unable to init library.  skipping rest of tests\n";
   return -1;
  }
  
  init = nmprk::translation::initDevice;
  std::cout << "Attempting to init the device.....";
  std::cout.flush();
  try {
    result = nmprk::translation::swSubSystemSetup(init,&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception swSubSystemSetup(device): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
  }
  std::cout << TEST_CHECK_BOOL(result) << std::endl;
  if(result == false) {
   std::cout << "Unable to init device.  skipping rest of tests\n";
   return -1;
  }

  std::cout << "Attempting to get Device Capabilities.....";
  std::cout.flush();
  nmprk::translation::capabilities_t* cap;
  try {
    cap = nmprk::translation::getCapabilities(&d);
  }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception getCapabilities(device): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
 } 
 std::cout << TEST_CHECK_VOID(cap) << std::endl;
 if(cap != NULL) {  
   std::cout << "Device Capabilities:\n";
   if(d.type == nmprk::ipmi::device_dnm || d.type == nmprk::ipmi::device_nm) {
    std::cout << "\tMax Concurrent Connections: " << cap->maxConSettings << std::endl;
    std::cout << "\tMax Trigger Value (for policies): " << cap->maxTriggerValue << std::endl;
    std::cout << "\tMin Trigger Value (for policies): " << cap->minTriggerValue << std::endl;
    std::cout << "\tMax Correction Time (for policies): " << cap->maxCorrectionTime << std::endl;
    std::cout << "\tMin Correction Time (for policies): " << cap->minCorrectionTime << std::endl;
    std::cout << "\tMax Statistics Reporting Period: " << cap->maxStatReportPeriod << std::endl;
    std::cout << "\tMin Statistics Reporting Period: " << cap->minStatReportPeriod << std::endl; 
   }else{ // display dcmi info 
    std::cout << "\tSupports Power Managment: " << ((cap->supportsPowerManagement == true) ? "True" : "False") << std::endl;
    std::cout << "\tSupports Manageability Acess: " << ((cap->manageabilityAccess == true) ? "True" : "False") << std::endl;
    std::cout << "\tSupports In Band Access: "   << ((cap->inBandInterfaceEnabled == true) ? "True" : "False") << std::endl;
    std::cout << "\tSupports Out Of Band Lan Access: " << ((cap->oobOverLan == true) ? "True" : "False") << std::endl;
    std::cout << "\tSupports Serial Mode: " << ((cap->serialMode == true) ? "True" : "False") << std::endl;
    std::cout << "\tTemperature Sampling Frequency: " << cap->temperatureSamplingFreq << std::endl;
   } 
 }
 
 nmprk::translation::subSystemComponentType_t comp = nmprk::translation::domainSystem;

 // Software Forge Inc. --- Start ------------------------------------------
 // Reset statistics does not apply for DCMI
 if(d.type != nmprk::ipmi::device_dcmi)
 {
	 std::cout << "Attempting to Reset Statistics.....";
	 std::cout.flush();
	 try {
	  result = nmprk::translation::resetStatistics(&d,comp,NULL);
	 }catch(nmprk::nmprkException* e) {
	   std::cout << "Failed!\nCaught Exception reset statistics(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
	   return -1;
	 }
	 std::cout << TEST_CHECK_BOOL(result) << std::endl;
 }
 // Software Forge Inc. --- End --------------------------------------------

 std::cout << "Waiting for statistics to be gathered...";
#ifdef WIN32
 Sleep(5000);
#else
 sleep(5);
#endif

 nmprk::translation::sample_t* sample;
 nmprk::translation::sampleType_t type = nmprk::translation::samplePower;
 std::cout << "Attempting to get sample power reading.....";
 std::cout.flush();
 try {
  sample = nmprk::translation::getSample(&d,type,comp,NULL);
 }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception getSample(power): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
 }
 std::cout << TEST_CHECK_VOID(sample) << std::endl;
 if(sample != NULL) {
  std::cout << "Power Reading:\n";
  std::cout << "\tCurrent Power Reading (Watts): " << sample->cur << std::endl;
  std::cout << "\tMin Power Reading (Watts): " << sample->min << std::endl;
  std::cout << "\tMax Power Reading (Watts): " << sample->max << std::endl;
  std::cout << "\tAverage Power Reading (Watts): " << sample->avg << std::endl;
  std::cout << "\tReading Time Stamp: " << asctime(&sample->timestamp) << std::endl;
  std::cout << "\tReporting Period (seconds): " << sample->statReportingPeriod << std::endl;
 }

 sample = NULL;
 type = nmprk::translation::sampleThermal;
 std::cout << "Attempting to get sample thermal reading.....";
 std::cout.flush();
 try {
  sample = nmprk::translation::getSample(&d,type,comp,NULL);
 }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception getSample(thermal): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   // Some servers do not support temp (i.e. SR1625 NM 1.5), let the test continue
   //return -1;
 }
 std::cout << TEST_CHECK_VOID(sample) << std::endl;
 if(sample != NULL) {
  std::cout << "Power|Thermal Reading:\n";
  std::cout << "\tCurrent Power Reading (Watts): " << sample->cur << std::endl;
  std::cout << "\tAverage Power Reading (Watts): " << sample->avg << std::endl;
  std::cout << "\tReading Time Stamp: " << asctime(&sample->timestamp) << std::endl;
  std::cout << "\tReporting Period (seconds): " << sample->statReportingPeriod << std::endl;
 }

 nmprk::translation::policy_t* policy = new nmprk::translation::policy_t();
 if(policy == NULL) {
  std::cout << "Memory allocation error, unable to do policy testing\n";
  return -1;
 }

 std::cout << "Attempting to create policy #3.....";
 std::cout.flush();

 policy->policyId = 0x03;
 policy->component = nmprk::translation::domainSystem;
 policy->policyType = nmprk::translation::policyPower;
 policy->policyLimit = cap->maxTriggerValue;
 // Software Forge Inc. --- Start ------------------------------------------
 // Some parameters are different for DCMI
 if(d.type == nmprk::ipmi::device_dcmi)
 {
	policy->statReportingPeriod = 5;
	policy->correctionTime = 60000;
 }
 else
 {
	policy->statReportingPeriod = cap->maxStatReportPeriod;
	policy->correctionTime = cap->maxCorrectionTime;
 }
 // Software Forge Inc. --- End --------------------------------------------
 policy->policyEnabled = false;
 policy->sendAlert = false;
 policy->shutdown = false;

 try {
  result = nmprk::translation::setPolicy(&d,policy);
 }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception setPolicy(0x03): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
 }
 std::cout << TEST_CHECK_BOOL(result) << std::endl;

 policy = new nmprk::translation::policy_t();
 policy->policyId = 0x03;
 if(policy == NULL) {
  std::cout << "Memory allocation error, unable to do policy testing\n";
  return -1;
 }

 std::cout << "Attempting to get policy #3.....";
 std::cout.flush();
 try {
  policy = nmprk::translation::getPolicy(&d,policy);
 }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception getPolicy(0x03): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
 }
 std::cout << TEST_CHECK_VOID(policy) << std::endl;
 if(policy != NULL) {
  std::cout << "Policy #3 Info:\n";
  std::cout << "Policy type: " << ((policy->policyType == nmprk::translation::policyPower) ? "Power" : "Thermal") << std::endl;
  std::cout << "Policy limit (" << ((policy->policyType == nmprk::translation::policyPower) ? "Watts" : "celcius") << "): " << policy->policyLimit << std::endl;
  std::cout << "Send Alert: " << ((policy->sendAlert == true)? "True" : "False") << std::endl;
  std::cout << "Shutdown: " << ((policy->shutdown == true)? "True" : "False") << std::endl;
  std::cout << "Policy Enabled: " << ((policy->policyEnabled == true) ? "True" : "False") << std::endl;
 }
 
 std::cout << "Attempting to delete policy #3.....";
 std::cout.flush();
 try {
  result = nmprk::translation::delPolicy(&d,policy);
 }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception delPolicy(0x03): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
 }
 std::cout << TEST_CHECK_BOOL(result) << std::endl;

 std::cout << "Attempting to disconnect the device..... ";
 std::cout.flush();
 try {
  result = nmprk::ipmi::disconnectDevice(&d);
 }catch(nmprk::nmprkException* e) {
   std::cout << "Failed!\nCaught Exception disconnectDevice(): Code[" << e->errorCode << "] MSG[" << e->errorMsg << "]\n";
   return -1;
 }
 std::cout << TEST_CHECK_BOOL(result) << std::endl;
 
 puts("\nPress the Enter key to exit");
 getchar();
 return 0;
}

