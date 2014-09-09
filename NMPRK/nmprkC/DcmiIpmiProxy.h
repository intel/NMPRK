#ifndef _DCMI_IPMI_PROXY
#define _DCMI_IPMI_PROXY

#include "IpmiProxy.h"

class DcmiIpmiProxy : public IpmiProxy
{
public:
	DcmiIpmiProxy(void);
	virtual ~DcmiIpmiProxy(void);

	ipmi_rsp_t RequestResponse(const ipmi_req_t& req);

	ipmi_rsp_t GetAsyncResponse(int msTimeout);

private:
	HANDLE h_ipmiDrv;
};

#endif //_DCMI_IPMI_PROXY

