#pragma once

template<class T>
class CProxy_IGroupListEvents :
	public IConnectionPointImpl<T, &__uuidof(_IGroupListEvents)>
{
public:
	HRESULT Fire_DeleteMember(ULONG userid)
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
				CComVariant avarParams[1];
				avarParams[0] = userid;
				avarParams[0].vt = VT_UI4;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 1, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};

