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
#ifndef NMPRK_ERRCODES_H
#define NMPRK_ERRCODES_H

// Following is for when one of the sub systems throws a exception
// and we arn't sure what it is
// No msg code, it gets set to what ever the 
// exception error message was

#define NMPRK_UNKNOWN_EXCPT_CODE           -1


#define NMPRK_NULL_CODE                       0x00
#define NMPRK_NULL_MSG                        "Function passed a NULL reference"  
#define NMPRK_FAILED_ALLOC_CODE               0x01
#define NMPRK_FAILED_ALLOC_MSG                "Library failed to allocat the memory required for the return structure"
#define NMPRK_INVALID_DOMAIN_CODE             0x02
#define NMPRK_INVALID_DOMAIN_MSG              "Specified domain is not valid for this platform"
#define NMPRK_CMD_FAILED_CODE                 0x03
#define NMPRK_CMD_FAILED_MSG                  "Command failed with Unknown Error"
#define NMPRK_CMD_NOT_SUPPORT_CODE            0x04
#define NMPRK_CMD_NOT_SUPPORT_MSG             "Command / Functionality is not supported on this device"
#define NMPRK_DCMI_NO_MEASUREMENT_CODE        0x05
#define NMPRK_DCMI_NO_MEASUREMENT_MSG         "No Measurements Available"
#define NMPRK_NM_DCMI_NO_DEV_CODE             0x06
#define NMPRK_NM_DCMI_NO_DEV_MSG              "Could not open device at /dev/ipmi0, /dev/ipmi/0 or /dev/ipmidev/0"
#define NMPRK_NM_DCMI_NO_EVENT_RCV_CODE       0x07
#define NMPRK_NM_DCMI_NO_EVENT_RCV_MSG        "Could not enable event receiver"
#define NMPRK_NM_DCMI_NOSET_IPMB_ADDY_CODE    0x08
#define NMPRK_NM_DCMI_NOSET_IPMB_ADDY_MSG     "Could not set IPMB address"
#define NMPRK_NM_DCMI_UNABLE_SND_CMD_CODE     0x09
#define NMPRK_NM_DCMI_UNABLE_SND_CMD_MSG      "Unable to send command"
#define NMPRK_NM_DCMI_IO_ERR_SND_CMD_CODE     0x0A
#define NMPRK_NM_DCMI_IO_ERR_SND_CMD_MSG      "I/O Error Getting CMD RSP"
#define NMPRK_NM_DCMI_NO_DATA_RSP_CODE        0x0B
#define NMPRK_NM_DCMI_NO_DATA_RSP_MSG         "No data available while getting CMD RSP"
#define NMPRK_CNVRT_TS_FAILED_CODE            0x0C
#define NMPRK_CNVRT_TS_FAILED_MSG             "Function to return struct TM from timestamp returned NULL.  Is SEL time set correctly?"
// THE FOLLOWING IS USED 
// to dynamically generated to include the 
// non zero exit code
#define NMPRK_CMD_RETURNED_NON_ZERO_CODE      0x0D
#define NMPRK_CMD_RETURNED_NON_ZERO_MSG       "CMD Returned back a non zero completion code: "
#define NMPRK_REQ_NOT_ENOUGH_ARGS_CODE        0x10
#define NMPRK_REQ_NOT_ENOUGH_ARGS_MSG         "nmprk::ipmi::commandReq_t* req did not hold enough arguments, at least 2 are required to specify Net Function and cmd"
#define NMPRK_FAILED_INIT_DLL_CODE	          0x11
#define NMPRK_FAILED_INIT_DLL_MSG             "Failed to initialize nmct code plug-in libraries"
#define NMPRK_FAILED_OPEN_KCS_CODE	          0x12
#define NMPRK_FAILED_OPEN_KCS_MSG             "KCS Host Interface connect returned an error" 
#define NMPRK_FAILED_OPEN_REMOTE_CODE	        0x13
#define NMPRK_FAILED_OPEN_REMOTE_MSG          "RMCPP Host Interface connect returned an error"
#define NMPRK_DEV_NOT_CONNECTED_CODE	        0x14
#define NMPRK_DEV_NOT_CONNECTED_MSG           "Device was not connected before operations were performed"
// LINUX LAN SPECIFIC ERRORS
#define NMPRK_LNX_LAN_NO_PING_CODE	          0x15
#define NMPRK_LNX_LAN_NO_PING_MSG             "Unable to send IPMI presence ping packet"
#define NMPRK_LNX_LAN_NO_LAN_INTF_CODE	      0x16
#define NMPRK_LNX_LAN_NO_LAN_INTF_MSG         "Failed to open LAN interface"
#define NMPRK_LNX_LAN_NO_RMTE_RSP_CODE	      0x17
#define NMPRK_LNX_LAN_NO_RMTE_RSP_MSG         "No response from remote controller"
#define NMPRK_LNX_LAN_INVLD_RSP_PKT_CODE      0x18
#define NMPRK_LNX_LAN_INVLD_RSP_PKT_MSG       "Invalid response packet"
#define NMPRK_LNX_LAN_PKT_SND_FAIL_CODE	      0x19
#define NMPRK_LNX_LAN_PKT_SND_FAIL_MSG        "Packet send failed"
#define NMPRK_LNX_LAN_INTERNAL_ERR_CODE	      0x1a
#define NMPRK_LNX_LAN_INTERNAL_ERR_MSG        "Lnx Lan portion of library had a fatal error: "
#define NMPRK_LNX_LAN_UNABLE_TO_EST_CODE      0x1b
#define NMPRK_LNX_LAN_UNABLE_TO_EST_MSG       "Unable to establish a LAN session"
#define NMPRK_LNX_LAN_CLSE_SES_FAIL_CODE      0x1c
#define NMPRK_LNX_LAN_CLSE_SES_FAIL_MSG       "Close Session Command Failed "
#define NMPRK_LNX_LAN_CLSE_SES_FAIL_BAD_SES_ID_CODE 0x1d
#define NMPRK_LNX_LAN_CLSE_SES_FAIL_BAD_SES_ID_MSG  "Failed to close session due to bad session id : "
// LAN / OOB General Errors
#define NMPRK_LAN_INVALID_USER_CODE	          0x20
#define NMPRK_LAN_INVALID_USER_MSG	          "Invalid user name"
#define NMPRK_LAN_NO_NULL_USER_CODE	          0x21
#define NMPRK_LAN_NO_NULL_USER_MSG	          "NULL user name not enabled"
#define NMPRK_LAN_GET_SES_CHAL_CMD_FAIL_CODE  0x22
#define NMPRK_LAN_GET_SES_CHAL_CMD_FAIL_MSG   "Get Session Challenge command failed:"
#define NMPRK_LAN_NO_SES_SLOTS_CODE	          0x23
#define NMPRK_LAN_NO_SES_SLOTS_MSG	          "No session slot available"
#define NMPRK_LAN_NO_USR_SES_SLOTS_CODE	      0x24
#define NMPRK_LAN_NO_USR_SES_SLOTS_MSG	      "No slot available for given user - limit reached"
#define NMPRK_LAN_NO_USR_PRIV_SLOTS_CODE      0x25
#define NMPRK_LAN_NO_USR_PRIV_SLOTS_MSG	      "No slot available to support user due to maximum privilege capacity"
#define NMPRK_LAN_USR_REQ_PRIV_EXCEED_CODE    0x26
#define NMPRK_LAN_USR_REQ_PRIV_EXCEED_MSG     "Requested privilege level exceeds limit"
#define NMPRK_LAN_USR_PRIV_INSUFF_CODE	      0x27
#define NMPRK_LAN_USR_PRIV_INSUFF_MSG	        "Insufficient privilege level"
#define NMPRK_LAN_SET_SES_PRIV_LVL_FAIL_CODE  0x28
#define NMPRK_LAN_SET_SES_PRIV_LVL_FAIL_MSG   "Set Session Privilege Level Failed"
#define NMPRK_LAN_SCKT_CONNECT_FAIL_CODE	    0x29
#define NMPRK_LAN_SCKT_CONNECT_FAIL_MSG       "Socket Connection Failed"
#define NMPRK_LAN_SCKT_CREATE_FAILED_CODE     0x2a
#define NMPRK_LAN_SCKT_CREATE_FAILED_MSG      "Socket Creation Failed"
#define NMPRK_LAN_ADDY_LOOKUP_FAILED_CODE	    0x2b
#define NMPRK_LAN_ADDY_LOOKUP_FAILED_MSG      "Address look up failed"
#define NMPRK_LAN_NO_ADDY_SPECIFIED_CODE      0x2c
#define NMPRK_LAN_NO_ADDY_SPECIFIED_MSG       "No hostname specified"
#define NMPRK_INVALID_CHECKSUM_CODE           0x2d
#define NMPRK_INVALID_CHECKSUM_MSG            "Invalid checksum found in the FRU common area"

#define NMPRK_NM_INVALID_POLICY_ID            0x80
#define NMPRK_NM_INVALID_POLICY_ID_MSG        "Invalid policy ID"
#define NMPRK_NM_INVALID_DOMAIN               0x81
#define NMPRK_NM_INVALID_DOMAIN_MSG	          "Invalid domain specified"
#define NMPRK_NM_INVALID_POLICY_TRIG          0x82
#define NMPRK_NM_INVALID_POLICY_TRIG_MSG      "Unsupported policy trigger specified"
#define NMPRK_NM_UNKNOWN_POLICY_TYPE          0x83
#define NMPRK_NM_UNKNOWN_POLICY_TYPE_MSG      "Unknown policy type specified"
#define NMPRK_NM_PWR_LIMIT_OUT_RANGE          0x84
#define NMPRK_NM_PWR_LIMIT_OUT_RANGE_MSG      "Power Limit out of range"
#define NMPRK_NM_COR_TIME_OUT_RANGE           0x85
#define NMPRK_NM_COR_TIME_OUT_RANGE_MSG       "Correction Time out of range"
#define NMPRK_NM_POL_TRIG_OUT_RANGE           0x86
#define NMPRK_NM_POL_TRIG_OUT_RANGE_MSG       "Policy trigger value out of range"
#define NMPRK_NM_INVALID_MODE                 0x88
#define NMPRK_NM_INVALID_MODE_MSG             "Invalid mode"
#define NMPRK_NM_STAT_REPORT_OUT_RANGE        0x89
#define NMPRK_NM_STAT_REPORT_OUT_RANGE_MSG    "Statistics reporting period out of range"
#define NMPRK_NM_INVALID_AGGRESSIVE_BIT       0x8B
#define NMPRK_NM_INVALID_AGGRESSIVE_BIT_MSG   "Invalid aggressive power correction field"

// Define Standard IPMI return codes and messages
#define IPMI_NODE_BUSY                        0xC0
#define IPMI_NODE_BUSY_MSG                    "Node busy, could not process command"
#define IPMI_INVALID_COMMAND                  0xC1
#define IPMI_INVALID_COMMAND_MSG              "Invalid or unsupported command"
#define IPMI_INVALID_LUN                      0xC2
#define IPMI_INVALID_LUN_MSG                  "Invalid command for specified LUN"
#define IPMI_TIMEOUT_PROCESS                  0xC3
#define IPMI_TIMEOUT_PROCESS_MSG              "Timeout while processing command"
#define IPMI_LACK_OF_STORAGE                  0xC4
#define IPMI_LACK_OF_STORAGE_MSG              "Command could not be processed due to lack of storage space"
#define IPMI_RESERVATION_CANCELED             0xC5
#define IPMI_RESERVATION_CANCELED_MSG         "Reservation canceled or invalid reservation ID"
#define IPMI_REQ_DATA_TRUNCATED               0xC6
#define IPMI_REQ_DATA_TRUNCATED_MSG           "Request data truncated"
#define IPMI_DATA_LENGTH_INVALID              0xC7
#define IPMI_DATA_LENGTH_INVALID_MSG          "Request data length invalid"
#define IPMI_DATA_LENGTH_EXCEEDED             0xC8
#define IPMI_DATA_LENGTH_EXCEEDED_MSG         "Request data field length limit exceeded"
#define IPMI_PARAMETER_OUT_OF_RANGE           0xC9
#define IPMI_PARAMETER_OUT_OF_RANGE_MSG       "Parameter out of range"
#define IPMI_CANNOT_RETURN_DATA               0xCA
#define IPMI_CANNOT_RETURN_DATA_MSG           "Cannot return number of requested data bytes"
#define IPMI_SENSOR_NOT_PRESENT               0xCB
#define IPMI_SENSOR_NOT_PRESENT_MSG           "Requested sensor, data or record not present"
#define IPMI_INVALID_DATA_FIELD               0xCC
#define IPMI_INVALID_DATA_FIELD_MSG           "Invalid data field in request"
#define IPMI_ILLEGAL_FOR_SENSOR               0xCD
#define IPMI_ILLEGAL_FOR_SENSOR_MSG           "Command illegal for specified sensor or record type"
#define IPMI_COULD_NOT_BE_PROVIDED            0xCE
#define IPMI_COULD_NOT_BE_PROVIDED_MSG        "Command response could not be provided"
#define IPMI_DUPLICATE_REQUEST                0xCF
#define IPMI_DUPLICATE_REQUEST_MSG            "Cannot execute duplicated request"
#define IPMI_SDR_IN_UPDATE_MODE               0xD0
#define IPMI_SDR_IN_UPDATE_MODE_MSG           "Response could not be provided, SDR repository in update mode"
#define IPMI_FW_IN_UPDATE_MODE                0xD1
#define IPMI_FW_IN_UPDATE_MODE_MSG            "Response could not be provided, device in firmware update mode"
#define IPMI_BMC_INITIALIZATION               0xD2
#define IPMI_BMC_INITIALIZATION_MSG           "Response could not be provided, BMC initialization in progress"
#define IPMI_DESTINATION_UNAVAIL              0xD3
#define IPMI_DESTINATION_UNAVAIL_MSG          "Cannot deliver request to selected destination"
#define IPMI_INSUFFICIENT_PRIV                0xD4
#define IPMI_INSUFFICIENT_PRIV_MSG            "Insufficient privilege level to execute command"
#define IPMI_UNSUPPORT_PRES_STATE             0xD5
#define IPMI_UNSUPPORT_PRES_STATE_MSG         "Command not supported in present state"
#define IPMI_SUB_FUNC_DISABLED                0xD6
#define IPMI_SUB_FUNC_DISABLED_MSG            "Parameter is illegal because subfunction is disabled"

#define NMPRK_NM_UNABLE_DISABLE_POL_CODE      0xda
#define NMPRK_NM_UNABLE_DISABLE_POL_MSG       "Unable to disable policy"

#define NMPRK_BRIDGE_IPMIRECV_ERR_CODE		    0xe0
#define NMPRK_BRIDGE_IPMIRECV_ERR_MSG		      "AUTO GENERATED BY CODE. THIS IS NOT USED"
#define NMPRK_BRIDGE_RECV_TIMEOUT_CODE		    0xe1
#define NMPRK_BRIDGE_RECV_TIMEOUT_MSG         "No data recieved after 5 seconds, aborting!"
#define NMPRK_BRIDGE_RECV_DATA_ERR_CODE		    0xe2
#define NMPRK_BRIDGE_RECV_DATA_ERR_MSG        "AUTO GENERATED BY CODE. THIS IS NOT USED"

#define NMPRK_NOT_IMPLEMNETED_CODE	          0xff
#define NMPRK_NOT_IMPLEMNETED_MSG             "This Code and or Function is not currently implemneted"

#endif

