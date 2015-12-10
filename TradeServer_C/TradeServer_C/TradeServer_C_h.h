

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Dec 10 15:16:46 2015
 */
/* Compiler settings for TradeServer_C.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
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


#ifndef __TradeServer_C_h_h__
#define __TradeServer_C_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ITradeServer_C_FWD_DEFINED__
#define __ITradeServer_C_FWD_DEFINED__
typedef interface ITradeServer_C ITradeServer_C;
#endif 	/* __ITradeServer_C_FWD_DEFINED__ */


#ifndef __TradeServer_C_FWD_DEFINED__
#define __TradeServer_C_FWD_DEFINED__

#ifdef __cplusplus
typedef class TradeServer_C TradeServer_C;
#else
typedef struct TradeServer_C TradeServer_C;
#endif /* __cplusplus */

#endif 	/* __TradeServer_C_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __TradeServer_C_LIBRARY_DEFINED__
#define __TradeServer_C_LIBRARY_DEFINED__

/* library TradeServer_C */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_TradeServer_C;

#ifndef __ITradeServer_C_DISPINTERFACE_DEFINED__
#define __ITradeServer_C_DISPINTERFACE_DEFINED__

/* dispinterface ITradeServer_C */
/* [uuid] */ 


EXTERN_C const IID DIID_ITradeServer_C;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D0619233-2E26-4068-8BA4-1A83DE9D379A")
    ITradeServer_C : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ITradeServer_CVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITradeServer_C * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITradeServer_C * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITradeServer_C * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITradeServer_C * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITradeServer_C * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITradeServer_C * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITradeServer_C * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ITradeServer_CVtbl;

    interface ITradeServer_C
    {
        CONST_VTBL struct ITradeServer_CVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITradeServer_C_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITradeServer_C_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITradeServer_C_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITradeServer_C_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITradeServer_C_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITradeServer_C_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITradeServer_C_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ITradeServer_C_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_TradeServer_C;

#ifdef __cplusplus

class DECLSPEC_UUID("D5C41A4F-3097-4E63-B095-4BEF012C937B")
TradeServer_C;
#endif
#endif /* __TradeServer_C_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


