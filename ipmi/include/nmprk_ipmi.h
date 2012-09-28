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

#ifndef NMPRK_IPMI_H
#define NMPRK_IPMI_H

#include <string>
#include "nmprk_defines.h"

namespace nmprk {

  namespace ipmi {

    typedef enum {
      //device_dcmi = 0x01,       // jrm, removed support for DCMI.
      //device_dnm = 0x02,				// jrm, removed option for DNM support as it was never implemented.
      device_nm = 0x03
    }deviceType_t;

	typedef enum
	{
    cipherSuite_0 = 0,
	  cipherSuite_1 = 1,
	  cipherSuite_2 = 2,
	  cipherSuite_3 = 3,
	  cipherSuite_6 = 6,
	  cipherSuite_7 = 7,
	  cipherSuite_8 = 8,
	  cipherSuite_11 = 11,
	  cipherSuite_12 = 12
	  /* All others are not supported */
	}ipmiCipherSuite_t;

    static byte_t defaultBridge = -1;
    static byte_t defaultTrans  = 0x20;
    static std::string defaultLocal = "local";
	static ipmiCipherSuite_t defaultCipherSuite = cipherSuite_3;


    class device {
    public:
      deviceType_t type;
      std::string  address;
      std::string  user;
      std::string  password;
      byte_t       bridge;
      byte_t       transport;

	  ipmiCipherSuite_t	cipher;

	  // how we access on linux
      void*        intf;
	  // how we access on windows;
	  unsigned int handle;
    //device() : type(device_dcmi), address("local"), user(""), password(""), bridge(defaultBridge), transport(defaultTrans), cipher(defaultCipherSuite), intf(NULL)
    device() : type(device_nm), address("local"), user(""), password(""), bridge(defaultBridge), transport(defaultTrans), cipher(defaultCipherSuite), intf(NULL)
    {};

	  device(deviceType_t Type, std::string Address,std::string User,std::string Password):
	    type(Type), address(Address), user(User), password(Password), bridge(defaultBridge), transport(defaultTrans), cipher(defaultCipherSuite), intf(NULL)
		{}

    device(deviceType_t Type, std::string Address,std::string User,std::string Password,byte_t Bridge,byte_t Transport):
      type(Type), address(Address), user(User), password(Password), bridge(Bridge), transport(Transport), cipher(defaultCipherSuite), intf(NULL)
		{}

	  device(deviceType_t Type, std::string Address,std::string User,std::string Password,byte_t Bridge,byte_t Transport, ipmiCipherSuite_t Cipher):
      type(Type), address(Address), user(User), password(Password), bridge(Bridge), transport(Transport), cipher(Cipher), intf(NULL)
		{}
    };

    bool connectDevice(device* d);
    bool disconnectDevice(device* d);
    bool runIpmiCommand(device* d, commandReq_t* req,commandRsp_t* rsp);
    bool initSystemForLocal();
    bool ipmi_rspCodes(int rspCode);

  };

};

#endif
