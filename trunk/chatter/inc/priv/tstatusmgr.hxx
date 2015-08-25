#ifndef __TSTATUS_MGR_HXX__
#define __TSTATUS_MGR_HXX__

#include <map>
#include <list>
#include "mytypes.h"
#include "bridgenet.hxx"	//GetDataFromMsg(...)
#include "chatter.hxx"	//tUpdateFunc

#define CENTRALS_USER_ID	1

class TStatusMgr
{
public:
	/**
	 * @brief	:	Constructor
	 */
	TStatusMgr(tUpdateFunc update_func);
	~TStatusMgr();

	void RegisterInTStatus(tUInt32 userid,string username,tUInt32 tid,tTaskType tasktype,tTState tstate=TASK_STATE_REQUEST_REQUESTED,void* arg1=NULL,tUInt32 total=0);
	void RegisterOutTStatus(tUInt32 userid,string username,tUInt32 tid,tTaskType tasktype,tTState tstate=TASK_STATE_REQUEST_CREATED,void* arg1=NULL,tUInt32 total=0);
	tTStatus* QueryTStatus(tUInt32 tstatusid) const;
	tTStatus* QueryInTStatus(tUInt32 userid, tUInt32 tid) const;
	tTStatus* QueryOutTStatus(tUInt32 userid, tUInt32 tid) const;
	tBool UpdateInTStatus(tUInt32 userid,tUInt32 tid,tTState tstate,tUInt32 increment=0);
	tBool UpdateOutTStatus(tUInt32 userid,tUInt32 tid,tTState tstate,tUInt32 increment=0);

private:
	//
	static tUInt32 GetAUniqueStatusId(void);
	void ClearStales(void);
	void RegisterTStatus(tTStatusList& statuslist,tUInt32 userid,string username,tUInt32 tid,tTaskType tasktype,tTState tstate,void* arg1,tUInt32 total);
	tTStatus* QueryTStatus(const tTStatusList& statuslist,tUInt32 userid,tUInt32 tid) const;
	tBool UpdateTStatus(tTStatusList& statuslist,tUInt32 userid,tUInt32 tid,tTState tstate,tUInt32 increment);

	tTStatusList _tstatus_in_list;
	tTStatusList _tstatus_out_list;

	tUpdateFunc	_update_func;
};
#endif	//__TSTATUS_MGR_HXX__