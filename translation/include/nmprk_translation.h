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
 * Updates:							                                                  *
 * 4/30: prep for initial external release                                *
 **************************************************************************/
 #ifndef NMPRK_TRANSLATION_H
#define NMPRK_TRANSLATION_H

#include "../../ipmi/include/nmprk_ipmi.h"
#include "../../ipmi/include/nmprk_defines.h"
#include "nmprk_translation_defines.h"

namespace nmprk {
 
 namespace translation {
    // setup and settings functions
 	bool                                swSubSystemSetup(nmprk::translation::initType_t, nmprk::ipmi::device*);   // [un]Initilize software subsystems for a device 
                                                                                                                //  or for the library itself
  nmprk::translation::nmVersion_t*    getNMVersion(nmprk::ipmi::device*);                                       // Returns version of a device
  nmprk::translation::capabilities_t* getCapabilities(nmprk::ipmi::device*);                                    // Returns cap of a device
	// power management functions
	bool                                resetStatistics(nmprk::ipmi::device*,nmprk::translation::subSystemComponentType_t,nmprk::translation::policy_t*);                  // Resets a devices statistics
	nmprk::translation::sample_t*       getSample(nmprk::ipmi::device*,nmprk::translation::sampleType_t, nmprk::translation::subSystemComponentType_t, nmprk::translation::policy_t*); 
                                                                                                                       // Returns a sample reading
	nmprk::translation::policy_t*       getPolicy(nmprk::ipmi::device*,nmprk::translation::policy_t*);             // Returns the specified policy
	bool                                setPolicy(nmprk::ipmi::device*,nmprk::translation::policy_t*);             // Sets a policy 
	bool                 	              delPolicy(nmprk::ipmi::device*,nmprk::translation::policy_t*);             // Deletes a policy
	bool                                setPolicyStatus(nmprk::ipmi::device*,nmprk::translation::policy_t*,nmprk::translation::policyStatusType_t);          // Enables / disables a policy
	
	// alerting functions
	bool registerEventHandler(nmprk::ipmi::device*, nmprk::ipmi::sensorFilter_t, nmprk::ipmi::eventFilter_t, nmprk::ipmi::eventHandler_t);
	bool unregisterEventHandler(nmprk::ipmi::device*, nmprk::ipmi::sensorFilter_t, nmprk::ipmi::eventFilter_t);
 };

};

#endif

