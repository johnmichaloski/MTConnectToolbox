

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
<<<<<<< HEAD
#endif // __RPCPROXY_H_VERSION__
=======
#endif /* __RPCPROXY_H_VERSION__ */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d


#include "MtConnectDboardGui.h"

#define TYPE_FORMAT_STRING_SIZE   61                                
#define PROC_FORMAT_STRING_SIZE   493                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

typedef struct _MtConnectDboardGui_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MtConnectDboardGui_MIDL_TYPE_FORMAT_STRING;

typedef struct _MtConnectDboardGui_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MtConnectDboardGui_MIDL_PROC_FORMAT_STRING;

typedef struct _MtConnectDboardGui_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } MtConnectDboardGui_MIDL_EXPR_FORMAT_STRING;


<<<<<<< HEAD
static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
=======
static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MtConnectDboardGui_MIDL_TYPE_FORMAT_STRING MtConnectDboardGui__MIDL_TypeFormatString;
extern const MtConnectDboardGui_MIDL_PROC_FORMAT_STRING MtConnectDboardGui__MIDL_ProcFormatString;
extern const MtConnectDboardGui_MIDL_EXPR_FORMAT_STRING MtConnectDboardGui__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ICMTConnectTrayIcon_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ICMTConnectTrayIcon_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
<<<<<<< HEAD
#error You need a Windows 2000 or later to run this stub because it uses these features:
=======
#error You need Windows 2000 or later to run this stub because it uses these features:
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MtConnectDboardGui_MIDL_PROC_FORMAT_STRING MtConnectDboardGui__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure SetHeaderColumns */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x1 ),	/* 1 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter csv */

/* 24 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clear */

/* 36 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x8 ),	/* 8 */
/* 44 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 52 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 60 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 62 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 64 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPointerBrowser */

/* 66 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 68 */	NdrFcLong( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x9 ),	/* 9 */
/* 74 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 76 */	NdrFcShort( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x8 ),	/* 8 */
/* 80 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 82 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 84 */	NdrFcShort( 0x0 ),	/* 0 */
/* 86 */	NdrFcShort( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pWebBrowser */

/* 90 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 92 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 94 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */

/* 96 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 98 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTrayIconColor */

/* 102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0xa ),	/* 10 */
/* 110 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 112 */	NdrFcShort( 0x6 ),	/* 6 */
/* 114 */	NdrFcShort( 0x8 ),	/* 8 */
/* 116 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 118 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 122 */	NdrFcShort( 0x4 ),	/* 4 */
=======
/* 122 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter traycolor */

/* 126 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 128 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 130 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter flashing */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 136 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 140 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTooltip */

/* 144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0xb ),	/* 11 */
/* 152 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 158 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 160 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 164 */	NdrFcShort( 0x4 ),	/* 4 */
=======
/* 164 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstr */

/* 168 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 172 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 174 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateTabWindow */

/* 180 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0xc ),	/* 12 */
/* 188 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x8 ),	/* 8 */
/* 194 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 196 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 198 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 200 */	NdrFcShort( 0x4 ),	/* 4 */
=======
/* 200 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 202 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter name */

/* 204 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 206 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 208 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 212 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddRow */

/* 216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 222 */	NdrFcShort( 0xd ),	/* 13 */
/* 224 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 230 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 232 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 234 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 236 */	NdrFcShort( 0x8 ),	/* 8 */
=======
/* 236 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter csvFields */

/* 240 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 242 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 244 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter csvValues */

/* 246 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 248 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 250 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 254 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetIpAddr */

/* 258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0xe ),	/* 14 */
/* 266 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 272 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 274 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 278 */	NdrFcShort( 0x4 ),	/* 4 */
=======
/* 278 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ipaddr */

/* 282 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 284 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 286 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 290 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddFaultRow */

/* 294 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0xf ),	/* 15 */
/* 302 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 308 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 310 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 314 */	NdrFcShort( 0x8 ),	/* 8 */
=======
/* 314 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter csvFields */

/* 318 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 320 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 322 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter csvValues */

/* 324 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 326 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 328 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 330 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 332 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Types */

/* 336 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 338 */	NdrFcLong( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x10 ),	/* 16 */
/* 344 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 350 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 352 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 356 */	NdrFcShort( 0x8 ),	/* 8 */
=======
/* 356 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter csvFields */

/* 360 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 362 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 364 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter csvTypes */

/* 366 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 368 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 370 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 374 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Formats */

/* 378 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x11 ),	/* 17 */
/* 386 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 390 */	NdrFcShort( 0x8 ),	/* 8 */
/* 392 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 394 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 398 */	NdrFcShort( 0x8 ),	/* 8 */
=======
/* 398 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 400 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter csvFields */

/* 402 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 404 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 406 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter csvFormats */

/* 408 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 410 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 412 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 414 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 416 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateAlarmTab */

/* 420 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 422 */	NdrFcLong( 0x0 ),	/* 0 */
/* 426 */	NdrFcShort( 0x12 ),	/* 18 */
/* 428 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 432 */	NdrFcShort( 0x8 ),	/* 8 */
/* 434 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 436 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 438 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 440 */	NdrFcShort( 0x4 ),	/* 4 */
=======
/* 440 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 442 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter name */

/* 444 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 446 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 448 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 450 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 452 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 454 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFaultColumns */

/* 456 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 458 */	NdrFcLong( 0x0 ),	/* 0 */
/* 462 */	NdrFcShort( 0x13 ),	/* 19 */
/* 464 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 468 */	NdrFcShort( 0x8 ),	/* 8 */
/* 470 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 472 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */
<<<<<<< HEAD
/* 476 */	NdrFcShort( 0x4 ),	/* 4 */
=======
/* 476 */	NdrFcShort( 0x1 ),	/* 1 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
/* 478 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter csv */

/* 480 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 482 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 484 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 486 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 488 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MtConnectDboardGui_MIDL_TYPE_FORMAT_STRING MtConnectDboardGui__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x12, 0x0,	/* FC_UP */
/*  4 */	NdrFcShort( 0xe ),	/* Offset= 14 (18) */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/*  8 */	NdrFcShort( 0x2 ),	/* 2 */
/* 10 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 12 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 14 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 16 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 18 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 20 */	NdrFcShort( 0x8 ),	/* 8 */
/* 22 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (6) */
/* 24 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 26 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 28 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 30 */	NdrFcShort( 0x0 ),	/* 0 */
/* 32 */	NdrFcShort( 0x4 ),	/* 4 */
/* 34 */	NdrFcShort( 0x0 ),	/* 0 */
/* 36 */	NdrFcShort( 0xffde ),	/* Offset= -34 (2) */
/* 38 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 40 */	NdrFcShort( 0x2 ),	/* Offset= 2 (42) */
/* 42 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 44 */	NdrFcLong( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0x0 ),	/* 0 */
/* 52 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 54 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 56 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 58 */	0x0,		/* 0 */
			0x46,		/* 70 */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICMTConnectTrayIcon, ver. 0.0,
   GUID={0x4250C680,0x68DF,0x4151,{0xA5,0xD3,0xFB,0x1F,0x88,0x7F,0x12,0x11}} */

#pragma code_seg(".orpc")
static const unsigned short ICMTConnectTrayIcon_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    66,
    102,
    144,
    180,
    216,
    258,
    294,
    336,
    378,
    420,
    456
    };

static const MIDL_STUBLESS_PROXY_INFO ICMTConnectTrayIcon_ProxyInfo =
    {
    &Object_StubDesc,
    MtConnectDboardGui__MIDL_ProcFormatString.Format,
    &ICMTConnectTrayIcon_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ICMTConnectTrayIcon_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    MtConnectDboardGui__MIDL_ProcFormatString.Format,
    &ICMTConnectTrayIcon_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(20) _ICMTConnectTrayIconProxyVtbl = 
{
    &ICMTConnectTrayIcon_ProxyInfo,
    &IID_ICMTConnectTrayIcon,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
<<<<<<< HEAD
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
=======
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::SetHeaderColumns */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::Clear */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::GetPointerBrowser */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::SetTrayIconColor */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::SetTooltip */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::CreateTabWindow */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::AddRow */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::SetIpAddr */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::AddFaultRow */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::Types */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::Formats */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::CreateAlarmTab */ ,
    (void *) (INT_PTR) -1 /* ICMTConnectTrayIcon::SetFaultColumns */
};


static const PRPC_STUB_FUNCTION ICMTConnectTrayIcon_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _ICMTConnectTrayIconStubVtbl =
{
    &IID_ICMTConnectTrayIcon,
    &ICMTConnectTrayIcon_ServerInfo,
    20,
    &ICMTConnectTrayIcon_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    MtConnectDboardGui__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
<<<<<<< HEAD
    0x70001f4, /* MIDL Version 7.0.500 */
=======
    0x700022b, /* MIDL Version 7.0.555 */
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

<<<<<<< HEAD
const CInterfaceProxyVtbl * _MtConnectDboardGui_ProxyVtblList[] = 
=======
const CInterfaceProxyVtbl * const _MtConnectDboardGui_ProxyVtblList[] = 
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
{
    ( CInterfaceProxyVtbl *) &_ICMTConnectTrayIconProxyVtbl,
    0
};

<<<<<<< HEAD
const CInterfaceStubVtbl * _MtConnectDboardGui_StubVtblList[] = 
=======
const CInterfaceStubVtbl * const _MtConnectDboardGui_StubVtblList[] = 
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
{
    ( CInterfaceStubVtbl *) &_ICMTConnectTrayIconStubVtbl,
    0
};

PCInterfaceName const _MtConnectDboardGui_InterfaceNamesList[] = 
{
    "ICMTConnectTrayIcon",
    0
};

<<<<<<< HEAD
const IID *  _MtConnectDboardGui_BaseIIDList[] = 
=======
const IID *  const _MtConnectDboardGui_BaseIIDList[] = 
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
{
    &IID_IDispatch,
    0
};


#define _MtConnectDboardGui_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _MtConnectDboardGui, pIID, n)

int __stdcall _MtConnectDboardGui_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_MtConnectDboardGui_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo MtConnectDboardGui_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _MtConnectDboardGui_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _MtConnectDboardGui_StubVtblList,
    (const PCInterfaceName * ) & _MtConnectDboardGui_InterfaceNamesList,
    (const IID ** ) & _MtConnectDboardGui_BaseIIDList,
    & _MtConnectDboardGui_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

