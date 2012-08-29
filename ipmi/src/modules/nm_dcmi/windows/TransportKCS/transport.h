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


#ifndef _TRANSPORT_PLUGIN_H
#define _TRANSPORT_PLUGIN_H



#ifndef WIN32
    #include <stdlib.h>
    #include <fcntl.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define ERROR_CLASS_CONFIG    0x0001
#define ERROR_CLASS_COMM      0x0002
#define ERROR_CLASS_OTHER     0x8000

#define MAKE_ERROR_CODE(errorClass, errorCode) ((errorClass << 16) | (errorCode))

#define E_OK                  0x00000000
#define E_OTHER               MAKE_ERROR_CODE(ERROR_CLASS_OTHER, 0x0001)
#define E_INVALID_LEN		  MAKE_ERROR_CODE(ERROR_CLASS_OTHER, 0x0002)   //Length of response not compliant with IPMI/DCMI spec

#define E_INVALID_VALUE       MAKE_ERROR_CODE(ERROR_CLASS_CONFIG, 0x0001)
#define E_NAME_UNKNOWN        MAKE_ERROR_CODE(ERROR_CLASS_CONFIG, 0x0002)

#define E_NETWORK_PROBLEM     MAKE_ERROR_CODE(ERROR_CLASS_COMM, 0x0001)
#define E_NOT_CONFIGURED      MAKE_ERROR_CODE(ERROR_CLASS_COMM, 0x0002)
#define E_NOT_CONNECTED       MAKE_ERROR_CODE(ERROR_CLASS_COMM, 0x0003)


typedef unsigned char byte_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;

#define MAX_IPMI_SIZE 490
#ifdef WIN32
#define EXPORTS	__declspec(dllexport)
#else
#define EXPORTS
#endif

typedef unsigned int IPMI_SESSION_HANDLE;
#define IPMI_SESSION_HANDLE_DEFAULT 0

/**
 * This function type is used for handling information logging.
 * The main application must provide a pointer to a function of this type. 
 * This function is then called by the transport plug-in code to log
 * information to one ore more log files of the main application.
 *
 * The main application decides whether the text shall be written to a log
 * using the level parameter. The following priority levels are defined:
 * - 0 (zero) � error information (must always be written to the logs!)
 * - 5 � warnings
 * - 10 � summary information about DCMI test execution
 * - 50 � detailed information about DCMI command execution
 * - 100 � high-level debug info
 * - 200 � low-level debug info
 * - All other levels are reserved
 *
 * The intention is that the main application writes a particular text to its
 * log output if the information priority level is sufficient. Less important
 * text is not written to the log to keep it more readable.
 *
 * The implementation of this function must be thread-safe as the transport
 * plug-in library may create multiple threads and invoke this function
 * asynchronously.
 *
 * @param level The priority level of the text, as described in the above
 *              description.
 * @param text Pointer to a NULL-terminated text to be written to the log.
 *
 * @retval E_OK Successfull logging.
 * @retval E_OTHER Other error.
 */
typedef int(*dcmi_logger_fun_t)(int level, const char* text);


/**
 *  SOL callback 
 */
typedef int(*dcmi_SOL_recv_t)(const char* data, size_t len);

/**
 * This data structure is used to pass initialization information to the
 * transport plug-in library. 
 */
struct dcmi_init_info_t
{
    /**
     * A pointer to a logger function provided by the main application.
	 * This function may be invoked at anytime by the plug-in code to pass
	 * information to the log of the main application (including debug
	 * information).
     */
    dcmi_logger_fun_t loggerFun;

	 /**
     * A pointer to a SOL callback function provided by the main application.
	 * This function is invoked when a SOL packet is received from the BMC.
     */
	dcmi_SOL_recv_t	  SOLrecv;
};


/**
 * This data structure is used to describe the IPMI/DCMI request command 
 * to be sent to the UUT. Raw data bytes of the command are stored in the 
 * format specified in IPMI v2.0 specification.
 */
struct dcmi_req_t
{
    /**
     * A field that identifies the functional class of the message. 
     * See IPMI v2.0 specification, section 5.1 for more detailed 
     * information.
     */
    byte_t netFun;

    /**
     * Command identifier, as defined in section 5 of IPMI v2.0 specification.
     */
    byte_t cmd;

    /**
     * The array of data bytes that store the additional info of the IPMI 
     * request. Only the first <code>len</code> bytes are relevant.
     */ 
    byte_t data[MAX_IPMI_SIZE];

    /** 
     * The number of relevant data bytes in the <code>data</code> array.
     */
//    int len;
	 size_t len;

    /**
     * Responder�s address (this byte is always 0x20 when the BMC is the 
     * responder). See IPMI v2.0 specification, section 5 for more detailed 
     * information.
     */
    byte_t rsAddr;

    /**
     * Responder�s LUN (usually 0x00). See IPMI v2.0 specification, 
     * section 5.1 for more detailed information.
     */
    byte_t rsLun;
	
};

/**
 * This data structure is used to pass IPMI response from the transport 
 * plug-in to the main application. The response is stored in the raw 
 * format, exactly as defined in IPMI v2.0 specification.
 */
struct dcmi_rsp_t
{
    /**
     * The completion code returned in response to a previous request. 
     * The completion codes are as defined in IPMI v2.0 specification, 
     * section 5.2 and 5.3.
     */
    byte_t compCode;

    /** 
     * The array of data bytes that store the additional info of the IPMI 
     * response (the completion code is not in this array but in the separate 
     * field <code>compCode</code>). Only the first <code>len</code> bytes are 
     * relevant.
     */
    byte_t data[MAX_IPMI_SIZE];

    /**
     * The number of relevant data bytes in the <code>data</code> array.
     */
    size_t len;
};

/**
 * This function is used to initialize the transport library as well as set 
 * initial mandatory configuration parameters, such as logger functionality.
 * This function is normally called once at the initialization. If the 
 * initialization is successful, another call to this function is permitted
 * only after a call to Dcmi_Shutdown().
 *
 * If the initialization information is unacceptable, an error message shall be
 * printed using the logger function and the function shall return with 
 * E_INVALID_VALUE return code. Warnings, if any, shall also be printed using
 * the logger but the function shall not return with an error. Exceptionally,
 * if the logger function pointer is NULL, an error message shall be printed to
 * the standard error stream.
 *
 * The application may invoke other API functions only after successful return
 * from this function. If this function returns with an error, another call to
 * Dcmi_Initialize() is permitted.
 *
 * @param initInfo Pointer to the initialization information data structure.
 *
 * @retval E_OK Successful initialization.
 * @retval E_OTHER Other error.
 */
EXPORTS int Dcmi_Initialize(const dcmi_init_info_t* initInfo);

/**
 * This function is used to let the transport library know that the application
 * is going to unload this library (also, when the application is shutting
 * down). The implementation of the function shall perform a final cleanup,
 * such as freeing allocated memory.
 *
 * No API calls are permitted after invocation of this function. An exception
 * is the Dcmi_Initialize() function, which may be called to re-initialize the library.
 *
 * @retval E_OK Successful shutdown.
 * @retval E_OTHER Other error.
 */
EXPORTS int Dcmi_Shutdown();

/**
 * This function is used to set initial configuration of the transport. 
 * The implementation of this function shall set a configuration setting 
 * named <code>paramName</code> to the value provided in the <code>val</code> 
 * parameter. Configurable settings are, for example, IP address of the target 
 * system, username, password, etc. 
 *
 * If the configurable value shall be a number, the implementation of the function 
 * is responsible for parsing of the val parameter. If the value cannot be parsed, 
 * an error message shall be printed using the logger functionality and the function 
 * shall return with E_INVALID_VALUE return code. Warnings, if any, shall also be 
 * printed using the logger functionality but the function shall not return with 
 * an error.
 *
 * If paramName is not understood no change shall be done in the current settings 
 * of the transport module and the transport module shall not write an error message 
 * using the logger functionality (such an error may be silently ignored by the main 
 * application), but the return code shall be E_NAME_UNKNOWN.
 *
 * @param param_name Name of the configuration parameter.
 * @param val Value of the configuration parameter.
 *
 * @retval E_OK Parameter is known and its value is valid.
 * @retval E_INVALID_VALUE The value cannot be parsed.
 * @retval E_NAME_UNKNOWN Unknown parameter.
 * @retval E_OTHER Other error.
 */
EXPORTS int Dcmi_Configure(const char* paramName, const char* val);

/**
 * This function is used to connect to the UUT. The implementation usually 
 * authenticates itself to the UUT, tests the network connectivity, etc. 
 * Commands may be sent to the UUT only after this function completes successfully. 
 *
 * If an error occurs, a human-readable error description message shall be printed 
 * using the logger functionality and the function shall return with an error. 
 * Warnings, if any, shall also be printed using the logger functionality but the 
 * function shall not return with an error.

 * @retval E_OK Connected successfully.
 * @retval E_NETWORK_PROBLEM Network problem.
 * @retval E_NOT_CONFIGURED Some required configrable parameters not configured.
 * @retval E_OTHER Other error.
 */
EXPORTS int Dcmi_Connect(void);

/**
 * This function is used to send an IPMI/DCMI command to UUT and receive a response. 
 * The response message is put into the data structure pointed by the rsp parameter 
 * (the data structure must be allocated by the caller).
 *
 * Commands may be sent to the UUT only after the <code>Dcmi_Connect()</code> function 
 * completes successfully.

 * @param req Request to be sent.
 * @param rsp Buffer to get the response received.
 *
 * @retval E_OK Command sent successfully.
 * @retval E_NOT_CONNECTED Not connected. 
 * @retval E_NETWORK_PROBLEM Network problem. 
 * @retval E_OTHER Other error.
 */
EXPORTS int Dcmi_SendCmd(const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp);

/**
 * This function is the counterpart of Dcmi_Connect(). It is used to gracefully 
 * disconnect from the UUT. 
 * 
 * If an error occurs, a human-readable error description message shall be 
 * printed using the logger functionality and the function shall return with 
 * an error. Warnings, if any, shall also be printed using the logger functionality 
 * but the function shall not return with an error.
 *
 * @retval E_OK Disconnected successfully.
 * @retval E_NOT_CONNECTED Not connected. 
 * @retval E_NETWORK_PROBLEM Network problem. 
 * @retval E_OTHER Other error.
 */
EXPORTS int Dcmi_Disconnect(void);

/**
 * This function is used to receive data from UUT. This is a blocking call.
 * The response message is put into the data structure pointed by the rsp parameter 
 * (the data structure must be allocated by the caller).
 * In casde of a timeout (specified by the caller), the response is not filled, 
 * but the timeout value is set to true.
 *
 * Commands may be sent to the UUT only after the <code>Dcmi_Connect()</code> function 
 * completes successfully.

 * @param timeoutVal specifies the time should wait before it timing out
 * @param timeout is true if no data was received before a timeout occured.
 * @param rsp Buffer to get the response received.
 *
 * @retval E_OK Command sent successfully.
 * @retval E_NOT_CONNECTED Not connected. 
 * @retval E_NETWORK_PROBLEM Network problem. 
 * @retval E_OTHER Other error.
 */
EXPORTS int Dcmi_RecvData(const struct timeval timeoutVal, bool* timeout, dcmi_rsp_t* rsp);
/**
 * This function is used to send data to UUT. 
 *
 * Commands may be sent to the UUT only after the <code>Dcmi_Connect()</code> function 
 * completes successfully.

 * @param req Request to be sent.
 *
 * @retval E_OK Command sent successfully.
 * @retval E_NOT_CONNECTED Not connected. 
 * @retval E_NETWORK_PROBLEM Network problem. 
 * @retval E_OTHER Other error.
 */
EXPORTS int Dcmi_SendData(const dcmi_req_t* req);



EXPORTS int Ipmi_CreateSession(IPMI_SESSION_HANDLE &h);

EXPORTS int Ipmi_DestroySession(IPMI_SESSION_HANDLE h);

EXPORTS int Ipmi_Configure(IPMI_SESSION_HANDLE h, const char* paramName, const char* val);

EXPORTS int Ipmi_Connect(IPMI_SESSION_HANDLE h);

EXPORTS int Ipmi_SendCmd(IPMI_SESSION_HANDLE h, const struct dcmi_req_t* req, struct dcmi_rsp_t* rsp);

EXPORTS int Ipmi_Disconnect(IPMI_SESSION_HANDLE h);

EXPORTS int Ipmi_RecvData(IPMI_SESSION_HANDLE h, const timeval timeoutVal, bool *timeout, dcmi_rsp_t* rsp);

EXPORTS int Ipmi_SendData(IPMI_SESSION_HANDLE h, const dcmi_req_t* req);


#ifdef __cplusplus
}
#endif

#endif //#ifndef _TRANSPORT_PLUGIN_H
