

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Fri Aug 13 15:31:56 2010
 */
/* Compiler settings for .\AXChatter.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __AXChatter_h__
#define __AXChatter_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IChatterTabCtrl_FWD_DEFINED__
#define __IChatterTabCtrl_FWD_DEFINED__
typedef interface IChatterTabCtrl IChatterTabCtrl;
#endif 	/* __IChatterTabCtrl_FWD_DEFINED__ */


#ifndef __Ixchatter_FWD_DEFINED__
#define __Ixchatter_FWD_DEFINED__
typedef interface Ixchatter Ixchatter;
#endif 	/* __Ixchatter_FWD_DEFINED__ */


#ifndef __ISinkTabCtrl_FWD_DEFINED__
#define __ISinkTabCtrl_FWD_DEFINED__
typedef interface ISinkTabCtrl ISinkTabCtrl;
#endif 	/* __ISinkTabCtrl_FWD_DEFINED__ */


#ifndef __IChatterMMenu_FWD_DEFINED__
#define __IChatterMMenu_FWD_DEFINED__
typedef interface IChatterMMenu IChatterMMenu;
#endif 	/* __IChatterMMenu_FWD_DEFINED__ */


#ifndef __ISinkMMenu_FWD_DEFINED__
#define __ISinkMMenu_FWD_DEFINED__
typedef interface ISinkMMenu ISinkMMenu;
#endif 	/* __ISinkMMenu_FWD_DEFINED__ */


#ifndef __IRequestList_FWD_DEFINED__
#define __IRequestList_FWD_DEFINED__
typedef interface IRequestList IRequestList;
#endif 	/* __IRequestList_FWD_DEFINED__ */


#ifndef __ISinkRequestList_FWD_DEFINED__
#define __ISinkRequestList_FWD_DEFINED__
typedef interface ISinkRequestList ISinkRequestList;
#endif 	/* __ISinkRequestList_FWD_DEFINED__ */


#ifndef __IGroupList_FWD_DEFINED__
#define __IGroupList_FWD_DEFINED__
typedef interface IGroupList IGroupList;
#endif 	/* __IGroupList_FWD_DEFINED__ */


#ifndef __ISinkGroupList_FWD_DEFINED__
#define __ISinkGroupList_FWD_DEFINED__
typedef interface ISinkGroupList ISinkGroupList;
#endif 	/* __ISinkGroupList_FWD_DEFINED__ */


#ifndef __ISRenderCollection_FWD_DEFINED__
#define __ISRenderCollection_FWD_DEFINED__
typedef interface ISRenderCollection ISRenderCollection;
#endif 	/* __ISRenderCollection_FWD_DEFINED__ */


#ifndef ___IChatterTabCtrlEvents_FWD_DEFINED__
#define ___IChatterTabCtrlEvents_FWD_DEFINED__
typedef interface _IChatterTabCtrlEvents _IChatterTabCtrlEvents;
#endif 	/* ___IChatterTabCtrlEvents_FWD_DEFINED__ */


#ifndef __ChatterTabCtrl_FWD_DEFINED__
#define __ChatterTabCtrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChatterTabCtrl ChatterTabCtrl;
#else
typedef struct ChatterTabCtrl ChatterTabCtrl;
#endif /* __cplusplus */

#endif 	/* __ChatterTabCtrl_FWD_DEFINED__ */


#ifndef __xchatter_FWD_DEFINED__
#define __xchatter_FWD_DEFINED__

#ifdef __cplusplus
typedef class xchatter xchatter;
#else
typedef struct xchatter xchatter;
#endif /* __cplusplus */

#endif 	/* __xchatter_FWD_DEFINED__ */


#ifndef __SinkTabCtrl_FWD_DEFINED__
#define __SinkTabCtrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class SinkTabCtrl SinkTabCtrl;
#else
typedef struct SinkTabCtrl SinkTabCtrl;
#endif /* __cplusplus */

#endif 	/* __SinkTabCtrl_FWD_DEFINED__ */


#ifndef ___IChatterMMenuEvents_FWD_DEFINED__
#define ___IChatterMMenuEvents_FWD_DEFINED__
typedef interface _IChatterMMenuEvents _IChatterMMenuEvents;
#endif 	/* ___IChatterMMenuEvents_FWD_DEFINED__ */


#ifndef __ChatterMMenu_FWD_DEFINED__
#define __ChatterMMenu_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChatterMMenu ChatterMMenu;
#else
typedef struct ChatterMMenu ChatterMMenu;
#endif /* __cplusplus */

#endif 	/* __ChatterMMenu_FWD_DEFINED__ */


#ifndef __SinkMMenu_FWD_DEFINED__
#define __SinkMMenu_FWD_DEFINED__

#ifdef __cplusplus
typedef class SinkMMenu SinkMMenu;
#else
typedef struct SinkMMenu SinkMMenu;
#endif /* __cplusplus */

#endif 	/* __SinkMMenu_FWD_DEFINED__ */


#ifndef ___IRequestListEvents_FWD_DEFINED__
#define ___IRequestListEvents_FWD_DEFINED__
typedef interface _IRequestListEvents _IRequestListEvents;
#endif 	/* ___IRequestListEvents_FWD_DEFINED__ */


#ifndef __RequestList_FWD_DEFINED__
#define __RequestList_FWD_DEFINED__

#ifdef __cplusplus
typedef class RequestList RequestList;
#else
typedef struct RequestList RequestList;
#endif /* __cplusplus */

#endif 	/* __RequestList_FWD_DEFINED__ */


#ifndef __SinkRequestList_FWD_DEFINED__
#define __SinkRequestList_FWD_DEFINED__

#ifdef __cplusplus
typedef class SinkRequestList SinkRequestList;
#else
typedef struct SinkRequestList SinkRequestList;
#endif /* __cplusplus */

#endif 	/* __SinkRequestList_FWD_DEFINED__ */


#ifndef ___IGroupListEvents_FWD_DEFINED__
#define ___IGroupListEvents_FWD_DEFINED__
typedef interface _IGroupListEvents _IGroupListEvents;
#endif 	/* ___IGroupListEvents_FWD_DEFINED__ */


#ifndef __GroupList_FWD_DEFINED__
#define __GroupList_FWD_DEFINED__

#ifdef __cplusplus
typedef class GroupList GroupList;
#else
typedef struct GroupList GroupList;
#endif /* __cplusplus */

#endif 	/* __GroupList_FWD_DEFINED__ */


#ifndef __SinkGroupList_FWD_DEFINED__
#define __SinkGroupList_FWD_DEFINED__

#ifdef __cplusplus
typedef class SinkGroupList SinkGroupList;
#else
typedef struct SinkGroupList SinkGroupList;
#endif /* __cplusplus */

#endif 	/* __SinkGroupList_FWD_DEFINED__ */


#ifndef ___IRenderCollectionEvents_FWD_DEFINED__
#define ___IRenderCollectionEvents_FWD_DEFINED__
typedef interface _IRenderCollectionEvents _IRenderCollectionEvents;
#endif 	/* ___IRenderCollectionEvents_FWD_DEFINED__ */


#ifndef ___ISRenderCollectionEvents_FWD_DEFINED__
#define ___ISRenderCollectionEvents_FWD_DEFINED__
typedef interface _ISRenderCollectionEvents _ISRenderCollectionEvents;
#endif 	/* ___ISRenderCollectionEvents_FWD_DEFINED__ */


#ifndef __SRenderCollection_FWD_DEFINED__
#define __SRenderCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class SRenderCollection SRenderCollection;
#else
typedef struct SRenderCollection SRenderCollection;
#endif /* __cplusplus */

#endif 	/* __SRenderCollection_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IChatterTabCtrl_INTERFACE_DEFINED__
#define __IChatterTabCtrl_INTERFACE_DEFINED__

/* interface IChatterTabCtrl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IChatterTabCtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DA4D1476-C850-466B-AB81-833387F46914")
    IChatterTabCtrl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddItem( 
            ULONG userid,
            BSTR username,
            BSTR myname) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteItem( 
            ULONG userid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCurSel( 
            ULONG userid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurSel( 
            /* [retval][out] */ ULONG *userid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItemCount( 
            LONG *lCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetImageList( 
            LONG pImageList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisplayReceivedUserMsg( 
            ULONG userid,
            BSTR username,
            BSTR myname,
            BSTR instantmsg) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurSelName( 
            /* [retval][out] */ BSTR *username) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisplayNotification( 
            ULONG userid,
            BSTR username,
            BSTR myname,
            BSTR notification) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IChatterTabCtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChatterTabCtrl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChatterTabCtrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChatterTabCtrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IChatterTabCtrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IChatterTabCtrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IChatterTabCtrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IChatterTabCtrl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            IChatterTabCtrl * This,
            ULONG userid,
            BSTR username,
            BSTR myname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IChatterTabCtrl * This,
            ULONG userid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCurSel )( 
            IChatterTabCtrl * This,
            ULONG userid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurSel )( 
            IChatterTabCtrl * This,
            /* [retval][out] */ ULONG *userid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItemCount )( 
            IChatterTabCtrl * This,
            LONG *lCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetImageList )( 
            IChatterTabCtrl * This,
            LONG pImageList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisplayReceivedUserMsg )( 
            IChatterTabCtrl * This,
            ULONG userid,
            BSTR username,
            BSTR myname,
            BSTR instantmsg);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurSelName )( 
            IChatterTabCtrl * This,
            /* [retval][out] */ BSTR *username);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisplayNotification )( 
            IChatterTabCtrl * This,
            ULONG userid,
            BSTR username,
            BSTR myname,
            BSTR notification);
        
        END_INTERFACE
    } IChatterTabCtrlVtbl;

    interface IChatterTabCtrl
    {
        CONST_VTBL struct IChatterTabCtrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChatterTabCtrl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IChatterTabCtrl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IChatterTabCtrl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IChatterTabCtrl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IChatterTabCtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IChatterTabCtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IChatterTabCtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IChatterTabCtrl_AddItem(This,userid,username,myname)	\
    (This)->lpVtbl -> AddItem(This,userid,username,myname)

#define IChatterTabCtrl_DeleteItem(This,userid)	\
    (This)->lpVtbl -> DeleteItem(This,userid)

#define IChatterTabCtrl_SetCurSel(This,userid)	\
    (This)->lpVtbl -> SetCurSel(This,userid)

#define IChatterTabCtrl_GetCurSel(This,userid)	\
    (This)->lpVtbl -> GetCurSel(This,userid)

#define IChatterTabCtrl_GetItemCount(This,lCount)	\
    (This)->lpVtbl -> GetItemCount(This,lCount)

#define IChatterTabCtrl_SetImageList(This,pImageList)	\
    (This)->lpVtbl -> SetImageList(This,pImageList)

#define IChatterTabCtrl_DisplayReceivedUserMsg(This,userid,username,myname,instantmsg)	\
    (This)->lpVtbl -> DisplayReceivedUserMsg(This,userid,username,myname,instantmsg)

#define IChatterTabCtrl_GetCurSelName(This,username)	\
    (This)->lpVtbl -> GetCurSelName(This,username)

#define IChatterTabCtrl_DisplayNotification(This,userid,username,myname,notification)	\
    (This)->lpVtbl -> DisplayNotification(This,userid,username,myname,notification)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_AddItem_Proxy( 
    IChatterTabCtrl * This,
    ULONG userid,
    BSTR username,
    BSTR myname);


void __RPC_STUB IChatterTabCtrl_AddItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_DeleteItem_Proxy( 
    IChatterTabCtrl * This,
    ULONG userid);


void __RPC_STUB IChatterTabCtrl_DeleteItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_SetCurSel_Proxy( 
    IChatterTabCtrl * This,
    ULONG userid);


void __RPC_STUB IChatterTabCtrl_SetCurSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_GetCurSel_Proxy( 
    IChatterTabCtrl * This,
    /* [retval][out] */ ULONG *userid);


void __RPC_STUB IChatterTabCtrl_GetCurSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_GetItemCount_Proxy( 
    IChatterTabCtrl * This,
    LONG *lCount);


void __RPC_STUB IChatterTabCtrl_GetItemCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_SetImageList_Proxy( 
    IChatterTabCtrl * This,
    LONG pImageList);


void __RPC_STUB IChatterTabCtrl_SetImageList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_DisplayReceivedUserMsg_Proxy( 
    IChatterTabCtrl * This,
    ULONG userid,
    BSTR username,
    BSTR myname,
    BSTR instantmsg);


void __RPC_STUB IChatterTabCtrl_DisplayReceivedUserMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_GetCurSelName_Proxy( 
    IChatterTabCtrl * This,
    /* [retval][out] */ BSTR *username);


void __RPC_STUB IChatterTabCtrl_GetCurSelName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IChatterTabCtrl_DisplayNotification_Proxy( 
    IChatterTabCtrl * This,
    ULONG userid,
    BSTR username,
    BSTR myname,
    BSTR notification);


void __RPC_STUB IChatterTabCtrl_DisplayNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IChatterTabCtrl_INTERFACE_DEFINED__ */


#ifndef __Ixchatter_INTERFACE_DEFINED__
#define __Ixchatter_INTERFACE_DEFINED__

/* interface Ixchatter */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_Ixchatter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8B544304-5C4D-4CB3-BA2F-3D01D4A5EEFA")
    Ixchatter : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InstantMsgChat( 
            ULONG userid,
            BSTR username) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MyId( 
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MyId( 
            /* [in] */ ULONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MyName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MyName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvitePeerVoiceChat( 
            ULONG userid,
            BSTR username) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvitePeerVideoChat( 
            ULONG userid,
            BSTR username) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IxchatterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Ixchatter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Ixchatter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Ixchatter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Ixchatter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Ixchatter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Ixchatter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Ixchatter * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InstantMsgChat )( 
            Ixchatter * This,
            ULONG userid,
            BSTR username);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MyId )( 
            Ixchatter * This,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MyId )( 
            Ixchatter * This,
            /* [in] */ ULONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MyName )( 
            Ixchatter * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MyName )( 
            Ixchatter * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Start )( 
            Ixchatter * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InvitePeerVoiceChat )( 
            Ixchatter * This,
            ULONG userid,
            BSTR username);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InvitePeerVideoChat )( 
            Ixchatter * This,
            ULONG userid,
            BSTR username);
        
        END_INTERFACE
    } IxchatterVtbl;

    interface Ixchatter
    {
        CONST_VTBL struct IxchatterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Ixchatter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Ixchatter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Ixchatter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Ixchatter_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Ixchatter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Ixchatter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Ixchatter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Ixchatter_InstantMsgChat(This,userid,username)	\
    (This)->lpVtbl -> InstantMsgChat(This,userid,username)

#define Ixchatter_get_MyId(This,pVal)	\
    (This)->lpVtbl -> get_MyId(This,pVal)

#define Ixchatter_put_MyId(This,newVal)	\
    (This)->lpVtbl -> put_MyId(This,newVal)

#define Ixchatter_get_MyName(This,pVal)	\
    (This)->lpVtbl -> get_MyName(This,pVal)

#define Ixchatter_put_MyName(This,newVal)	\
    (This)->lpVtbl -> put_MyName(This,newVal)

#define Ixchatter_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define Ixchatter_InvitePeerVoiceChat(This,userid,username)	\
    (This)->lpVtbl -> InvitePeerVoiceChat(This,userid,username)

#define Ixchatter_InvitePeerVideoChat(This,userid,username)	\
    (This)->lpVtbl -> InvitePeerVideoChat(This,userid,username)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Ixchatter_InstantMsgChat_Proxy( 
    Ixchatter * This,
    ULONG userid,
    BSTR username);


void __RPC_STUB Ixchatter_InstantMsgChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE Ixchatter_get_MyId_Proxy( 
    Ixchatter * This,
    /* [retval][out] */ ULONG *pVal);


void __RPC_STUB Ixchatter_get_MyId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE Ixchatter_put_MyId_Proxy( 
    Ixchatter * This,
    /* [in] */ ULONG newVal);


void __RPC_STUB Ixchatter_put_MyId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE Ixchatter_get_MyName_Proxy( 
    Ixchatter * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB Ixchatter_get_MyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE Ixchatter_put_MyName_Proxy( 
    Ixchatter * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB Ixchatter_put_MyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Ixchatter_Start_Proxy( 
    Ixchatter * This);


void __RPC_STUB Ixchatter_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Ixchatter_InvitePeerVoiceChat_Proxy( 
    Ixchatter * This,
    ULONG userid,
    BSTR username);


void __RPC_STUB Ixchatter_InvitePeerVoiceChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Ixchatter_InvitePeerVideoChat_Proxy( 
    Ixchatter * This,
    ULONG userid,
    BSTR username);


void __RPC_STUB Ixchatter_InvitePeerVideoChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Ixchatter_INTERFACE_DEFINED__ */


#ifndef __ISinkTabCtrl_INTERFACE_DEFINED__
#define __ISinkTabCtrl_INTERFACE_DEFINED__

/* interface ISinkTabCtrl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISinkTabCtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("74D39396-306F-4139-87E5-C0943A3D707C")
    ISinkTabCtrl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendInstantMsg( 
            ULONG userid,
            BSTR username,
            BSTR instantmsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISinkTabCtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISinkTabCtrl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISinkTabCtrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISinkTabCtrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISinkTabCtrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISinkTabCtrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISinkTabCtrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISinkTabCtrl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SendInstantMsg )( 
            ISinkTabCtrl * This,
            ULONG userid,
            BSTR username,
            BSTR instantmsg);
        
        END_INTERFACE
    } ISinkTabCtrlVtbl;

    interface ISinkTabCtrl
    {
        CONST_VTBL struct ISinkTabCtrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISinkTabCtrl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISinkTabCtrl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISinkTabCtrl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISinkTabCtrl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISinkTabCtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISinkTabCtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISinkTabCtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISinkTabCtrl_SendInstantMsg(This,userid,username,instantmsg)	\
    (This)->lpVtbl -> SendInstantMsg(This,userid,username,instantmsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkTabCtrl_SendInstantMsg_Proxy( 
    ISinkTabCtrl * This,
    ULONG userid,
    BSTR username,
    BSTR instantmsg);


void __RPC_STUB ISinkTabCtrl_SendInstantMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISinkTabCtrl_INTERFACE_DEFINED__ */


#ifndef __IChatterMMenu_INTERFACE_DEFINED__
#define __IChatterMMenu_INTERFACE_DEFINED__

/* interface IChatterMMenu */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IChatterMMenu;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("675826B7-B0D2-4709-AEDD-A2064F963350")
    IChatterMMenu : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IChatterMMenuVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChatterMMenu * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChatterMMenu * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChatterMMenu * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IChatterMMenu * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IChatterMMenu * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IChatterMMenu * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IChatterMMenu * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IChatterMMenuVtbl;

    interface IChatterMMenu
    {
        CONST_VTBL struct IChatterMMenuVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChatterMMenu_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IChatterMMenu_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IChatterMMenu_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IChatterMMenu_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IChatterMMenu_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IChatterMMenu_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IChatterMMenu_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IChatterMMenu_INTERFACE_DEFINED__ */


#ifndef __ISinkMMenu_INTERFACE_DEFINED__
#define __ISinkMMenu_INTERFACE_DEFINED__

/* interface ISinkMMenu */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISinkMMenu;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C675AD60-8C0B-48D5-BAAB-EB7475492418")
    ISinkMMenu : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InviteCurVoiceChat( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InviteCurVideoChat( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransferCurFile( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransferCurFileOffline( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISinkMMenuVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISinkMMenu * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISinkMMenu * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISinkMMenu * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISinkMMenu * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISinkMMenu * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISinkMMenu * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISinkMMenu * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InviteCurVoiceChat )( 
            ISinkMMenu * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InviteCurVideoChat )( 
            ISinkMMenu * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TransferCurFile )( 
            ISinkMMenu * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TransferCurFileOffline )( 
            ISinkMMenu * This);
        
        END_INTERFACE
    } ISinkMMenuVtbl;

    interface ISinkMMenu
    {
        CONST_VTBL struct ISinkMMenuVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISinkMMenu_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISinkMMenu_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISinkMMenu_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISinkMMenu_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISinkMMenu_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISinkMMenu_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISinkMMenu_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISinkMMenu_InviteCurVoiceChat(This)	\
    (This)->lpVtbl -> InviteCurVoiceChat(This)

#define ISinkMMenu_InviteCurVideoChat(This)	\
    (This)->lpVtbl -> InviteCurVideoChat(This)

#define ISinkMMenu_TransferCurFile(This)	\
    (This)->lpVtbl -> TransferCurFile(This)

#define ISinkMMenu_TransferCurFileOffline(This)	\
    (This)->lpVtbl -> TransferCurFileOffline(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkMMenu_InviteCurVoiceChat_Proxy( 
    ISinkMMenu * This);


void __RPC_STUB ISinkMMenu_InviteCurVoiceChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkMMenu_InviteCurVideoChat_Proxy( 
    ISinkMMenu * This);


void __RPC_STUB ISinkMMenu_InviteCurVideoChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkMMenu_TransferCurFile_Proxy( 
    ISinkMMenu * This);


void __RPC_STUB ISinkMMenu_TransferCurFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkMMenu_TransferCurFileOffline_Proxy( 
    ISinkMMenu * This);


void __RPC_STUB ISinkMMenu_TransferCurFileOffline_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISinkMMenu_INTERFACE_DEFINED__ */


#ifndef __IRequestList_INTERFACE_DEFINED__
#define __IRequestList_INTERFACE_DEFINED__

/* interface IRequestList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IRequestList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2527B1B3-8EB1-420B-9046-F14EFE08BB14")
    IRequestList : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddItem( 
            ULONG tstatusid,
            BSTR tittle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteItem( 
            ULONG tstatusid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetButtonYesNo( 
            ULONG tstatusid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetButtonYesAsNo( 
            ULONG tstatusid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetButtonCancel( 
            ULONG tstatusid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProgress( 
            ULONG tstatusid,
            ULONG percentage) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRequestListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRequestList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRequestList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRequestList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IRequestList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IRequestList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IRequestList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IRequestList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            IRequestList * This,
            ULONG tstatusid,
            BSTR tittle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IRequestList * This,
            ULONG tstatusid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetButtonYesNo )( 
            IRequestList * This,
            ULONG tstatusid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetButtonYesAsNo )( 
            IRequestList * This,
            ULONG tstatusid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetButtonCancel )( 
            IRequestList * This,
            ULONG tstatusid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProgress )( 
            IRequestList * This,
            ULONG tstatusid,
            ULONG percentage);
        
        END_INTERFACE
    } IRequestListVtbl;

    interface IRequestList
    {
        CONST_VTBL struct IRequestListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRequestList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRequestList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRequestList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRequestList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRequestList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRequestList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRequestList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRequestList_AddItem(This,tstatusid,tittle)	\
    (This)->lpVtbl -> AddItem(This,tstatusid,tittle)

#define IRequestList_DeleteItem(This,tstatusid)	\
    (This)->lpVtbl -> DeleteItem(This,tstatusid)

#define IRequestList_SetButtonYesNo(This,tstatusid)	\
    (This)->lpVtbl -> SetButtonYesNo(This,tstatusid)

#define IRequestList_SetButtonYesAsNo(This,tstatusid)	\
    (This)->lpVtbl -> SetButtonYesAsNo(This,tstatusid)

#define IRequestList_SetButtonCancel(This,tstatusid)	\
    (This)->lpVtbl -> SetButtonCancel(This,tstatusid)

#define IRequestList_SetProgress(This,tstatusid,percentage)	\
    (This)->lpVtbl -> SetProgress(This,tstatusid,percentage)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IRequestList_AddItem_Proxy( 
    IRequestList * This,
    ULONG tstatusid,
    BSTR tittle);


void __RPC_STUB IRequestList_AddItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IRequestList_DeleteItem_Proxy( 
    IRequestList * This,
    ULONG tstatusid);


void __RPC_STUB IRequestList_DeleteItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IRequestList_SetButtonYesNo_Proxy( 
    IRequestList * This,
    ULONG tstatusid);


void __RPC_STUB IRequestList_SetButtonYesNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IRequestList_SetButtonYesAsNo_Proxy( 
    IRequestList * This,
    ULONG tstatusid);


void __RPC_STUB IRequestList_SetButtonYesAsNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IRequestList_SetButtonCancel_Proxy( 
    IRequestList * This,
    ULONG tstatusid);


void __RPC_STUB IRequestList_SetButtonCancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IRequestList_SetProgress_Proxy( 
    IRequestList * This,
    ULONG tstatusid,
    ULONG percentage);


void __RPC_STUB IRequestList_SetProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRequestList_INTERFACE_DEFINED__ */


#ifndef __ISinkRequestList_INTERFACE_DEFINED__
#define __ISinkRequestList_INTERFACE_DEFINED__

/* interface ISinkRequestList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISinkRequestList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7320AA82-1A46-4A7C-B83D-5DC8403B05EC")
    ISinkRequestList : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AcceptRequest( 
            ULONG tstatusid,
            BSTR additional) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RejectRequest( 
            ULONG tstatusid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CancelTask( 
            ULONG tstatusid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISinkRequestListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISinkRequestList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISinkRequestList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISinkRequestList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISinkRequestList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISinkRequestList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISinkRequestList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISinkRequestList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AcceptRequest )( 
            ISinkRequestList * This,
            ULONG tstatusid,
            BSTR additional);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RejectRequest )( 
            ISinkRequestList * This,
            ULONG tstatusid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CancelTask )( 
            ISinkRequestList * This,
            ULONG tstatusid);
        
        END_INTERFACE
    } ISinkRequestListVtbl;

    interface ISinkRequestList
    {
        CONST_VTBL struct ISinkRequestListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISinkRequestList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISinkRequestList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISinkRequestList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISinkRequestList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISinkRequestList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISinkRequestList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISinkRequestList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISinkRequestList_AcceptRequest(This,tstatusid,additional)	\
    (This)->lpVtbl -> AcceptRequest(This,tstatusid,additional)

#define ISinkRequestList_RejectRequest(This,tstatusid)	\
    (This)->lpVtbl -> RejectRequest(This,tstatusid)

#define ISinkRequestList_CancelTask(This,tstatusid)	\
    (This)->lpVtbl -> CancelTask(This,tstatusid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkRequestList_AcceptRequest_Proxy( 
    ISinkRequestList * This,
    ULONG tstatusid,
    BSTR additional);


void __RPC_STUB ISinkRequestList_AcceptRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkRequestList_RejectRequest_Proxy( 
    ISinkRequestList * This,
    ULONG tstatusid);


void __RPC_STUB ISinkRequestList_RejectRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkRequestList_CancelTask_Proxy( 
    ISinkRequestList * This,
    ULONG tstatusid);


void __RPC_STUB ISinkRequestList_CancelTask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISinkRequestList_INTERFACE_DEFINED__ */


#ifndef __IGroupList_INTERFACE_DEFINED__
#define __IGroupList_INTERFACE_DEFINED__

/* interface IGroupList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IGroupList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("350428DE-7230-435A-B054-8037DE185223")
    IGroupList : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddMember( 
            ULONG userid,
            BSTR username,
            BOOL isadmin) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteMember( 
            ULONG userid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMeAdmin( 
            BOOL bAmIAdmin) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Empty( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Size( 
            /* [retval][out] */ LONG *size) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGroupListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGroupList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGroupList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGroupList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGroupList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGroupList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGroupList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGroupList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddMember )( 
            IGroupList * This,
            ULONG userid,
            BSTR username,
            BOOL isadmin);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteMember )( 
            IGroupList * This,
            ULONG userid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMeAdmin )( 
            IGroupList * This,
            BOOL bAmIAdmin);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Empty )( 
            IGroupList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Size )( 
            IGroupList * This,
            /* [retval][out] */ LONG *size);
        
        END_INTERFACE
    } IGroupListVtbl;

    interface IGroupList
    {
        CONST_VTBL struct IGroupListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGroupList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGroupList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGroupList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGroupList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGroupList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGroupList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGroupList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGroupList_AddMember(This,userid,username,isadmin)	\
    (This)->lpVtbl -> AddMember(This,userid,username,isadmin)

#define IGroupList_DeleteMember(This,userid)	\
    (This)->lpVtbl -> DeleteMember(This,userid)

#define IGroupList_SetMeAdmin(This,bAmIAdmin)	\
    (This)->lpVtbl -> SetMeAdmin(This,bAmIAdmin)

#define IGroupList_Empty(This)	\
    (This)->lpVtbl -> Empty(This)

#define IGroupList_Size(This,size)	\
    (This)->lpVtbl -> Size(This,size)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGroupList_AddMember_Proxy( 
    IGroupList * This,
    ULONG userid,
    BSTR username,
    BOOL isadmin);


void __RPC_STUB IGroupList_AddMember_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGroupList_DeleteMember_Proxy( 
    IGroupList * This,
    ULONG userid);


void __RPC_STUB IGroupList_DeleteMember_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGroupList_SetMeAdmin_Proxy( 
    IGroupList * This,
    BOOL bAmIAdmin);


void __RPC_STUB IGroupList_SetMeAdmin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGroupList_Empty_Proxy( 
    IGroupList * This);


void __RPC_STUB IGroupList_Empty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGroupList_Size_Proxy( 
    IGroupList * This,
    /* [retval][out] */ LONG *size);


void __RPC_STUB IGroupList_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGroupList_INTERFACE_DEFINED__ */


#ifndef __ISinkGroupList_INTERFACE_DEFINED__
#define __ISinkGroupList_INTERFACE_DEFINED__

/* interface ISinkGroupList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISinkGroupList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("407D1B4E-ED74-4AB7-83FC-75B1B46ECFFF")
    ISinkGroupList : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteMember( 
            ULONG userid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISinkGroupListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISinkGroupList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISinkGroupList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISinkGroupList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISinkGroupList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISinkGroupList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISinkGroupList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISinkGroupList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteMember )( 
            ISinkGroupList * This,
            ULONG userid);
        
        END_INTERFACE
    } ISinkGroupListVtbl;

    interface ISinkGroupList
    {
        CONST_VTBL struct ISinkGroupListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISinkGroupList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISinkGroupList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISinkGroupList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISinkGroupList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISinkGroupList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISinkGroupList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISinkGroupList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISinkGroupList_DeleteMember(This,userid)	\
    (This)->lpVtbl -> DeleteMember(This,userid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISinkGroupList_DeleteMember_Proxy( 
    ISinkGroupList * This,
    ULONG userid);


void __RPC_STUB ISinkGroupList_DeleteMember_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISinkGroupList_INTERFACE_DEFINED__ */


#ifndef __ISRenderCollection_INTERFACE_DEFINED__
#define __ISRenderCollection_INTERFACE_DEFINED__

/* interface ISRenderCollection */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISRenderCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DAEA2911-2AE1-40CB-9C7B-9F8D3D77E8F5")
    ISRenderCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateRender( 
            BYTE *host,
            ULONG port,
            BYTE *caption) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyRender( 
            BYTE *host,
            ULONG port) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Play( 
            ULONG interval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISRenderCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISRenderCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISRenderCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISRenderCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISRenderCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISRenderCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISRenderCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISRenderCollection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateRender )( 
            ISRenderCollection * This,
            BYTE *host,
            ULONG port,
            BYTE *caption);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyRender )( 
            ISRenderCollection * This,
            BYTE *host,
            ULONG port);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Play )( 
            ISRenderCollection * This,
            ULONG interval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            ISRenderCollection * This);
        
        END_INTERFACE
    } ISRenderCollectionVtbl;

    interface ISRenderCollection
    {
        CONST_VTBL struct ISRenderCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISRenderCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISRenderCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISRenderCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISRenderCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISRenderCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISRenderCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISRenderCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISRenderCollection_CreateRender(This,host,port,caption)	\
    (This)->lpVtbl -> CreateRender(This,host,port,caption)

#define ISRenderCollection_DestroyRender(This,host,port)	\
    (This)->lpVtbl -> DestroyRender(This,host,port)

#define ISRenderCollection_Play(This,interval)	\
    (This)->lpVtbl -> Play(This,interval)

#define ISRenderCollection_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISRenderCollection_CreateRender_Proxy( 
    ISRenderCollection * This,
    BYTE *host,
    ULONG port,
    BYTE *caption);


void __RPC_STUB ISRenderCollection_CreateRender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISRenderCollection_DestroyRender_Proxy( 
    ISRenderCollection * This,
    BYTE *host,
    ULONG port);


void __RPC_STUB ISRenderCollection_DestroyRender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISRenderCollection_Play_Proxy( 
    ISRenderCollection * This,
    ULONG interval);


void __RPC_STUB ISRenderCollection_Play_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISRenderCollection_Stop_Proxy( 
    ISRenderCollection * This);


void __RPC_STUB ISRenderCollection_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISRenderCollection_INTERFACE_DEFINED__ */



#ifndef __AXChatterLib_LIBRARY_DEFINED__
#define __AXChatterLib_LIBRARY_DEFINED__

/* library AXChatterLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AXChatterLib;

#ifndef ___IChatterTabCtrlEvents_DISPINTERFACE_DEFINED__
#define ___IChatterTabCtrlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IChatterTabCtrlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IChatterTabCtrlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5871BE4C-C2AF-4642-916E-FA3354F0E537")
    _IChatterTabCtrlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IChatterTabCtrlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IChatterTabCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IChatterTabCtrlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IChatterTabCtrlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IChatterTabCtrlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IChatterTabCtrlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IChatterTabCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IChatterTabCtrlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IChatterTabCtrlEventsVtbl;

    interface _IChatterTabCtrlEvents
    {
        CONST_VTBL struct _IChatterTabCtrlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IChatterTabCtrlEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IChatterTabCtrlEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IChatterTabCtrlEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IChatterTabCtrlEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IChatterTabCtrlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IChatterTabCtrlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IChatterTabCtrlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IChatterTabCtrlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ChatterTabCtrl;

#ifdef __cplusplus

class DECLSPEC_UUID("6865EF4A-C109-46D3-91D7-9181DCB13A6F")
ChatterTabCtrl;
#endif

EXTERN_C const CLSID CLSID_xchatter;

#ifdef __cplusplus

class DECLSPEC_UUID("FDED6FA5-6596-4412-A414-5E3A552044AB")
xchatter;
#endif

EXTERN_C const CLSID CLSID_SinkTabCtrl;

#ifdef __cplusplus

class DECLSPEC_UUID("20CF3F07-5EDE-4EA4-8709-FC4C54213FAB")
SinkTabCtrl;
#endif

#ifndef ___IChatterMMenuEvents_DISPINTERFACE_DEFINED__
#define ___IChatterMMenuEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IChatterMMenuEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IChatterMMenuEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C4B825C2-1639-4164-99CE-F3FF6685BE2F")
    _IChatterMMenuEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IChatterMMenuEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IChatterMMenuEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IChatterMMenuEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IChatterMMenuEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IChatterMMenuEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IChatterMMenuEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IChatterMMenuEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IChatterMMenuEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IChatterMMenuEventsVtbl;

    interface _IChatterMMenuEvents
    {
        CONST_VTBL struct _IChatterMMenuEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IChatterMMenuEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IChatterMMenuEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IChatterMMenuEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IChatterMMenuEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IChatterMMenuEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IChatterMMenuEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IChatterMMenuEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IChatterMMenuEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ChatterMMenu;

#ifdef __cplusplus

class DECLSPEC_UUID("F23F1295-D6D4-4C7F-9E28-B2CA23639CC4")
ChatterMMenu;
#endif

EXTERN_C const CLSID CLSID_SinkMMenu;

#ifdef __cplusplus

class DECLSPEC_UUID("A5F7955F-03A9-4F31-B607-9C60B4151F54")
SinkMMenu;
#endif

#ifndef ___IRequestListEvents_DISPINTERFACE_DEFINED__
#define ___IRequestListEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IRequestListEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IRequestListEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3D8D539A-F7E3-45EA-9E51-6ABC682C3057")
    _IRequestListEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IRequestListEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IRequestListEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IRequestListEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IRequestListEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IRequestListEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IRequestListEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IRequestListEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IRequestListEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IRequestListEventsVtbl;

    interface _IRequestListEvents
    {
        CONST_VTBL struct _IRequestListEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IRequestListEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IRequestListEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IRequestListEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IRequestListEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IRequestListEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IRequestListEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IRequestListEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IRequestListEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_RequestList;

#ifdef __cplusplus

class DECLSPEC_UUID("8022EC6E-129C-4AC4-BBC1-BE1B9EAA9142")
RequestList;
#endif

EXTERN_C const CLSID CLSID_SinkRequestList;

#ifdef __cplusplus

class DECLSPEC_UUID("60E03A65-6B94-4F49-A4C7-400952AD7925")
SinkRequestList;
#endif

#ifndef ___IGroupListEvents_DISPINTERFACE_DEFINED__
#define ___IGroupListEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IGroupListEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IGroupListEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D1165812-DD97-4DC8-9F6B-2973FEF8E3D5")
    _IGroupListEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IGroupListEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IGroupListEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IGroupListEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IGroupListEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IGroupListEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IGroupListEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IGroupListEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IGroupListEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IGroupListEventsVtbl;

    interface _IGroupListEvents
    {
        CONST_VTBL struct _IGroupListEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IGroupListEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IGroupListEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IGroupListEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IGroupListEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IGroupListEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IGroupListEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IGroupListEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IGroupListEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_GroupList;

#ifdef __cplusplus

class DECLSPEC_UUID("87AAE186-4FB5-4353-8DAA-65872C4DAC73")
GroupList;
#endif

EXTERN_C const CLSID CLSID_SinkGroupList;

#ifdef __cplusplus

class DECLSPEC_UUID("B801A6AE-D819-46B4-9946-648475124BB9")
SinkGroupList;
#endif

#ifndef ___IRenderCollectionEvents_DISPINTERFACE_DEFINED__
#define ___IRenderCollectionEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IRenderCollectionEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IRenderCollectionEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("63757BCA-A84D-401C-AFEE-183C5620983C")
    _IRenderCollectionEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IRenderCollectionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IRenderCollectionEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IRenderCollectionEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IRenderCollectionEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IRenderCollectionEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IRenderCollectionEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IRenderCollectionEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IRenderCollectionEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IRenderCollectionEventsVtbl;

    interface _IRenderCollectionEvents
    {
        CONST_VTBL struct _IRenderCollectionEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IRenderCollectionEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IRenderCollectionEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IRenderCollectionEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IRenderCollectionEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IRenderCollectionEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IRenderCollectionEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IRenderCollectionEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IRenderCollectionEvents_DISPINTERFACE_DEFINED__ */


#ifndef ___ISRenderCollectionEvents_DISPINTERFACE_DEFINED__
#define ___ISRenderCollectionEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISRenderCollectionEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISRenderCollectionEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("67630F9B-35F6-41E6-A226-9E4D35AAFA0D")
    _ISRenderCollectionEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISRenderCollectionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISRenderCollectionEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISRenderCollectionEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISRenderCollectionEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISRenderCollectionEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISRenderCollectionEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISRenderCollectionEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISRenderCollectionEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISRenderCollectionEventsVtbl;

    interface _ISRenderCollectionEvents
    {
        CONST_VTBL struct _ISRenderCollectionEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISRenderCollectionEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ISRenderCollectionEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ISRenderCollectionEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ISRenderCollectionEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ISRenderCollectionEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ISRenderCollectionEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ISRenderCollectionEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISRenderCollectionEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SRenderCollection;

#ifdef __cplusplus

class DECLSPEC_UUID("A17D0285-990D-4C85-8711-E6BD32088F87")
SRenderCollection;
#endif
#endif /* __AXChatterLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


