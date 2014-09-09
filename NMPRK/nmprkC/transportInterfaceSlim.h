#include "transportInterface.h"

#include <map>
#include "IpmiProxy.h"
#include "RmcppIpmiProxy.h"

using namespace std;

class transportInterfaceSlim : public transportInterface
{
public:
	transportInterfaceSlim();
	nmprk_status_t ConnectLocal(OUT nmprk_conn_handle_t *h);
	nmprk_status_t ConnectRemote(IN nmprk_connect_remote_parameters_t *input, OUT nmprk_conn_handle_t *h);
	nmprk_status_t Disconnect(IN nmprk_conn_handle_t h);
	nmprk_status_t SetDefaultNmCommandBridging(IN nmprk_conn_handle_t h, IN byte_t channel, IN byte_t address);
	nmprk_status_t SendCommandInternal(IN nmprk_conn_handle_t h, IN bool bridgeIfSet, IN ipmi_req_t *reqA, OUT ipmi_rsp_t *rspA);
	nmprk_status_t SendBridgedCommand(IN nmprk_conn_handle_t h, IN byte_t channel, IN byte_t address, IN ipmi_req_t *req, OUT ipmi_rsp_t *rsp);
	nmprk_status_t GetLastRequestResponse(IN nmprk_conn_handle_t h, OUT ipmi_capture_req_t *req, OUT ipmi_capture_rsp_t *rsp);
	nmprk_status_t GetErrorString(IN nmprk_status_t code, IN OUT char *str, IN int strsize);

private:
	IpmiProxy *getProxyByHandle(nmprk_conn_handle_t h);
	void saveLastReq(ipmi_req_t *req);
	void saveLastRsp(ipmi_rsp_t *rsp);

private:
	std::map<nmprk_conn_handle_t, IpmiProxy *> proxyMap;
	Mutex proxyMapLock;
	std::string lastErrorDetails;
	Mutex sendRecvLock;
	int handleAllocator;
	std::map<nmprk_conn_handle_t, nm_bridging_info_t> handleBridgingMap;
	ipmi_capture_req_t lastReq;
	ipmi_capture_rsp_t lastRsp;
};
