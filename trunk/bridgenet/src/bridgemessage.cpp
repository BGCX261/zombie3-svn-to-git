#include "bridgemessage.hxx"

tBridgeMsg* CreateBridgeMsg(void)
{
	return new tBridgeMsg();
}
void ReleaseBridgeMsg(tBridgeMsg** ppmsg)
{
	if(ppmsg!=NULL && NULL!=*ppmsg)
	{
		if ((*ppmsg)->isReceived)
			free((*ppmsg)->msg);
		else
			was_free_msg((void**)(&(*ppmsg)->msg));

		 delete *ppmsg;
		 *ppmsg=NULL;
	} 
}
tBridgeMsg* DuplicateMsg(tBridgeMsg* msg)
{
	if (!msg)
		return 0;

	tBridgeMsg* new_msg = CreateBridgeMsg();

	new_msg->rhost = msg->rhost;
	new_msg->rport = msg->rport;

	new_msg->lom = msg->lom;
	new_msg->mpi = msg->mpi;
	new_msg->rspc = msg->rspc;
	new_msg->tid = msg->tid;
	new_msg->tom = msg->tom;

	new_msg->msg = (tByte*)malloc(sizeof(tByte) * msg->lom);
	memcpy(new_msg->msg, msg->msg, sizeof(tByte) *  msg->lom);

	new_msg->isReceived = FALSE;

	return new_msg;
}
tChar* GetDataFromMsg(tBridgeMsg* msg,E_WAS_TAG_ID tag_id)
{
	tChar* data = NULL;
    E_WAS_TAG_ID tmp_tag_id = E_WAS_TAG_ID_NONE;
    S_WAS_TAG_DATA p_td;

    memset(&p_td, 0x00, sizeof(S_WAS_TAG_DATA));

	while (E_WAS_TAG_ID_NONE != (tmp_tag_id=was_get_next_tag_data(msg->msg, msg->lom, &p_td)))
    {
        if (tmp_tag_id==tag_id)
        {
            break;
        }
    }

    if (tmp_tag_id==E_WAS_TAG_ID_NONE)
    {
        return NULL;
    }

    data=(tChar*)malloc((1+p_td.data.bs.byte_stream_len));
    memset(data, 0x00, p_td.data.bs.byte_stream_len+1);
    memcpy(data, p_td.data.bs.byte_stream, p_td.data.bs.byte_stream_len);

    return data;
}
void ReleaseDataFromMsg(tChar* pdata)
{
	if (pdata)
		free(pdata);
}
