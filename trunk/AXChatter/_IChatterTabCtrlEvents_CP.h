#pragma once

template<class T>
class CProxy_IChatterTabCtrlEvents :
	public IConnectionPointImpl<T, &__uuidof(_IChatterTabCtrlEvents)>
{
public:
	HRESULT Fire_SendInstantMsg(LONG userid, BSTR username, BSTR instantmsg)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant avarParams[3];
				avarParams[2] = userid;
				avarParams[2].vt = VT_I4;
				avarParams[1] = username;
				avarParams[1].vt = VT_BSTR;
				avarParams[0] = instantmsg;
				avarParams[0].vt = VT_BSTR;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 3, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};

