
#include "nmprkTypes.h"
#include "nmprkUtilities.h"
#include "version.h"

#ifndef WIN32
#define IN
#define OUT
#endif

typedef struct ipmi_capture_req_rsp_t
{
	ipmi_capture_req_t req;
	ipmi_capture_rsp_t rsp;
} ipmi_capture_req_rsp_t;

typedef struct nm_bridging_info_t
{
	byte_t channel;
	byte_t address;
} nm_bridging_info_t;

class transportInterface
{
public:
	virtual ~transportInterface() { }
	virtual nmprk_status_t ConnectLocal(OUT nmprk_conn_handle_t *h) = 0;
	virtual nmprk_status_t ConnectRemote(IN nmprk_connect_remote_parameters_t *input, OUT nmprk_conn_handle_t *h) = 0;
	virtual nmprk_status_t Disconnect(IN nmprk_conn_handle_t h) = 0;
	virtual nmprk_status_t SetDefaultNmCommandBridging(IN nmprk_conn_handle_t h, IN byte_t channel, IN byte_t address) = 0;
	virtual nmprk_status_t SendCommandInternal(IN nmprk_conn_handle_t h, IN bool bridgeIfSet, IN ipmi_req_t *reqA, OUT ipmi_rsp_t *rspA) = 0;
	virtual nmprk_status_t SendBridgedCommand(IN nmprk_conn_handle_t h, IN byte_t channel, IN byte_t address, IN ipmi_req_t *req, OUT ipmi_rsp_t *rsp) = 0;
	virtual nmprk_status_t GetLastRequestResponse(IN nmprk_conn_handle_t h, OUT ipmi_capture_req_t *req, OUT ipmi_capture_rsp_t *rsp) = 0;
	virtual nmprk_status_t GetErrorString(IN nmprk_status_t code, IN OUT char *str, IN int strsize) = 0;
};

