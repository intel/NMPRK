#include "stdafx.h"

#include "nmprkTypes.h"
#include "RmcppIpmiProxy.h"

RmcppIpmiProxy::RmcppIpmiProxy(void)
{
	session_ = NULL;
}


RmcppIpmiProxy::~RmcppIpmiProxy(void)
{
	if(session_)
	{
		delete session_;
		session_ = NULL;
	}
}

void RmcppIpmiProxy::Connect(std::string ip, std::string username, std::string password, ipmi_auth_algo auth_algo, ipmi_integ_algo integ_algo, ipmi_confid_algo confid_algo)
{
	session_ = new RmcppIpmiSession(ip.c_str(), username.c_str(), password.c_str(), auth_algo, integ_algo, confid_algo);
	session_->open();
}

void RmcppIpmiProxy::Disconnect()
{
	if(session_ != NULL)
		session_->close();
}

ipmi_rsp_t RmcppIpmiProxy::RequestResponse(const ipmi_req_t& req) throw()
{
	ipmi_rsp_t rsp;
	session_->sendRecv(req, rsp);

	return rsp;
}

ipmi_rsp_t RmcppIpmiProxy::GetAsyncResponse(int msTimeout)
{
	bool timedOut = false;
	timeval timeout;
	ipmi_req_t dummy;
	ipmi_rsp_t rsp;

	timeout.tv_sec = msTimeout / 1000;
	timeout.tv_usec = (msTimeout - (timeout.tv_sec * 1000)) * 1000;

	session_->sendRecv(timeout, false, true, timedOut, dummy, rsp);
	if(timedOut == true)
		throw IpmiProxyException(NMPRK_PROXY_ERROR_NO_RSP, "No Asynchronous Response received");

	return rsp;
}
