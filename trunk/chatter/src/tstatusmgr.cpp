#include "priv/tstatusmgr.hxx"

TStatusMgr::TStatusMgr(tUpdateFunc update_func)
:_tstatus_in_list(),
 _tstatus_out_list(),
 _update_func(update_func)
{}
TStatusMgr::~TStatusMgr()
{}
void TStatusMgr::RegisterInTStatus(tUInt32 userid,string username,tUInt32 tid,tTaskType tasktype,tTState tstate,void* arg1,tUInt32 total)
{
	return RegisterTStatus(_tstatus_in_list,userid,username,tid,tasktype,tstate,arg1,total);
}
void TStatusMgr::RegisterOutTStatus(tUInt32 userid,string username,tUInt32 tid,tTaskType tasktype,tTState tstate,void* arg1,tUInt32 total)
{
	return RegisterTStatus(_tstatus_out_list,userid,username,tid,tasktype,tstate,arg1,total);;
}
tTStatus* TStatusMgr::QueryTStatus(tUInt32 tstatusid) const
{
	tTStatusListConstIter iter;
	tTStatus* tstatus = NULL;
	
	iter = _tstatus_in_list.begin();
	while (iter!= _tstatus_in_list.end())
	{
		if ((*iter)->tstatusid == tstatusid)
		{
			tstatus = (*iter);
			break;
		}
		else 
			++iter;
	}

	if (tstatus)
		return tstatus;

	iter = _tstatus_out_list.begin();
	while (iter!= _tstatus_out_list.end())
	{
		if ((*iter)->tstatusid == tstatusid)
		{
			tstatus = (*iter);
			break;
		}
		else 
			++iter;
	}

	return tstatus;
}
tTStatus* TStatusMgr::QueryInTStatus(tUInt32 userid, tUInt32 tid) const
{
	return QueryTStatus(_tstatus_in_list,userid,tid);
}
tTStatus* TStatusMgr::QueryOutTStatus(tUInt32 userid, tUInt32 tid) const
{
	return QueryTStatus(_tstatus_out_list,userid,tid);
}
tBool TStatusMgr::UpdateInTStatus(tUInt32 userid,tUInt32 tid,tTState tstate,tUInt32 increment)
{
	return UpdateTStatus(_tstatus_in_list,userid,tid,tstate,increment);
}
tBool TStatusMgr::UpdateOutTStatus(tUInt32 userid,tUInt32 tid,tTState tstate,tUInt32 increment)
{
	return UpdateTStatus(_tstatus_out_list,userid,tid,tstate,increment);
}
void TStatusMgr::ClearStales(void)
{
	tTStatusListIter iter;
	tTStatus* tstatus = NULL;
	
	iter = _tstatus_in_list.begin();
	while (iter!= _tstatus_in_list.end())
	{
		tstatus = (*iter);

		if (tstatus->tstate==TASK_STATE_FINISHED)
		{
			delete tstatus;
			tstatus = NULL;
			iter = _tstatus_in_list.erase(iter);
		}
		else 
			++iter;
	}

	iter = _tstatus_out_list.begin();
	while (iter!= _tstatus_out_list.end())
	{
		tstatus = (*iter);

		if (tstatus->tstate==TASK_STATE_FINISHED)
		{
			delete tstatus;
			tstatus = NULL;
			iter = _tstatus_out_list.erase(iter);
		}
		else 
			++iter;
	}
}
void TStatusMgr::RegisterTStatus(tTStatusList& statuslist,tUInt32 userid,string username,tUInt32 tid,tTaskType tasktype,tTState tstate,void* arg1,tUInt32 total)
{
	static tUInt32 CLEAR_COUNTER_TOP=100;
	static tUInt32 clearcounter = 0;

	tTStatus* tstatus;

	tstatus = new tTStatus;

	tstatus->tstatusid = GetAUniqueStatusId();
	tstatus->userid = userid;
	tstatus->username = username;
	tstatus->tid = tid;
	tstatus->tasktype = tasktype;
	tstatus->tstate = tstate;
	tstatus->arg1 = arg1;
	tstatus->total = total;
	tstatus->finished = 0;

	statuslist.push_front(tstatus);

	if (_update_func)
	{
		_update_func(tstatus->tstatusid);
	}

	/**since it's time-consuming,do it every 100 times.*/
	if (++clearcounter==CLEAR_COUNTER_TOP)
	{
		clearcounter = 0;
		ClearStales();
	}
}
tTStatus* TStatusMgr::QueryTStatus(const tTStatusList& statuslist,tUInt32 userid, tUInt32 tid) const
{
	tTStatus* tstatus=NULL;

	tTStatusListConstIter iter = statuslist.begin();
	while (iter!=statuslist.end())
	{
		if ((*iter)->userid==userid && (*iter)->tid==tid)
		{
			tstatus = (*iter);
			break;
		}
		else
			++iter;
	}

	return tstatus;
}
tBool TStatusMgr::UpdateTStatus(tTStatusList& statuslist,tUInt32 userid,tUInt32 tid,tTState tstate,tUInt32 increment)
{
	tTStatus* tstatus = NULL;

	if (!(tstatus = QueryTStatus(statuslist,userid,tid)))
		return FALSE;

	tstatus->tstate = tstate;
	tstatus->finished += increment;	

	if (_update_func)
		_update_func(tstatus->tstatusid);

	return TRUE;
}
tUInt32 TStatusMgr::GetAUniqueStatusId(void)
{
	const static tUInt32 STATUS_ID_TOP = 60000;
	const static tUInt32 STATUS_ID_BOTTOM = 1;
	static tUInt32 counter = STATUS_ID_BOTTOM;

	if (counter>STATUS_ID_TOP)
		counter = STATUS_ID_BOTTOM;

	return counter++;
}
