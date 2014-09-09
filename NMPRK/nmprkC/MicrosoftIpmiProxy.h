#ifndef _MICROSOFT_IPMI_PROXY
#define _MICROSOFT_IPMI_PROXY

#include <atlbase.h>
#include <wbemidl.h>
#include <comdef.h>
#include <SetupAPI.h>
#include <queue>

#include "nmprkTypes.h"
#include "IpmiProxy.h"

using namespace std;

class MicrosoftIpmiProxy : public IpmiProxy
{
public:
	MicrosoftIpmiProxy(void);
	virtual ~MicrosoftIpmiProxy(void);

	ipmi_rsp_t RequestResponse(const ipmi_req_t& req);

	ipmi_rsp_t GetAsyncResponse(int msTimeout);

private:
	void initialize();
	void cleanup();

	ipmi_rsp_t RequestResponse_internal(const ipmi_req_t& req);

	void ExecutionThread();
	static DWORD WINAPI ExecutionThread(LPVOID lpParameter)
	{
		reinterpret_cast<MicrosoftIpmiProxy*>(lpParameter)->ExecutionThread();
		return 0;
    }
	HANDLE m_ExecutionThreadHandle;
	bool m_ExecutionThreadExited;

private:
	/// Ptr to the IWbemServices interface.
    CComPtr<IWbemServices> wbemServices;

    /// Path the Microsoft_IPMI instance.
    bstr_t ipmiInstancePath;

    /// BMC Address retrieved from the Microsoft_IPMI instance.
    byte_t bmcAddress;

    /// Signature of [in] parameters to the Microsoft_IPMI::RequestResponse.
    CComPtr<IWbemClassObject> inParamsDef;

private:

	CRITICAL_SECTION m_reqMutex;
	CRITICAL_SECTION m_rspMutex;
	HANDLE m_reqEvent;
	HANDLE m_exitEvent;
	HANDLE m_rspEvent;
	std::queue<ipmi_req_t> reqQueue;
	std::queue<ipmi_rsp_t> rspQueue;

	ipmi_req_t getReq()
	{
		EnterCriticalSection(&m_reqMutex);
		ipmi_req_t req = reqQueue.front();
		reqQueue.pop();
		LeaveCriticalSection(&m_reqMutex);
		return req;
	}

	void postReq(ipmi_req_t req)
	{
		EnterCriticalSection(&m_reqMutex);
		reqQueue.push(req);
		LeaveCriticalSection(&m_reqMutex);
		SetEvent(m_reqEvent);
	}

	ipmi_rsp_t getRsp()
	{
		EnterCriticalSection(&m_rspMutex);
		ipmi_rsp_t rsp = rspQueue.front();
		rspQueue.pop();
		LeaveCriticalSection(&m_rspMutex);
		return rsp;
	}

	void postRsp(ipmi_rsp_t rsp)
	{
		EnterCriticalSection(&m_rspMutex);
		rspQueue.push(rsp);
		LeaveCriticalSection(&m_rspMutex);
		SetEvent(m_rspEvent);
	}
};

#endif //_MICROSOFT_IPMI_PROXY

