#ifndef _RMCPP_IPMI_PROXY
#define _RMCPP_IPMI_PROXY

#include "IpmiProxy.h"
#include "RmcppIpmiSession.h"

class RmcppIpmiProxy : public IpmiProxy
{
public:
	RmcppIpmiProxy(void);
	virtual ~RmcppIpmiProxy(void);

	ipmi_rsp_t RequestResponse(const ipmi_req_t& req) throw();

	ipmi_rsp_t GetAsyncResponse(int msTimeout);

	void Connect(std::string ip, std::string username, std::string password, ipmi_auth_algo auth_algo, ipmi_integ_algo integ_algo, ipmi_confid_algo confid_algo);

	void Disconnect();

private:
	RmcppIpmiSession *session_;
};

#endif //_RMCPP_IPMI_PROXY

