

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


<<<<<<< HEAD
 /* File created by MIDL compiler version 7.00.0500 */
/* at Mon Feb 25 16:51:03 2013
 */
/* Compiler settings for .\MtConnectDboardGui.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
=======
 /* File created by MIDL compiler version 7.00.0555 */
/* at Tue Mar 05 12:35:46 2013
 */
/* Compiler settings for MtConnectDboardGui.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
<<<<<<< HEAD
//@@MIDL_FILE_HEADING(  )
=======
/* @@MIDL_FILE_HEADING(  ) */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ICMTConnectTrayIcon,0x4250C680,0x68DF,0x4151,0xA5,0xD3,0xFB,0x1F,0x88,0x7F,0x12,0x11);


MIDL_DEFINE_GUID(IID, LIBID_MtConnectDboardGuiLib,0x44220923,0x6EDE,0x4d93,0xB3,0x76,0x72,0x0C,0xD6,0x9E,0xD0,0xA6);


MIDL_DEFINE_GUID(IID, DIID__IDboardNotificationEvents,0xE16BBA79,0x46DD,0x4114,0xAE,0x31,0x05,0x10,0x64,0xBD,0x15,0x13);


MIDL_DEFINE_GUID(CLSID, CLSID_CMTConnectTrayIcon,0xEF0FE7A0,0xA9EF,0x4323,0xB6,0xD8,0x6F,0x71,0x10,0xB0,0xA5,0xCE);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



