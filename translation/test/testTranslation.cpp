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
using namespace std;

#ifdef WIN32
#include <Windows.h>
#endif

#define TEST_CHECK_BOOL(x) ( (x == true) ? "Passed" : "Failed" )
#define TEST_CHECK_VOID(x) ( (x != NULL) ? "Passed" : "Failed" )
#define TEST_VOID(x)       ( (x == NULL) ? "Passed" : "Failed" )
#define DISPLAY_EXCEPTION(x) ( cout << "\nError: " << x << " Exception - Code: 0x" << nmprk::helper::int2HexStr(e->errorCode) << ", Msg: " << e->errorMsg << endl )

int main(int argc, char** argv) 
{
  nmprk::ipmi::device d;
  d.type = nmprk::ipmi::device_nm;
  d.transport = 0x2c;
  d.bridge = 0x06;

#if 0 // If you want to test in debugger without command line, set to 1
  d.address = "10.4.2.29";
  d.address = "local";
  d.user = "intel";
  d.password = "P@ssw0rd";

#else
  // valid are 2 and 5 arguments
  if((argc != 2) && (argc != 5))  
  {
USAGE:  
    cout << "Invalid arguments...\n\n";
    cout << "Usage:\n";
    cout << argv[0] << " [ -local | -remote host username pw ]\n";
    return -1;
  }else{
    string band(argv[1]);
    size_t found = band.find("-local");
    if(found != string::npos)
      d.address = nmprk::ipmi::defaultLocal;
    else
    { 
      found = band.find("-remote");
      if(found != string::npos){
        d.address = argv[2];
        d.user = argv[3];
        d.password = argv[4];
        cout << "Doing remote test on " << d.address << " using " << d.user << " / " << d.password << endl;
      }else 
        goto USAGE;
    }
  }

#endif

  nmprk::ipmi::commandReq_t req;
  req.data.push_back("06");
  req.data.push_back("01");  

  nmprk::ipmi::commandRsp_t rsp;
  bool result;
  int  i = 0;

  cout << "Testing translation engine functions:\n";
  
  cout << "Attempting to init the library.....";
  nmprk::translation::initType_t init = nmprk::translation::initLibrary;
  cout.flush(); 
  try {
    result = nmprk::translation::swSubSystemSetup(init,NULL);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("swSubSystemSetup");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;
  if(result == false) {
    cout << "Unable to init library.  skipping rest of tests" << endl;
    return -1;
  }
  
  init = nmprk::translation::initDevice;
  cout << "Attempting to init the device.....";
  cout.flush();
  try {
    result = nmprk::translation::swSubSystemSetup(init,&d);
  }catch(nmprk::nmprkException* e) {
	  DISPLAY_EXCEPTION("swSubSystemSetup");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;
  if(result == false) {
    cout << "Unable to init device.  skipping rest of tests" << endl;
    return -1;
  }

  bool nmVersion2pt0 = false;
  cout << "Attempting to get Node Manager Version.....";
  cout.flush();
  nmprk::translation::nmVersion_t* ver;
  try {
    ver = nmprk::translation::getNMVersion(&d);
  }catch(nmprk::nmprkException* e) {
	  DISPLAY_EXCEPTION("getNMVersion");
    return -1;
  } 
  cout << TEST_CHECK_VOID(ver) << endl;
  if(ver != NULL) 
  {  
    cout << "Versions:" << endl;
    if(ver->nmVersion_1pt5 == true) {
      cout << "\tNode Manager Version 1.5" << endl;
    }else if(ver->nmVersion_2pt0 == true){
      cout << "\tNode Manager Version 2.0" << endl;
      nmVersion2pt0 = true;
    }
    if(ver->ipmiVersion_1pt0 == true)
      cout << "\tIPMI Version 1.0" << endl;
    else if(ver->ipmiVersion_2pt0 == true)
      cout << "\tIPMI Version 2.0" << endl;
  }
  delete ver;

  cout << "Attempting to get Device Capabilities.....";
  cout.flush();
  nmprk::translation::capabilities_t* cap;
  try {
    cap = nmprk::translation::getCapabilities(&d);
  }catch(nmprk::nmprkException* e) {
	  DISPLAY_EXCEPTION("getCapabilities");
    return -1;
  } 
  cout << TEST_CHECK_VOID(cap) << endl;
  if(cap != NULL) 
  {  
    cout << "Device Capabilities:" << endl;
    if(d.type == nmprk::ipmi::device_nm) 
    {
      cout << "\tMax Concurrent Connections: " << cap->maxConSettings << endl;
      cout << "\tMax Trigger Value (for policies): " << cap->maxTriggerValue << endl;
      cout << "\tMin Trigger Value (for policies): " << cap->minTriggerValue << endl;
      cout << "\tMax Correction Time (for policies): " << cap->maxCorrectionTime << endl;
      cout << "\tMin Correction Time (for policies): " << cap->minCorrectionTime << endl;
      cout << "\tMax Statistics Reporting Period: " << cap->maxStatReportPeriod << endl;
      cout << "\tMin Statistics Reporting Period: " << cap->minStatReportPeriod << endl; 
    }else{ // display dcmi info 
      cout << "\tSupports Power Managment: " << ((cap->supportsPowerManagement == true) ? "True" : "False") << endl;
      cout << "\tSupports Manageability Acess: " << ((cap->manageabilityAccess == true) ? "True" : "False") << endl;
      cout << "\tSupports In Band Access: "   << ((cap->inBandInterfaceEnabled == true) ? "True" : "False") << endl;
      cout << "\tSupports Out Of Band Lan Access: " << ((cap->oobOverLan == true) ? "True" : "False") << endl;
      cout << "\tSupports Serial Mode: " << ((cap->serialMode == true) ? "True" : "False") << endl;
      cout << "\tTemperature Sampling Frequency: " << cap->temperatureSamplingFreq << endl;
    } 
  }
 
  nmprk::translation::subSystemComponentType_t comp = nmprk::translation::domainSystem;

  // Reset statistics does not apply for DCMI
  if(d.type == nmprk::ipmi::device_nm)
  {
    cout << "Attempting to Reset Statistics.....";
	  cout.flush();
  	try {
	    result = nmprk::translation::resetStatistics(&d,comp,NULL);
	  }catch(nmprk::nmprkException* e) {
	    DISPLAY_EXCEPTION("resetStatistics");
	    return -1;
    }
	  cout << TEST_CHECK_BOOL(result) << endl;
  }

  cout << "\nWaiting for statistics to be gathered..." << endl;
#ifdef WIN32
  Sleep(5000);
#else
  sleep(5);
#endif

  nmprk::translation::sample_t* sample;
  nmprk::translation::sampleType_t type = nmprk::translation::samplePower;
  cout << "Attempting to get sample power reading.....";
  cout.flush();
  try {
    sample = nmprk::translation::getSample(&d,type,comp,NULL);
  }catch(nmprk::nmprkException* e) {
	  DISPLAY_EXCEPTION("getSample");
    return -1;
  }
  cout << TEST_CHECK_VOID(sample) << endl;
  if(sample != NULL) 
  {
    cout << "Power Reading:\n";
    cout << "\tCurrent Power Reading (Watts): " << sample->cur << endl;
    cout << "\tMin Power Reading (Watts): " << sample->min << endl;
    cout << "\tMax Power Reading (Watts): " << sample->max << endl;
    cout << "\tAverage Power Reading (Watts): " << sample->avg << endl;
    cout << "\tReading Time Stamp: " << asctime(&sample->timestamp); 
    cout << "\tReporting Period (seconds): " << sample->statReportingPeriod << endl << endl;
  }

  sample = NULL;
  type = nmprk::translation::sampleThermal;
  cout << "Attempting to get sample thermal reading.....";
  cout.flush();
  try {
   sample = nmprk::translation::getSample(&d,type,comp,NULL);
  }catch(nmprk::nmprkException* e) {
	  DISPLAY_EXCEPTION("getSample");
    // Some servers do not support temp (i.e. SR1625 NM 1.5), so let the test continue
  }
  cout << TEST_CHECK_VOID(sample) << endl;
  if(sample != NULL) 
  {
    cout << "Power|Thermal Reading:\n";
    cout << "\tCurrent Thermal Reading (Celcius): " << sample->cur << endl;
    cout << "\tAverage Thermal Reading (Celcius): " << sample->avg << endl;
    cout << "\tReading Time Stamp: " << asctime(&sample->timestamp); 
    cout << "\tReporting Period (seconds): " << sample->statReportingPeriod << endl << endl;
  }

  nmprk::translation::policy_t* policy = new nmprk::translation::policy_t();
  if(policy == NULL) {
    cout << "Memory allocation error, unable to do policy testing\n";
    return -1;
  }

  unsigned int policyNum = 3;
  cout << "Attempting to get policy #" << policyNum << ", it should not exist....." << endl;
  policy->policyId = policyNum;
  policy->component = nmprk::translation::domainSystem;
  cout.flush();
  try {
    policy = nmprk::translation::getPolicy(&d,policy);
    if(policy != NULL) // then policy exists so delete it.
	  {
      // Now try to delete it
      cout << "Policy #" << policyNum << " exists, attempting to delete policy #" << policyNum << "....." << endl;
      cout.flush();
      try {
        result = nmprk::translation::delPolicy(&d,policy);
      }catch(nmprk::nmprkException* e) {
	      DISPLAY_EXCEPTION("delPolicy");
        return -1;
      }
      cout << TEST_CHECK_BOOL(result) << endl;
    }
  }catch(nmprk::nmprkException* e) {
    if(e->errorCode != 0x80) {
	    DISPLAY_EXCEPTION("getPolicy");
      return -1;
    }else cout << "As expected, policy does not exist. Continuing test." << endl;
  }

  cout << "Attempting to create policy #" << policyNum << ".....";
  cout.flush();

  policy->policyId = policyNum;
  policy->component = nmprk::translation::domainSystem;
  policy->policyType = nmprk::translation::noPolicyTrigger;
  policy->policyLimit = 350;			// in watts, (old value cap->maxTriggerValue)

  // Some parameters are different for DCMI
  if(d.type != nmprk::ipmi::device_nm)
  {
	  policy->statReportingPeriod = 5;
	  policy->correctionTime = 60000;
  }
  else
  {
	  policy->statReportingPeriod = cap->maxStatReportPeriod;
	  policy->correctionTime = cap->maxCorrectionTime;
  }
  policy->policyEnabled = true;
  policy->sendAlert = false;
  policy->shutdown = false;

  try {
    result = nmprk::translation::setPolicy(&d,policy);
  }catch(nmprk::nmprkException* e) {
	  DISPLAY_EXCEPTION("setPolicy");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;

  cout << "\nVerify policy #" << policyNum << " on remote server exists and is enabled, then press Enter" << endl;
  getchar();
  
  cout << "Attempting to disable policy #" << policyNum << ".....";
  cout.flush();

  policy->policyId = policyNum;
  policy->component = nmprk::translation::domainSystem;
  nmprk::translation::policyStatusType_t status;
  status = nmprk::translation::policyDisabled;

  try {
    result = nmprk::translation::setPolicyStatus(&d,policy,status);
  }catch(nmprk::nmprkException* e) {
	  DISPLAY_EXCEPTION("setPolicyStatus");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;
  
  cout << "Attempting to modify policy #" << policyNum << " to 330 watts.....";
  cout.flush();

  policy->policyId = policyNum;
  policy->component = nmprk::translation::domainSystem;
  policy->policyType = nmprk::translation::noPolicyTrigger;
  policy->policyLimit = 330;			// in watts, (old value cap->maxTriggerValue)

  // Some parameters are different for DCMI
  if(d.type != nmprk::ipmi::device_nm)
  {
	  policy->statReportingPeriod = 5;
	  policy->correctionTime = 60000;
  }
  else
  {
	  policy->statReportingPeriod = cap->maxStatReportPeriod;
	  policy->correctionTime = cap->maxCorrectionTime;
  }
  policy->policyEnabled = false;
  policy->sendAlert = false;
  policy->shutdown = false;

  try {
    result = nmprk::translation::setPolicy(&d,policy);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("setPolicy");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;

  cout << "\nVerify policy #" << policyNum << " on remote server exists and is not enabled, then press Enter" << endl;
  getchar();
 
  cout << "Attempting to enable policy #" << policyNum << ".....";
  cout.flush();

  policy->policyId = policyNum;
  policy->component = nmprk::translation::domainSystem;
  status = nmprk::translation::policyEnabled;

  try {
    result = nmprk::translation::setPolicyStatus(&d,policy,status);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("setPolicyStatus");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;

  cout << "\nVerify policy #" << policyNum << " on remote server is now enabled, then press Enter" << endl;
  getchar();

  policy = new nmprk::translation::policy_t();
  policy->policyId = policyNum;
  if(policy == NULL) {
    cout << "Memory allocation error, unable to do policy testing\n";
    return -1;
  }

  cout << "Attempting to get policy #" << policyNum << ".....";
  cout.flush();
  try {
    policy = nmprk::translation::getPolicy(&d,policy);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("getPolicy");
    return -1;
  }
  cout << TEST_CHECK_VOID(policy) << endl;
  if(policy != NULL) 
  {
    cout << "Policy #" << policyNum << " Info: " << endl;
    cout << "\tPolicy trigger type: " << ((policy->policyType == nmprk::translation::noPolicyTrigger) ? "No trigger" : "Thermal trigger") << endl;
    cout << "\tAggressive Enabled: " << ((policy->aggressiveCorrect == nmprk::translation::forceAggressive) ? "True" : "False") << endl;
    cout << "\tSend Alert: " << ((policy->sendAlert == true)? "True" : "False") << endl;
    cout << "\tShutdown: " << ((policy->shutdown == true)? "True" : "False") << endl;
    cout << "\tPolicy Enabled: " << ((policy->policyEnabled == true) ? "True" : "False") << endl;
  }
 
  cout << "Attempting to delete policy #" << policyNum << ".....";
  cout.flush();
  try {
    result = nmprk::translation::delPolicy(&d,policy);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("delPolicy");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;
 
  // Start thermal trigger power policy test
  policy = new nmprk::translation::policy_t();
  if(policy == NULL) {
    cout << "Memory allocation error, unable to do policy testing" << endl;
    return -1;
  }


  // The following code appears to only work on policies 0 and 1 on NM 2.0 systems that have not been
  // setup for DCM. It sets a thermal trip policy. It should also work on NM 1.5 systems, but....
  if(nmVersion2pt0 == true)
  {
    policyNum = 1;
    cout << "\nAttempting to get policy #" << policyNum << ", it should not exist....." << endl;
    bool skip = false;
    policy->policyId = policyNum;
    policy->component = nmprk::translation::domainSystem;
    cout.flush();
    try {
      policy = nmprk::translation::getPolicy(&d,policy);
      if(policy != NULL) // then policy exists so delete it.
  	  {
        cout << "Policy #" << policyNum << " exists, attempting to delete policy #" << policyNum << "....." << endl;
        cout.flush();
        try {
          result = nmprk::translation::delPolicy(&d,policy);
        }catch(nmprk::nmprkException* e) {
          DISPLAY_EXCEPTION("delPolicy");
          cout << "This would fail if DCM is connected to the server" << endl;
          skip = true;
        }
        if(skip == false) cout << TEST_CHECK_BOOL(result) << endl;
      }
    }catch(nmprk::nmprkException* e) {
      if(e->errorCode != 0x80) {
	      DISPLAY_EXCEPTION("getPolicy");
        return -1;
      }else cout << "As expected, policy #" << policyNum << " does not exist. Continuing test." << endl;
    }
    cout << endl;

    if(skip == false)
    {
      cout << "Attempting to create policy #" << policyNum << ".....";
      cout.flush();

      policy->policyId = policyNum;
      policy->component = nmprk::translation::domainSystem;
      policy->policyType = nmprk::translation::inletTempTrigger;	
      policy->policyTriggerLimit = 0x15;
      policy->policyLimit = 350;				// in watts, (old cap->maxTriggerValue)
      policy->aggressiveCorrect = nmprk::translation::forceNonAggressive;

      // Some parameters are different for DCMI
      if(d.type != nmprk::ipmi::device_nm)
      {
	      policy->statReportingPeriod = 5;
  	    policy->correctionTime = 60000;
      }
      else
      {
	      policy->statReportingPeriod = cap->maxStatReportPeriod;
  	    policy->correctionTime = cap->maxCorrectionTime;
      }
      policy->policyEnabled = true; 
      policy->sendAlert = true;
      policy->shutdown = false;

      try {
        result = nmprk::translation::setPolicy(&d,policy);
      }catch(nmprk::nmprkException* e) {
        DISPLAY_EXCEPTION("setPolicy");
        return -1;
      }
      cout << TEST_CHECK_BOOL(result) << endl;

      policy = new nmprk::translation::policy_t();
      policy->policyId = policyNum;
      if(policy == NULL) {
        cout << "Memory allocation error, unable to do policy testing\n";
        return -1;
      }

      cout << "\nVerify policy #" << policyNum << " on remote server, then press Enter" << endl;
      getchar();

      cout << "Attempting to get policy #" << policyNum << ".....";
      cout.flush();
      try {
        policy = nmprk::translation::getPolicy(&d,policy);
      }catch(nmprk::nmprkException* e) {
        DISPLAY_EXCEPTION("getPolicy");
        return -1;
      }
      cout << TEST_CHECK_VOID(policy) << endl;
      if(policy != NULL) 
      {
        cout << "Policy #" << policyNum << " Info: " << endl;
        cout << "\tPolicy trigger type: " << ((policy->policyType == nmprk::translation::noPolicyTrigger) ? "No trigger" : "Thermal trigger") << endl;
        cout << "\tAggressive Enabled: " << ((policy->aggressiveCorrect == nmprk::translation::forceAggressive) ? "True" : "False") << endl;
        cout << "\tSend Alert: " << ((policy->sendAlert == true)? "True" : "False") << endl;
        cout << "\tShutdown: " << ((policy->shutdown == true)? "True" : "False") << endl;
        cout << "\tPolicy Enabled: " << ((policy->policyEnabled == true) ? "True" : "False") << endl;
      }
 
      cout << "Attempting to delete policy #" << policyNum << ".....";
      cout.flush();
      try {
        result = nmprk::translation::delPolicy(&d,policy);
      }catch(nmprk::nmprkException* e) {
        DISPLAY_EXCEPTION("delPolicy");
        return -1;
      }
      cout << TEST_CHECK_BOOL(result) << endl;

      // Attempt to delete the policy again.
      cout << "Attempting to delete policy #" << policyNum << " again....." << endl;
      cout.flush();
      try {
        result = nmprk::translation::delPolicy(&d,policy);
      }catch(nmprk::nmprkException* e) {
        if(e->errorCode != 0x80) {
          DISPLAY_EXCEPTION("delPolicy");
          return -1;
        }else{
	        cout << "As expected, policy #" << policyNum << " does not exist. Continuing test." << endl;
	        result = true;
        }
      }
      cout << TEST_CHECK_BOOL(result) << endl;
      // End thermal trigger power policy test
    }
  }

  cout << "Attempting to disconnect the device..... ";
  cout.flush();
  try {
    result = nmprk::ipmi::disconnectDevice(&d);
  }catch(nmprk::nmprkException* e) {
    DISPLAY_EXCEPTION("disconnectDevice");
    return -1;
  }
  cout << TEST_CHECK_BOOL(result) << endl;
 
  cout << "\nPress the Enter key to exit" << endl;
  getchar();
  return 0;
}

