
#include "transportInterface.h"

#ifdef USE_TI_ORIGINAL
#include <map>
#include "nmprkC.h"
#include "nmprk_ipmi.h"
#include "nmprk_defines.h"
#include "nmprk_exception.h"
#include "nmprk_errCodes.h"
#include "nmprk_helper.h"
#include "nmprk_translation.h"
#include "nmprk_translation_defines.h"
#include "nmprk_ipmi_global.h"
#include "nmprk_ipmi_sel.h"
#include "nmprk_ipmi_sdr.h"
#include "nmprk_ipmi_fru.h"
//#include "nm_dnm.h"

class transportInterfaceOriginal : public transportInterface
{
public:
	nmprk_status_t ConnectLocal(OUT nmprk_conn_handle_t *h);
	nmprk_status_t ConnectRemote(IN nmprk_connect_remote_parameters_t *input, OUT nmprk_conn_handle_t *h);
	nmprk_status_t Disconnect(IN nmprk_conn_handle_t h);
	nmprk_status_t SetDefaultNmCommandBridging(IN nmprk_conn_handle_t h, IN byte_t channel, IN byte_t address);
	nmprk_status_t SendCommandInternal(IN nmprk_conn_handle_t h, IN bool bridgeIfSet, IN ipmi_req_t *reqA, OUT ipmi_rsp_t *rspA);
	nmprk_status_t SendBridgedCommand(IN nmprk_conn_handle_t h, IN byte_t channel, IN byte_t address, IN ipmi_req_t *req, OUT ipmi_rsp_t *rsp);
	nmprk_status_t GetLastRequestResponse(IN nmprk_conn_handle_t h, OUT ipmi_capture_req_t *req, OUT ipmi_capture_rsp_t *rsp);
	nmprk_status_t GetErrorString(IN nmprk_status_t code, IN OUT char *str, IN int strsize);

public:
	// These are temporary until they can be implemented with standard interface
	nmprk_status_t NMPRK_GetDiscoveryParameters(IN nmprk_conn_handle_t h, OUT nm_discovery_parameters_t *params);
	nmprk_status_t NMPRK_IPMI_GetDeviceId(IN nmprk_conn_handle_t h, OUT nm_ipmi_device_id_t *devId);
	nmprk_status_t NMPRK_IPMI_GetSdrInfo(IN nmprk_conn_handle_t h, OUT nm_ipmi_repo_info_t *info);
	nmprk_status_t NMPRK_IPMI_GetSdrRecord(IN nmprk_conn_handle_t h, IN ushort_t recordId, OUT nm_ipmi_record_t *record);
	nmprk_status_t NMPRK_IPMI_AddSdrRecord(IN nmprk_conn_handle_t h, IN nm_ipmi_record_t *record, OUT ushort_t *recordId);
	nmprk_status_t NMPRK_IPMI_DeleteSdrRecord(IN nmprk_conn_handle_t h, IN ushort_t recordId);
	nmprk_status_t NMPRK_IPMI_ClearSdrRepository(IN nmprk_conn_handle_t h);
	nmprk_status_t NMPRK_IPMI_GetSelInfo(IN nmprk_conn_handle_t h, OUT nm_ipmi_repo_info_t *info);
	nmprk_status_t NMPRK_IPMI_GetSelEntry(IN nmprk_conn_handle_t h, IN ushort_t recordId, OUT nm_ipmi_sel_entry_t *entry);
	nmprk_status_t NMPRK_IPMI_DeleteSelEntry(IN nmprk_conn_handle_t h, IN ushort_t recordId);
	nmprk_status_t NMPRK_IPMI_ClearSel(IN nmprk_conn_handle_t h);
	nmprk_status_t NMPRK_IPMI_GetFruInfo(IN nmprk_conn_handle_t h, OUT nm_ipmi_fru_info_t *output);
	nmprk_status_t NMPRK_IPMI_ReadFruData(IN nmprk_conn_handle_t h, IN int_t offset, IN OUT int_t *length, OUT byte_t *data);
	nmprk_status_t NMPRK_IPMI_WriteFruData(IN nmprk_conn_handle_t h, IN int_t offset, IN OUT int_t *length, IN byte_t *data);
	nmprk_status_t NMPRK_IPMI_SetAcpiPowerState(IN nmprk_conn_handle_t h, IN ipmi_acpi_power_state_t *input);
	nmprk_status_t NMPRK_IPMI_GetAcpiPowerState(IN nmprk_conn_handle_t h, OUT ipmi_acpi_power_state_t *output);

private:
	void addDevice(nmprk_conn_handle_t h, nmprk::ipmi::device *d);
	void removeDeviceByHandle(nmprk_conn_handle_t h);
	nmprk::ipmi::device *getDeviceByHandle(nmprk_conn_handle_t h);
	void saveLastReqRsp(nmprk::ipmi::device *d);
	void clearLastReqRsp(nmprk::ipmi::device *d);
	bool runIpmiCommandWrapper(nmprk::ipmi::device *d, nmprk::ipmi::commandReq_t *req, nmprk::ipmi::commandRsp_t *rsp);

private:	
	std::map<nmprk_conn_handle_t, nmprk::ipmi::device *> gRemoteDeviceMap;
	nmprk::mutex gRemoveDeviceMapLock;
	std::map<nmprk::ipmi::device *, ipmi_capture_req_rsp_t> gHandleLastReqRspMap;
	std::map<nmprk_conn_handle_t, nm_bridging_info_t> gHandleBridgingMap;
};
#endif
