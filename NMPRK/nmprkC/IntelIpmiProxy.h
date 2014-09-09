#ifndef _INTEL_IPMI_PROXY
#define _INTEL_IPMI_PROXY

#include "IpmiProxy.h"

class IntelIpmiProxy : public IpmiProxy
{
public:
	IntelIpmiProxy(void);
	virtual ~IntelIpmiProxy(void);

	ipmi_rsp_t RequestResponse(const ipmi_req_t& req);

	ipmi_rsp_t GetAsyncResponse(int msTimeout);

private:
	DWORD seqNo_;
	byte_t lastRequestChannel_;
};

#endif //_INTEL_IPMI_PROXY

