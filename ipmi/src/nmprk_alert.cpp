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
#include <nmprk_alert.h>
#include <nmprk_ipmi.h>
#include <nmprk_defines.h>
#include <nmprk_exception.h>
#include <nmprk_errCodes.h>

#include <stdio.h>
#include <string>
#include <vector>

typedef struct {
 nmprk::ipmi::device*        d;
 nmprk::ipmi::sensorFilter_t sensorFilter;
 nmprk::ipmi::eventFilter_t  eventFilter;
 nmprk::ipmi::eventHandler_t handler;
}eventEntry;

std::vector<eventEntry> eventList;
static bool scanStarted = false;

void scanRecords() {
	throw new nmprk::nmprkException(NMPRK_NOT_IMPLEMNETED_CODE,NMPRK_NOT_IMPLEMNETED_MSG);
}

bool nmprk::ipmi::alert::registerEventHandler(nmprk::ipmi::device* d, nmprk::ipmi::sensorFilter_t sensorF, nmprk::ipmi::eventFilter_t eventF, nmprk::ipmi::eventHandler_t handler)  {
 bool ret = true;
 if(eventList.size() == 0) {

 }
 eventEntry ee;
 ee.d = d;
 ee.sensorFilter = sensorF;
 ee.eventFilter = eventF;
 ee.handler = handler;
 eventList.push_back(ee);
 return ret;
}

bool nmprk::ipmi::alert::unregisterEventHandler(nmprk::ipmi::device* d, nmprk::ipmi::sensorFilter_t sensorF, nmprk::ipmi::eventFilter_t eventF) {
 bool ret = false;

 std::vector<eventEntry>::iterator it = eventList.begin();

 int index = 0;
 bool found = false;
 for(; it != eventList.end() ; it++) {
	 if(it->d == d && it->eventFilter == eventF && it->sensorFilter == sensorF){
		 found = true;
		 break;
	 }
 }

 if(found == true) {
	 eventList.erase(eventList.begin() + index);
	 ret = true;
 }
 
 return ret;
}