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
 #ifndef NMPRK_TRANSLATION_DCMI_H
#define NMPRK_TRANSLATION_DCMI_H


#include "../../../include/nmprk_translation.h"
#include "../../../include/nmprk_translation_defines.h"
#include "../../../../ipmi/include/nmprk_ipmi.h"

namespace nmprk {
 
 namespace translation {

  namespace dcmi {
   
   bool                                dcmi_swSubSystemSetup(nmprk::translation::initType_t, nmprk::ipmi::device*);   // [un]Initilize software subsystems for a device 
                                                                                                                      //  or for the library itself
   nmprk::translation::capabilities_t* dcmi_getCapabilities(nmprk::ipmi::device*);                                    // Returns cap of a device
   bool                                dcmi_resetStatistics(nmprk::ipmi::device*,nmprk::translation::subSystemComponentType_t,nmprk::translation::policy_t*);                  // Resets a devices statistics
   nmprk::translation::sample_t*       dcmi_getSample(nmprk::ipmi::device*,nmprk::translation::sampleType_t, nmprk::translation::subSystemComponentType_t, nmprk::translation::policy_t*); 
                                                                                                                      // Returns a sample reading
   nmprk::translation::policy_t*       dcmi_getPolicy(nmprk::ipmi::device*,nmprk::translation::policy_t*);             // Returns the specified policy
   bool                                dcmi_setPolicy(nmprk::ipmi::device*,nmprk::translation::policy_t*);             // Sets a policy 
   bool                 	       dcmi_delPolicy(nmprk::ipmi::device*,nmprk::translation::policy_t*);             // Deletes a policy
   bool                                dcmi_setPolicyStatus(nmprk::ipmi::device*,nmprk::translation::policy_t*,nmprk::translation::policyStatusType_t);          // Enables / disables a policy
  };

 };

};
#endif

