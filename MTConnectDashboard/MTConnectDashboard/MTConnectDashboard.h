

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Mon Feb 25 16:51:03 2013
 */
/* Compiler settings for .\MtConnectDashboard.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
 /* File created by MIDL compiler version 7.00.0555 */
/* at Tue Mar 05 12:35:46 2013
 */
/* Compiler settings for MtConnectDashboard.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 

    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __MtConnectDashboard_h__
#define __MtConnectDashboard_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICMTConnectTrayIcon_FWD_DEFINED__
#define __ICMTConnectTrayIcon_FWD_DEFINED__
typedef interface ICMTConnectTrayIcon ICMTConnectTrayIcon;
#endif 	/* __ICMTConnectTrayIcon_FWD_DEFINED__ */


#ifndef ___IDboardNotificationEvents_FWD_DEFINED__
#define ___IDboardNotificationEvents_FWD_DEFINED__
typedef interface _IDboardNotificationEvents _IDboardNotificationEvents;
#endif 	/* ___IDboardNotificationEvents_FWD_DEFINED__ */


#ifndef __CMTConnectTrayIcon_FWD_DEFINED__
#define __CMTConnectTrayIcon_FWD_DEFINED__

#ifdef __cplusplus
typedef class CMTConnectTrayIcon CMTConnectTrayIcon;
#else
typedef struct CMTConnectTrayIcon CMTConnectTrayIcon;
#endif /* __cplusplus */

#endif 	/* __CMTConnectTrayIcon_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ICMTConnectTrayIcon_INTERFACE_DEFINED__
#define __ICMTConnectTrayIcon_INTERFACE_DEFINED__

/* interface ICMTConnectTrayIcon */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICMTConnectTrayIcon;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4250C680-68DF-4151-A5D3-FB1F887F1211")
    ICMTConnectTrayIcon : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHeaderColumns( 
            BSTR csv) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPointerBrowser( 
            /* [retval][out] */ IUnknown **pWebBrowser) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTrayIconColor( 
            BSTR traycolor,
            /* [in] */ VARIANT_BOOL flashing) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTooltip( 
            BSTR bstr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateTabWindow( 
            BSTR name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddRow( 
            /* [in] */ BSTR csvFields,
            /* [in] */ BSTR csvValues) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIpAddr( 
            BSTR ipaddr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddFaultRow( 
            /* [in] */ BSTR csvFields,
            /* [in] */ BSTR csvValues) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Types( 
            /* [in] */ BSTR csvFields,
            /* [in] */ BSTR csvTypes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Formats( 
            /* [in] */ BSTR csvFields,
            /* [in] */ BSTR csvFormats) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateAlarmTab( 
            /* [in] */ BSTR name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFaultColumns( 
            BSTR csv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICMTConnectTrayIconVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ REFIID riid,
<<<<<<< HEAD
            /* [iid_is][out] */ 
=======
            /* [annotation][iid_is][out] */ 
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICMTConnectTrayIcon * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICMTConnectTrayIcon * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICMTConnectTrayIcon * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHeaderColumns )( 
            ICMTConnectTrayIcon * This,
            BSTR csv);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            ICMTConnectTrayIcon * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPointerBrowser )( 
            ICMTConnectTrayIcon * This,
            /* [retval][out] */ IUnknown **pWebBrowser);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTrayIconColor )( 
            ICMTConnectTrayIcon * This,
            BSTR traycolor,
            /* [in] */ VARIANT_BOOL flashing);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTooltip )( 
            ICMTConnectTrayIcon * This,
            BSTR bstr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateTabWindow )( 
            ICMTConnectTrayIcon * This,
            BSTR name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddRow )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ BSTR csvFields,
            /* [in] */ BSTR csvValues);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetIpAddr )( 
            ICMTConnectTrayIcon * This,
            BSTR ipaddr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddFaultRow )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ BSTR csvFields,
            /* [in] */ BSTR csvValues);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Types )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ BSTR csvFields,
            /* [in] */ BSTR csvTypes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Formats )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ BSTR csvFields,
            /* [in] */ BSTR csvFormats);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateAlarmTab )( 
            ICMTConnectTrayIcon * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetFaultColumns )( 
            ICMTConnectTrayIcon * This,
            BSTR csv);
        
        END_INTERFACE
    } ICMTConnectTrayIconVtbl;

    interface ICMTConnectTrayIcon
    {
        CONST_VTBL struct ICMTConnectTrayIconVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICMTConnectTrayIcon_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICMTConnectTrayIcon_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICMTConnectTrayIcon_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICMTConnectTrayIcon_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICMTConnectTrayIcon_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICMTConnectTrayIcon_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICMTConnectTrayIcon_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICMTConnectTrayIcon_SetHeaderColumns(This,csv)	\
    ( (This)->lpVtbl -> SetHeaderColumns(This,csv) ) 

#define ICMTConnectTrayIcon_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define ICMTConnectTrayIcon_GetPointerBrowser(This,pWebBrowser)	\
    ( (This)->lpVtbl -> GetPointerBrowser(This,pWebBrowser) ) 

#define ICMTConnectTrayIcon_SetTrayIconColor(This,traycolor,flashing)	\
    ( (This)->lpVtbl -> SetTrayIconColor(This,traycolor,flashing) ) 

#define ICMTConnectTrayIcon_SetTooltip(This,bstr)	\
    ( (This)->lpVtbl -> SetTooltip(This,bstr) ) 

#define ICMTConnectTrayIcon_CreateTabWindow(This,name)	\
    ( (This)->lpVtbl -> CreateTabWindow(This,name) ) 

#define ICMTConnectTrayIcon_AddRow(This,csvFields,csvValues)	\
    ( (This)->lpVtbl -> AddRow(This,csvFields,csvValues) ) 

#define ICMTConnectTrayIcon_SetIpAddr(This,ipaddr)	\
    ( (This)->lpVtbl -> SetIpAddr(This,ipaddr) ) 

#define ICMTConnectTrayIcon_AddFaultRow(This,csvFields,csvValues)	\
    ( (This)->lpVtbl -> AddFaultRow(This,csvFields,csvValues) ) 

#define ICMTConnectTrayIcon_Types(This,csvFields,csvTypes)	\
    ( (This)->lpVtbl -> Types(This,csvFields,csvTypes) ) 

#define ICMTConnectTrayIcon_Formats(This,csvFields,csvFormats)	\
    ( (This)->lpVtbl -> Formats(This,csvFields,csvFormats) ) 

#define ICMTConnectTrayIcon_CreateAlarmTab(This,name)	\
    ( (This)->lpVtbl -> CreateAlarmTab(This,name) ) 

#define ICMTConnectTrayIcon_SetFaultColumns(This,csv)	\
    ( (This)->lpVtbl -> SetFaultColumns(This,csv) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICMTConnectTrayIcon_INTERFACE_DEFINED__ */



#ifndef __MtConnectDashboardLib_LIBRARY_DEFINED__
#define __MtConnectDashboardLib_LIBRARY_DEFINED__

/* library MtConnectDashboardLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MtConnectDashboardLib;

#ifndef ___IDboardNotificationEvents_DISPINTERFACE_DEFINED__
#define ___IDboardNotificationEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IDboardNotificationEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IDboardNotificationEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E16BBA79-46DD-4114-AE31-051064BD1513")
    _IDboardNotificationEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IDboardNotificationEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IDboardNotificationEvents * This,
            /* [in] */ REFIID riid,
<<<<<<< HEAD
            /* [iid_is][out] */ 
=======
            /* [annotation][iid_is][out] */ 
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IDboardNotificationEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IDboardNotificationEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IDboardNotificationEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IDboardNotificationEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IDboardNotificationEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IDboardNotificationEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IDboardNotificationEventsVtbl;

    interface _IDboardNotificationEvents
    {
        CONST_VTBL struct _IDboardNotificationEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IDboardNotificationEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IDboardNotificationEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IDboardNotificationEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IDboardNotificationEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IDboardNotificationEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IDboardNotificationEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IDboardNotificationEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IDboardNotificationEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CMTConnectTrayIcon;

#ifdef __cplusplus

class DECLSPEC_UUID("EF0FE7A0-A9EF-4323-B6D8-6F7110B0A5CE")
CMTConnectTrayIcon;
#endif
#endif /* __MtConnectDashboardLib_LIBRARY_DEFINED__ */

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


