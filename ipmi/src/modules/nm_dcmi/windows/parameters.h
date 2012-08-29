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

/*******************************************************************************
***		 File		:	Parameters.h    									  **
***																			  **
********************************************************************************/

#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#define TOOL_REV "1.3"
#define SPEC_REV "1.00"
#define CIPHER_AUTOCHECK 

#include <time.h>

#include <string.h>
#include <stdio.h>
#ifdef WIN32
    #include <conio.h>
#else
    #include <curses.h>
    #include <ncurses.h>
#endif
#include <stdlib.h>
#include <ctype.h>
#include "transport.h"
#include <stdarg.h>
#include <fstream>
#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <iostream>
#include <string>
#include <locale>
using namespace std; 


//Define the length of the Date,Time string
#define MAX_DATE_STR_LEN 9

//Fix to keep Session Alive
#define MAX_RETRY_COUNT 3

using namespace std;

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
#ifndef WIN32
typedef unsigned long  UINT32;
#endif

/*Interface variable defined and can be one of the following choices
 RMCPP
 SOL
 T_MODE
 Host
 NONE
 */
enum interface_type {RMCPP, SOL, HOST_DIRECT_KCS, NONE};

enum transport_plugin_id {TP_RMCPP, TP_KCS};

//Define the BMC Address
#define BMC_ADD  0x20

//Define the requestor LUN, normally 0x00
#define LUN  0x00

//Define the Sequence Number, normally 0x99
#define SEQUENCE_NUM  0x99

//Define the DCMI Extension ID
#define DCMI_EXT_ID  0xDC

//Define the offset variable used for both SEL and SDR reads.
static int offset;

//Define the bytesToRead variable used for both SEL and SDR reads.
static int bytesToRead;


/*Establish Remote Connection
 Need the following to communicate to UUT
 Need IPAddress
 Need User Name
 Need User Password*/
#pragma pack(1)
//User Name, Password, IPAddress required to connect to UUT
//via RMCP and Session interfaces

struct s_dcmi_session_config
{
    char ipAddress[16];
    char userName[16];
    char userPassword[20];

};

//Defining the netfunctions to be used in the DCMI Conformance Test Suite
struct s_dcmi_net_fn
{

    static const int CHASSIS = 0x00;
    static const int APPLICATION = 0x06;
    static const int SENSOR = 0x04;
    static const int STORAGE = 0x0A;
    static const int TRANSPORT = 0x0C;
    static const int DCMI = 0x2C;

};

//Define the struct for return data from issuing the DCMI Discovery Command
struct s_cmd_dcmi_discovery
{
    byte_t extensionId;
    byte_t identificationId;
    byte_t sensorType;
    byte_t entityId;
    byte_t entityInstance;
    byte_t entityStart;
};



//From the record ID pulled from the DCMI discovery command will need
//to load the following parameters for checking of sensor readings and
//comparing to the upper and lower sensor values.
struct s_sdr_data
{

	byte_t sensorType;
	byte_t senseNumber;
    byte_t senseUnits;
    byte_t unrValue;
    byte_t ucValue;
    byte_t uncValue;
    byte_t lnrValue;
    byte_t lcValue;
    byte_t lncValue;
    byte_t readMaskValue;

};

//Define the Mandatory Platform Capabilities returned in
//"Get DCMI Capability Info" command for Parameter 1
// corresponding to Table 6-3 of DCMI Spec ver 1.0
struct s_min_platform_capab
{
	UINT8 bIDSupport:1 ;
	UINT8 bSELLog:1 ;
	UINT8 bChassisPower:1 ;
	UINT8 bTempMonitor:1 ;
	UINT8 Reserved:4 ;
};

//Define the Optional Platform Capabilities returned in
//"Get DCMI Capability Info" command for Parameter 1
// corresponding to Table 6-3 of DCMI Spec ver 1.0

struct s_opt_platform_capab 
{
	UINT8 bPowerMgmt:1;
	UINT8 Reserved:7;
};

//Define the Manageability access Capabilities returned in
//"Get DCMI Capability Info" command for Parameter 1
// corresponding to Table 6-3 of DCMI Spec ver 1.0
struct s_mgmt_access_capab
{
	UINT8 bKCSpresent:1;
	UINT8 bOOBTmodePresent:1;
	UINT8 bOOBSecLanPresent:1;
	UINT8 bOOBPrimLanPresent:1;
	UINT8 bSOLpresent:1;
	UINT8 bVLANpresent:1;
	UINT8 Reserved:2;
};

//Define the Identification Attributes returned in
//"Get DCMI Capability Info" command for Parameter 2
// corresponding to Table 6-3 of DCMI Spec ver 1.0
struct s_ID_Attrib 
{
	UINT8 bGUIDSupport:1 ;
	UINT8 bDHCPHostName:1 ;
	UINT8 bAssetTagSupport:1;
	UINT8 Reserved:5;
};

//Define the Temperature monitoring returned in
//"Get DCMI Capability Info" command for Parameter 2
// corresponding to Table 6-3 of DCMI Spec ver 1.0
struct s_TempMonitor 
{
	UINT8 bInletTemp:1 ;
	UINT8 bProcessorTemp:1;
	UINT8 bBaseboardTemp:1;
	UINT8 Reserved:5;
};

//Defining the return data for byte 6:8
//for GetDCMICapabilitiesInfo command - parameter Selector 1
//corresponds to Table6-3 of DCMI Spec ver 1.0 
struct s_supported_DCMI_capab
{
	struct s_min_platform_capab smin_capab;
    struct s_opt_platform_capab sopt_capab;  
    struct s_mgmt_access_capab  smgmt_capab;
};

//Defining the return data for byte 6:9
//for GetDCMICapabilitiesInfo command - parameter Selector 2
//corresponds to Table6-3 of DCMI Spec ver 1.0 
struct s_Min_Platform_Attrib
{ 
    UINT8 uSELAttrib[2];
	struct s_ID_Attrib sID; 
	struct s_TempMonitor sTempSensor;
};

//Defining the return data for byte 6:7
//for GetDCMICapabilitiesInfo command - parameter Selector 3
//corresponds to Table6-3 of DCMI Spec ver 1.0 
struct s_Opt_Platform_Attrib
{
    UINT8 uPMDevSlaveAddr;
	UINT8 uPMChannelNum;
};

//Defining the return data for byte 6:8
//for GetDCMICapabilitiesInfo command - parameter Selector 4
//corresponds to Table6-3 of DCMI Spec ver 1.0 
struct s_Mgmt_Access_Attrib
{
    UINT8 PrimChannelNum;
	UINT8 SecChannelNum;
	UINT8 TMODEChannelNum;
};

#pragma pack()


//Structure for SEL Capabilities
struct SEL_Cape
{
	int SEL_Auto_Roll_Enabled;		// SEL automatic rollover enabled
    int SEL_Entry;					// number of SEL entries
};

//The Maximum number of EntityIDs supported in DCMI spec is 3
//as Temperature Monitoring in Get DCMI Capabilities Command supports only 3 entities in DCMI 1.0 ver spec
#define MAX_ENTITY_ID	3				//The number of Entity IDs supported in the DCMI Spec 
										//as in table 6-6

//Define the const table 6-6 in DCMI 1.0 spec to associate EntityID with SensorType
const int iEntityIDType[MAX_ENTITY_ID][2] = { {0x40,0x1}, {0x41,0x1}, {0x42,0x1} };

//Define CipherSuites supported in table-6 of DCMI1.0 spec
#define MAX_SPEC_SUPP_CIPHER 9
const int gCipherList[MAX_SPEC_SUPP_CIPHER] = { 0,1,2,3,6,7,8,11,12 };

//Maximum CipherSuites the platform can support as per IPMI currently this could be FF
#define MAX_CIPHER_SUPP	0xFF

//This stores the Sensors supported in the platform
struct s_Sensor_capab
{
	int iEntityID[MAX_ENTITY_ID];		// Array of EntityIDs supported in platform,  (refer to Table 6-6 for list of DCMI supported EntityIDs)
										//This array is initialised as per response in Get DCMI Sensor Info command
	int iEntityType[MAX_ENTITY_ID];     //Store the corresponding Sensor Type of the EntityID iEntityID[x]
	int iLen;							//stores the actual length of the iEntityID[] array 
};

//Structure for DCMI Capabilities describing the Mandatory and Optional fields to handle in the entire conformance suite.
struct Opt_Mand
{
	//Mandatory has value = 'M'.
	//Optional has value = 'O'.
	static const char Cha_Pow = 'M';
	static const char SEL_Log = 'M';
	static const char ID_Sup = 'M';
	static const char Temp_Mon = 'M';
	static const char Power_Manage = 'O';
	static const char SOL_Ena = 'M';
	static const char OOB_Pri = 'M';
	static const char OOB_Sec = 'O';
	static const char OOB_TM = 'O';
	static const char IB_KCS_Ch = 'M';
	static const char SEL_Roll = 'M';
	static const char Ast_Tag = 'O';
	static const char DHCP = 'O';
	static const char GUID_Sup = 'M';
	static const char Pri_LAN_Ch = 'M';
	static const char Sec_LAN_Ch = 'O';
	static const char VLAN	= 'M';
};


extern struct	s_supported_DCMI_capab gsSuppDCMICapab;
extern struct	s_Min_Platform_Attrib  gsMinAttrib;
extern struct	s_Opt_Platform_Attrib  gsOptAttrib;
extern struct   s_Mgmt_Access_Attrib   gsMgmtAttrib;
extern struct   s_dcmi_net_fn s_net_fn;
extern struct   SEL_Cape gsSELRetData;
extern struct   s_Sensor_capab gsSensorList;
extern struct   Opt_Mand gsOM_Ret;

//definitions for Get DCMI Capability Info Cmd
#define	SUPP_DCMI_CAPAB_PARAM	1
#define	MIN_ATTRIB_PARAM		2
#define OPT_ATTRIB_PARAM		3
#define	MGMT_ATTRIB_PARAM		4

//Transport Error values
enum transport_errors
{
    SESS_BAD = 100,
    SESS_TIMEOUT = 101};

//Definitions for Power Management library functions
#define POWER_STATISTICS 0x01

//Following definitions are for return status from IPMI commands / functions
//Transport Error caused by network failure or Packet loss
#define TRANSPORT_ERROR		-1
#define E_SAT_PWR_CTRLR		-2
#define E_CONFIG_LOAD		-3
#define E_INIT_LOGGER		-4
#define E_INVALID_OPTION	-5
#define SUCCESS				0x00

//Privelege Level of Commands
#define PRIV_USER		0x02
#define PRIV_OPERATOR	0x03
#define	PRIV_ADMIN		0x04

//Test Result
#define TEST_PASS		0x00
#define TEST_FAIL		0x01
#define TEST_SKIP		0x02

#ifndef WIN32
#define strcat_s(dest, destSize, src)\
    (strlen(dest) + strlen(src) + 1 > destSize) ? \
        (strlen(dest) + 1 < destSize) ? \
        strncat(dest, src, destSize - strlen(dest) - 1) : dest : strcat(dest, src)
#define	scanf_s scanf
#endif

//define the Cipher array and the var to hold the total number of Ciphers
extern int gTotalCipher;
extern byte_t gPlatformCipherList[MAX_CIPHER_SUPP];
extern int getChannelInfo();
#endif

