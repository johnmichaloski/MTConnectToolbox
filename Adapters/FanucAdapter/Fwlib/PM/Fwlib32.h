/*-------------------------------------------------------*/
/* fwlib32.h                                             */
/*                                                       */
/* CNC/PMC Data Window Library for Power Mate            */
/*                                                       */
/* Copyright (C) 1996 by FANUC LTD. All rights reserved. */
/*                                                       */
/*-------------------------------------------------------*/

#ifndef _INC_FWLIB
#define _INC_FWLIB

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_FWLIBDLL_)
#define FWLIBAPI __declspec(dllimport)
#else
#define FWLIBAPI __declspec(dllexport)
#endif

/*
	Axis define
*/
//#define	MAX_AXIS	2	/* Power Mate-D TYPE */
#define	MAX_AXIS	6	/* Power Mate-H TYPE */
#define	ALL_AXES	-1

/*
	Error Codes
*/
#define EW_NODLL	-15	/* DLL not exist error */
#define	EW_INIERR	-14	/* error in APi library inital valiefile */
#define	EW_ITLOW	-13	/* low temperature alarm of intelligent terminal */
#define	EW_ITHIGHT	-12	/* hight temperature alarm of intelligent terminal */
#define	EW_BUS		-11	/* bus error */
#define	EW_SYSTEM2	-10	/* system error(2) */
#define	EW_HSSB		-9	/* communication error of HSSB */
#define	EW_HANDLE	-8	/* windows  library handle error */
#define	EW_VERSION	-7	/* CNC/PMC version missmach */
#define	EW_UNEXP	-6	/* abnomal error */
#define	EW_SYSTEM	-5	/* system error */
#define	EW_PARITY	-4	/* shared RAM parity error */
#define	EW_MMCSYS	-3	/* EMM386 or MMCSYS install error */
#define	EW_RESET	-2	/* RESET or STOP occured error */
#define	EW_BUSY		-1	/* busy error */
#define	EW_OK		0	/* no problem */
#define	EW_FUNC		1	/* command prepare error */
#define	EW_NOPMC	1	/* PMC not exist */
#define	EW_LENGTH	2	/* data block length error */
#define	EW_NUMBER	3	/* data number error */
#define	EW_RANGE	3	/* address range error */
#define	EW_ATTRIB	4	/* data attribute error */
#define	EW_TYPE		4	/* data type error */
#define	EW_DATA		5	/* data error */
#define	EW_NOOPT	6	/* no option error */
#define	EW_PROT		7	/* write protect error */
#define	EW_OVRFLOW	8	/* memeory overflow error */
#define	EW_PARAM	9	/* CNC parameter not correct error */
#define	EW_BUFFER	10	/* buffer error */
#define	EW_PATH		11	/* PATH error */
#define	EW_MODE		12	/* CNC mode error */
#define	EW_REJECT	13	/* execution rejected error */
#define EW_DTSRVR       14	/* data server error */

typedef	unsigned char	byte;
typedef	unsigned short	word;
typedef	unsigned long	dword;

/*--------------------*/
/*                    */
/* Structure Template */
/*                    */
/*--------------------*/
/*-------------------*/
/* CNC: Control axis */
/*-------------------*/

#pragma pack(push, 4)

/* cnc_actf:read actual axis feedrate(F) */
typedef struct odbact {
	short	dummy[2];	/* dummy */
	long	data;		/* actual feed */
} ODBACT;

/* cnc_absolute:read absolute axis position */
/* cnc_machine:read machine axis position */
/* cnc_relative:read relative axis position */
/* cnc_distance:read distance to go */
/* cnc_skip:read skip position */
/* cnc_srvdelay:read servo delay value */
/* cnc_accdecdly:read acceleration/deceleration delay value */
typedef struct odbaxis {
	short	dummy;		/* dummy */
	short	type;		/* axis number */
	long	data[MAX_AXIS];	/* data value */
} ODBAXIS;

/* cnc_rddynamic:read all dynamic data */
typedef struct odbdy {
	short  dummy;	/* dummy */
	short  axis;	/* axis number */
	short  alarm;	/* alarm status */
	short  prgnum;	/* current program number */
	short  prgmnum;	/* main program number */
	long   seqnum;	/* current sequence number */
	long   actf;	/* actual feedrate */
	long   acts;	/* not used */
	union {
	    struct {
			long  absolute[MAX_AXIS];	/* absolute position */
			long  machine[MAX_AXIS];	/* machine position */
			long  relative[MAX_AXIS];	/* relative position */
			long  distance[MAX_AXIS];	/* distance to go */
	    } faxis;
	    struct {
			long  absolute;	/* absolute position */
			long  machine;	/* machine position */
			long  relative;	/* relative position */
			long  distance;	/* distance to go */
	    } oaxis;
	} pos;
} ODBDY;

/* cnc_wrrelpos:set origin / preset relative axis position */
typedef struct idbwrr {
	short	datano;		/* not used */
	short	type;		/* axis number */
	long	data[MAX_AXIS];	/* preset data */
} IDBWRR;


/*----------------------*/
/* CNC: Program related */
/*----------------------*/

/* cnc_upload:upload NC program */
typedef struct odbup {
	short	dummy[2];	/* dummy */
	char	data[256];	/* data */
} ODBUP;	/* In case that the number of data is 256 */

/* cnc_rdprogdir:read program directory */
typedef struct prgdir {
	char	prg_data[256];	/* directory data */
} PRGDIR;	/* In case that the number of data is 256 */

/* cnc_rdproginfo:read program information */
typedef struct odbnc {
	union {
		struct {
			short	reg_prg;	/* registered program number */
			short	unreg_prg;	/* unregistered program number */
			long	used_mem;	/* used memory area */
			long	unused_mem;	/* unused memory area */
		} bin;				/* binary data type */
		char	asc[31];		/* ASCII string type */
	} u;
} ODBNC;

/* cnc_rdprgnum:read program number under execution */
typedef struct odbpro {
	short	dummy[2];	/* dummy */
	short	data;		/* running program number */
	short	mdata;		/* main program number */
} ODBPRO;

/* cnc_rdseqnum:read sequence number under execution */
typedef struct odbseq {
	short	dummy[2];	/* dummy */
	long	data;		/* sequence number */
} ODBSEQ;

/* cnc_rdmdipntr:read execution pointer for MDI operation */
typedef struct odbmdip {
	short	mdiprog;	/* exec. program number */
	long	mdipntr;	/* exec. pointer */
	short	crntprog;	/* prepare program number */
	long	crntpntr;	/* prepare pointer */
} ODBMDIP ;


/*---------------------------*/
/* CNC: NC file data related */
/*---------------------------*/

/* cnc_rdtofs:read tool offset value */
typedef struct odbtofs {
	short	datano;	/* data number */
	short	type;	/* data type */
	long	data;	/* data */
} ODBTOFS;

/* cnc_rdtofsr:read tool offset value(area specified) */
/* cnc_wrtofsr:write tool offset value(area specified) */
typedef struct iodbto {
	short	datano_s;	/* start offset number */
	short	type;		/* offset type */
	short	datano_e;	/* end offset number */
	union{
		long	m_ofs[5];	/* Each */
		long	m_ofs_a[5];	/* All */
	} u;
} IODBTO;	/* In case that the number of data is 5 */

/* cnc_rdparam:read parameter */
/* cnc_wrparam:write parameter */
/* cnc_rdparar:read parameter(area specified) */
/* cnc_wrparas:write parameter(plural specified) */
/* cnc_rdsetr:read setting data(area specified) */
/* cnc_wrsets:write setting data(plural specified) */
typedef struct iodbpsd {
	short	datano;	/* data number */
	short	type;	/* axis number */
	union{
		char	cdata;	/* parameter / setting data */
		short	idata;
		long	ldata;
		char	cdatas[MAX_AXIS];
		short	idatas[MAX_AXIS];
		long	ldatas[MAX_AXIS];
	} u;
} IODBPSD;

/* cnc_rdmacro:read custom macro variable */
typedef struct odbm {
	short	datano;	/* variable number */
	short	dummy;	/* dummy */
	long	mcr_val;/* macro variable */
	short	dec_val;/* decimal point */
} ODBM;

/* cnc_rdmacror:read custom macro variables(area specified) */
/* cnc_wrmacror:write custom macro variables(area specified) */
typedef struct iodbmr {
	short	datano_s;	/* start macro number */
	short	dummy;		/* dummy */
	short	datano_e;	/* end macro number */
	struct {
		long	mcr_val;/* macro variable */
		short	dec_val;/* decimal point */
	} data[5];
} IODBMR;	/* In case that the number of data is 5 */

/* cnc_rdtofsinfo:read tool offset information */
typedef	struct odbtlinf {
	short	ofs_type;	/* memory type */
	short	use_no;		/* available number */
} ODBTLINF;

/* cnc_rdparainfo:read informations of CNC parameter */
typedef struct odbparaif {
	unsigned short	info_no;	/* number of parameter information */
	short	prev_no;		/* previous parameter number */
	short	next_no;		/* next parameter number */
	struct {
		short	prm_no;		/* parameter number */
		short	prm_type;	/* attribute of parameter */
	} info[10];
} ODBPARAIF;	/* In case that the number of data is 10 */

/* cnc_rdmacroinfo:read custom macro variable information */
typedef	struct odbmvinf {
	short	use_no1;	/* number of local variable */
	short  	use_no2;	/* indicator of common variable */
} ODBMVINF;


/*-------------------------------------*/
/* CNC: Operation history data related */
/*-------------------------------------*/

/* cnc_rdophistry:read operation history data */
typedef struct odbhis {
	unsigned short	s_no;	/* start number */
	short	type;		/* not used */
	unsigned short	e_no;	/* end number */
	union {
		struct	{
			short	rec_type;	/* record type */
			short	alm_grp;	/* alarm group */
			short	alm_no;		/* alarm number */
			char	axis_no;	/* axis number */
			char	dummy;		/* dummy */
		} rec_alm;
		struct {
			short	rec_type;	/* record type */
			char	key_code;	/* key code */
			char	pw_flag;	/* power on flag */
			char	dummy[4];	/* dummy */
		} rec_mdi;
		struct {
			short	rec_type;	/* record type */
			char	sig_name;	/* signal name */
			char	sig_old	;	/* old signal bit pattern */
			char	sig_new;	/* new signal bit pattern */
			char	dummy;		/* dummy */
			short	sig_no;		/* signal number */
		} rec_sgn;
		struct {
			short	rec_type;	/* record type */
			char	year;		/* year */
			char	month;		/* month */
			char	day;		/* day */
			char	pw_flg;		/* power on flag */
			char	dummy[2];	/* dummy */
		} rec_date;
		struct {
			short	rec_type;	/* record flag */
			char	hour;		/* hour */
			char	minute;		/* minute */
			char	second;		/* second */
			char	pw_flg;		/* power on flag */
			char	dummy[2];	/* dummy */
		} rec_time;
	} data[10];
} ODBHIS;	/* In case that the number of data is 10 */

/* cnc_rdalmhistry:read alarm history data */
typedef struct odbahis {
	unsigned short	s_no;	/* start number */
	short	type;		/* not used */
	unsigned short	e_no;	/* end number */
	struct {
		short	dummy ;		/* dummy */
		short	alm_grp;	/* alarm group */
		short	alm_no;		/* alarm number */
		char	axis_no;	/* axis number */
		char	year;		/* year */
		char	month;		/* month */
		char	day;		/* day */
		char	hour;		/* hour */
		char	minute;		/* minute */
		char	second;		/* second */
		char	dummy2;		/* dummy */
		short	len_msg;	/* alarm message length */
		char	alm_msg[32];/* alarm message */
	} alm_his[10];
} ODBAHIS;	/* In case that the number of data is 10 */

/* cnc_rdhissgnl:read signals related operation history */
/* cnc_wrhissgnl:write signals related operation history */
typedef struct iodbsig {
	short	datano;	/* not used */
	short	type;	/* not used */
	struct {
		short	ent_no;		/* entry number */
		short	sig_no;		/* signal number */
		char	sig_name;	/* signal name */
		char	mask_pat;	/* signal mask pattern */
	} data[20];
} IODBSIG;	/* In case that the number of data is 20 */



/*-------------*/
/* CNC: Others */
/*-------------*/

/* cnc_sysinfo:read CNC system information */
typedef struct odbsys {
	short	addinfo;	/* not used */
	short	max_axis;	/* maximum axis number */
	char	cnc_type[2];	/* cnc type <ascii char> */
	char	mt_type[2];	/* M/T/TT <ascii char> */
	char	series[4];	/* series NO. <ascii char> */
	char	version[4];	/* version NO.<ascii char> */
	char	axes[2];	/* axis number<ascii char> */
} ODBSYS;

/* cnc_statinfo:read CNC status information */
typedef struct odbst {
	short	dummy[2];	/* dummy */
	short	aut;		/* selected automatic mode */
	short	run;		/* running status */
	short	motion;		/* axis, dwell status */
	short	mstb;		/* m, s, t, b status */
	short	emergency;	/* emergency stop status */
	short	alarm;		/* alarm status */
	short	edit;		/* editting status */
} ODBST;

/* cnc_alarm:read alarm status */
typedef struct odbalm {
	short	dummy[2];	/* dummy */
	short	data;		/* alarm status */
} ODBALM;

/* cnc_rdalminfo:read alarm information */
typedef struct alminfo {
	union {
		struct {
			struct{
				short	axis;	/* axis information */
				short	alm_no;	/* alarm number */
			} alm[5];
			short	data_end;	/* data end */
		} alm1;
		struct {
			struct{
				short	axis;		/* axis information.*/
				short	alm_no;		/* alarm number. */
				short	msg_len;	/* message length. */
				char	alm_msg[32];	/* alarm message. */
			} alm[5];
			short	data_end;		/* data end */
		} alm2;
	} u;
} ALMINFO;	/* In case that the number of alarm is 5 */

/* cnc_modal:read modal data */
typedef struct odbmdl {
	short	datano;	/* kind of modal data. */
	short	type;	/* objective block. */
	union {
		char	g_data;			/* modal data of G code. */
		char	g_rdata[21];
		char	g_1short[4];
		struct {
			long	aux_data;	/* modal data other than G code */
			char	flag1;
			char	flag2;
		} aux;
		struct {
			long	aux_data;	/* modal data other than G code */
			char	flag1;
			char	flag2;
		} raux1[27];
		struct {
			long	aux_data;	/* modal data other than G code */
			char	flag1;
			char	flag2;
		} raux2[MAX_AXIS];
	} modal;
} ODBMDL;

/* cnc_diagnoss:read diagnosis data */
/* cnc_diagnosr:read diagnosis data(area specified) */
typedef struct odbdgn {
	short	datano;	/* data number */
	short	type;	/* axis number */
	union {
		char	cdata;	/* diagnosis data */
		short	idata;
		long	ldata;
		char	cdatas[MAX_AXIS];
		short	idatas[MAX_AXIS];
		long	ldatas[MAX_AXIS];
	} u;
} ODBDGN;

/* cnc_rdopmsg:read operator's message */
typedef struct msg {
     short  datano;	/* operator's message number */
     short  type;	/* operator's message type */
     short  char_num;	/* message string length */
     char  data[256];	/* operator's message string */
} OPMSG;	/* In case that the data length is 256 */

/* cnc_rdopmsg3:read operator's message */
typedef struct opmsg3 {
     short  datano;	/* operator's message number */
     short  type;	/* operator's message type */
     short  char_num;	/* message string length */
     char  data[256];	/* operator's message string */
} OPMSG3;	/* In case that the data length is 256 */

/* cnc_sysconfig:read CNC configuration information */
typedef struct odbsysc {
     char    slot_no_p[16];	/* not used */
     char    slot_no_l[16];	/* not used */
     short   mod_id[16];	/* not used */
     short   soft_id[16];	/* software ID */
     char    s_series[16][5];	/* software series */
     char    s_version[16][5];	/* software version */
     char    dummy[16];		/* dummy */
     short   m_rom;		/* not used */
     short   s_rom;		/* not used */
     char    svo_soft[8];	/* series and version of searvo software */
     char    pmc_soft[6];	/* series and version of PMC management software */
     char    lad_soft[6];	/* series and version of Ladder software */
     char    mcr_soft[8];	/* series and version of a macro executor */
     char    spl1_soft[6];	/* not used */
     char    spl2_soft[6];	/* not used */
     short   frmmin;		/* capacity of FROM module */
     short   drmmin;		/* capacity of DRAM module */
     short   srmmin;		/* capacity of added SRAM module */
     short   pmcmin;		/* type of PMC module */
     short   sv1min;		/* presence or absence of a servo module 1,2 */
     short   sv3min;		/* presence or absence of a servo module 3,4 */
     short   sv5min;		/* presence or absence of a servo module 5,6 */
     short   sicmin;		/* presence or absence of SIC */
     short   posmin;		/* presence or absence of a position LSI */
     short   submin;		/* information of sub-board */
     short   hdiio;		/* presence or absence of an LSI for high-speed skip(I/O card) */
     short   dummy2[32];	/* dummy */
} ODBSYSC;

/* cnc_rdopnlsgnl:read output signal image of software operator's panel */
/* cnc_wropnlsgnl:write output signal of software operator's panel */
typedef struct iodbsgnl {
     short     datano;		/* not used */
     short     type;		/* data select flag */
     short     mode;		/* mode signal */
     short     hndl_ax;		/* Manual handle feed axis selection signal */
     short     hndl_mv;		/* Manual handle feed travel distance selection signal */
     short     rpd_ovrd;	/* rapid traverse override signal */
     short     jog_ovrd;	/* manual feedrate override signal */
     short     feed_ovrd;	/* feedrate override signal */
     short     spdl_ovrd;	/* not used */
     short     blck_del;	/* optional block skip signal */
     short     sngl_blck;	/* single block signal */
     short     machn_lock;	/* machine lock signal */
     short     dry_run;		/* dry run signal */
     short     mem_prtct;	/* memory protection signal */
     short     feed_hold;	/* automatic operation halt signal */
} IODBSGNL;

/* cnc_rdopnlgnrl:read general signal image of software operator's panel */
/* cnc_wropnlgnrl:write general signal image of software operator's panel */
typedef struct iodbgnrl {
     short     datano;	/* not used */
     short     type;	/* data select flag */
     char      sgnal;	/* general signal */
} IODBGNRL;

/* cnc_rdopnlgsname:read general signal name of software operator's panel */
/* cnc_wropnlgsname:write general signal name of software operator's panel*/
typedef struct iodbrdna {
     short    datano;		/* not used */
     short    type;		/* data select flag */
     char     sgnl1_name[9];	/* general signal 1 name */
     char     sgnl2_name[9];	/* general signal 2 name */
     char     sgnl3_name[9];	/* general signal 3 name */
     char     sgnl4_name[9];	/* general signal 4 name */
     char     sgnl5_name[9];	/* general signal 5 name */
     char     sgnl6_name[9];	/* general signal 6 name */
     char     sgnl7_name[9];	/* general signal 7 name */
     char     sgnl8_name[9];	/* general signal 8 name */
} IODBRDNA;


/*------------------------------*/
/* MAXIS: Axis Movement Control */
/*------------------------------*/

/* cnc_opdi:signal operation command */
typedef struct odbopdi {
	short	axis;	/* axis number */
	union {
		char	cdata;
		short	idata;
		long	ldata;
	} u;
} ODBOPDI;

/* cnc_abspoint:absolute movement */
/* cnc_incpoint:incremental movement */
/* cnc_dwell:dwell */
/* cnc_coordre:coordinate establihment */
typedef struct odbpos {
	short	idata;	/* axis number */
	long	ldata;	/* coordinate value */
} ODBPOS;

/* cnc_refpoint:reference point return */
/* cnc_abspoint:absolute movement */
/* cnc_incpoint:incremental movement */
/* cnc_dwell:dwell */
/* cnc_coordre:coordinate establihment */
/* cnc_exebufstat:reading of the executive buffer condition */
typedef struct odbexec {
	short	dummy;		/* dummy */
	char	cdata[2][8];	/* the infomation of the executive condition of */
} ODBEXEC;			/* each PATH                                    */

/* cnc_finstate:Reading of the execution completion condition */
/* cnc_setfin:Release of the reading mode of the execution completion condition */
typedef struct odbfin {
	short	dummy;		/* dummy */
	char	cdata[8];	/* the infomation of the complete notice flag */
} ODBFIN;			/* condition of each PATH                     */


/*-----*/
/* PMC */
/*-----*/

/* pmc_rdpmcrng:read PMC data(area specified) */
/* pmc_wrpmcrng:write PMC data(area specified) */
typedef struct iodbpmc {
	short	type_a;		/* PMC address type */
	short	type_d;		/* PMC data type */
	short	datano_s;	/* start PMC address */
	short	datano_e;	/* end PMC address */
	union {
		char	cdata[5];	/* PMC data */
		short	idata[5];
		long	ldata[5];
	} u;
} IODBPMC;	/* In case that the number of data is 5 */

/* pmc_rdpmcinfo:read informations of PMC data */
typedef struct odbpmcinf {
	short	datano;	/* number of PMC data information */
	struct {
		char	pmc_adr;		/* kind of PMC address */
		char	adr_attr;		/* attribute of PMC address */
		unsigned short	top_num;	/* top number of PMC address */
		unsigned short	last_num;	/* end number of PMC address */
   } info[64];
} ODBPMCINF;


/*--------------------------*/
/* HSSB multiple connection */
/*--------------------------*/

/* cnc_rdnodeinfo:read node informations */
typedef struct odbnode {
	long node_no;
	long io_base;
	long status;
	long cnc_type;
	char node_name[20];
} ODBNODE;

#pragma pack(pop)

/*-------------------*/
/* CNC: Control axis */
/*-------------------*/

/* read actual axis feedrate(F) */
FWLIBAPI short WINAPI cnc_actf(unsigned short,ODBACT *);

/* read absolute axis position */
FWLIBAPI short WINAPI cnc_absolute(unsigned short,short,short,ODBAXIS *);

/* read machine axis position */
FWLIBAPI short WINAPI cnc_machine(unsigned short,short,short,ODBAXIS *);

/* read relative axis position */
FWLIBAPI short WINAPI cnc_relative(unsigned short,short,short,ODBAXIS *);

/* read distance to go */
FWLIBAPI short WINAPI cnc_distance(unsigned short,short,short,ODBAXIS *);

/* read skip position */
FWLIBAPI short WINAPI cnc_skip(unsigned short,short,short,ODBAXIS *);

/* read servo delay value */
FWLIBAPI short WINAPI cnc_srvdelay(unsigned short,short,short,ODBAXIS *);

/* read acceleration/deceleration delay value */
FWLIBAPI short WINAPI cnc_accdecdly(unsigned short,short,short,ODBAXIS *);

/* read all dynamic data */
FWLIBAPI short WINAPI cnc_rddynamic(unsigned short,short,short,ODBDY *);

/* set origin / preset relative axis position */
FWLIBAPI short WINAPI cnc_wrrelpos(unsigned short,short,IDBWRR *);

/*----------------------*/
/* CNC: Program related */
/*----------------------*/

/* start downloading NC program */
FWLIBAPI short WINAPI cnc_dwnstart(unsigned short);

/* download NC program */
FWLIBAPI short WINAPI cnc_download(unsigned short,char *,short);

/* end of downloading NC program */
FWLIBAPI short WINAPI cnc_dwnend(unsigned short);

/* start uploading NC program */
FWLIBAPI short WINAPI cnc_upstart(unsigned short,short);

/* upload NC program */
FWLIBAPI short WINAPI cnc_upload(unsigned short,ODBUP *,unsigned short *);

/* end of uploading NC program */
FWLIBAPI short WINAPI cnc_upend(unsigned short);

/* search specified program */
FWLIBAPI short WINAPI cnc_search(unsigned short,short);

/* delete all programs */
FWLIBAPI short WINAPI cnc_delall(unsigned short);

/* delete specified program */
FWLIBAPI short WINAPI cnc_delete(unsigned short,short);

/* read program directory */
FWLIBAPI short WINAPI cnc_rdprogdir(unsigned short,short,short,short,unsigned short,PRGDIR *);

/* read program information */
FWLIBAPI short WINAPI cnc_rdproginfo(unsigned short,short,short,ODBNC *);

/* read program number under execution */
FWLIBAPI short WINAPI cnc_rdprgnum(unsigned short,ODBPRO *);

/* read sequence number under execution */
FWLIBAPI short WINAPI cnc_rdseqnum(unsigned short,ODBSEQ *);

/* search specified sequence number */
FWLIBAPI short WINAPI cnc_seqsrch(unsigned short,long);

/* rewind cursor of NC program */
FWLIBAPI short WINAPI cnc_rewind(unsigned short);

/* read program under execution */
FWLIBAPI short WINAPI cnc_rdexecprog(unsigned short,unsigned short *,short *,char *);

/* write program for MDI operation */
FWLIBAPI short WINAPI cnc_wrmdiprog(unsigned short,short,char *);

/* read execution pointer for MDI operation */
FWLIBAPI short WINAPI cnc_rdmdipntr(unsigned short,ODBMDIP *);

/* write execution pointer for MDI operation */
FWLIBAPI short WINAPI cnc_wrmdipntr(unsigned short,long);


/*---------------------------*/
/* CNC: NC file data related */
/*---------------------------*/

/* read tool offset value */
FWLIBAPI short WINAPI cnc_rdtofs(unsigned short,short,short,short,ODBTOFS *);

/* write tool offset value */
FWLIBAPI short WINAPI cnc_wrtofs(unsigned short,short,short,short,long);

/* read tool offset value(area specified) */
FWLIBAPI short WINAPI cnc_rdtofsr(unsigned short,short,short,short,short,IODBTO *);

/* write tool offset value(area specified) */
FWLIBAPI short WINAPI cnc_wrtofsr(unsigned short,short,IODBTO *);

/* read parameter */
FWLIBAPI short WINAPI cnc_rdparam(unsigned short,short,short,short,IODBPSD *);

/* write parameter */
FWLIBAPI short WINAPI cnc_wrparam(unsigned short,short,IODBPSD *);

/* read parameter(area specified) */
FWLIBAPI short WINAPI cnc_rdparar(unsigned short,short *,short,short *,short *,void *);

/* write parameter(area specified) */
FWLIBAPI short WINAPI cnc_wrparas(unsigned short,short,void *);

/* read setting data(area specified) */
FWLIBAPI short WINAPI cnc_rdsetr(unsigned short,short *,short,short *,short *,void *);

/* write setting data(area specified) */
FWLIBAPI short WINAPI cnc_wrsets(unsigned short,short,void *);

/* read custom macro variable */
FWLIBAPI short WINAPI cnc_rdmacro(unsigned short,short,short,ODBM *);

/* write custom macro variable */
FWLIBAPI short WINAPI cnc_wrmacro(unsigned short,short,short,long,short);

/* read custom macro variables(area specified) */
FWLIBAPI short WINAPI cnc_rdmacror(unsigned short,short,short,short,IODBMR *);

/* write custom macro variables(area specified) */
FWLIBAPI short WINAPI cnc_wrmacror(unsigned short,short,IODBMR *);

/* read tool offset information */
FWLIBAPI short WINAPI cnc_rdtofsinfo(unsigned short,ODBTLINF *);

/* read informations of CNC parameter */
FWLIBAPI short WINAPI cnc_rdparainfo(unsigned short,short,unsigned short,ODBPARAIF *);

/* read custom macro variable information */
FWLIBAPI short WINAPI cnc_rdmacroinfo(unsigned short,ODBMVINF *);


/*-------------------------------------*/
/* CNC: Operation history data related */
/*-------------------------------------*/

/* stop logging operation history data */
FWLIBAPI short WINAPI cnc_stopophis(unsigned short);

/* restart logging operation history data */
FWLIBAPI short WINAPI cnc_startophis(unsigned short);

/* read number of operation history data */
FWLIBAPI short WINAPI cnc_rdophisno(unsigned short,unsigned short *);

/* read operation history data */
FWLIBAPI short WINAPI cnc_rdophistry(unsigned short,unsigned short,unsigned short,unsigned short,ODBHIS *);

/* read number of alarm history data */
FWLIBAPI short WINAPI cnc_rdalmhisno(unsigned short,unsigned short *);

/* read alarm history data */
FWLIBAPI short WINAPI cnc_rdalmhistry(unsigned short,unsigned short,unsigned short,unsigned short,ODBAHIS *);

/* clear operation history data */
FWLIBAPI short WINAPI cnc_clearophis(unsigned short,short);

/* read signals related operation history */
FWLIBAPI short WINAPI cnc_rdhissgnl(unsigned short,IODBSIG *);

/* write signals related operation history */
FWLIBAPI short WINAPI cnc_wrhissgnl(unsigned short,IODBSIG *);


/*-------------*/
/* CNC: Others */
/*-------------*/

/* read CNC system information */
FWLIBAPI short WINAPI cnc_sysinfo(unsigned short,ODBSYS *);

/* read CNC status information */
FWLIBAPI short WINAPI cnc_statinfo(unsigned short,ODBST *);

/* read alarm status */
FWLIBAPI short WINAPI cnc_alarm(unsigned short,ODBALM *);

/* read alarm information */
FWLIBAPI short WINAPI cnc_rdalminfo(unsigned short,short,short,short,ALMINFO *);

/* read modal data */
FWLIBAPI short WINAPI cnc_modal(unsigned short,short,short,ODBMDL *);

/* read diagnosis data */
FWLIBAPI short WINAPI cnc_diagnoss(unsigned short,short,short,short,ODBDGN *);

/* read diagnosis data(area specified) */
FWLIBAPI short WINAPI cnc_diagnosr(unsigned short,short *,short,short *,short *,void *);

/* read operator's message */
FWLIBAPI short WINAPI cnc_rdopmsg(unsigned short,short,short,OPMSG *);

/* read operator's message3 */
FWLIBAPI short WINAPI cnc_rdopmsg3(unsigned short,short,short *,OPMSG3 *);

/* allocate library handle */
FWLIBAPI short WINAPI cnc_allclibhndl(unsigned short *);

/* free library handle */
FWLIBAPI short WINAPI cnc_freelibhndl(unsigned short);

/* get custom macro type */
FWLIBAPI short WINAPI cnc_getmactype(unsigned short,short *);

/* set custom macro type */
FWLIBAPI short WINAPI cnc_setmactype(unsigned short,short);

/* read CNC configuration information */
FWLIBAPI short WINAPI cnc_sysconfig(unsigned short,ODBSYSC *);

/* read output signal image of software operator's panel */
FWLIBAPI short WINAPI cnc_rdopnlsgnl(unsigned short,short,IODBSGNL *);

/* write output signal of software operator's panel */
FWLIBAPI short WINAPI cnc_wropnlsgnl(unsigned short,IODBSGNL *);

/* read general signal image of software operator's panel */
FWLIBAPI short WINAPI cnc_rdopnlgnrl(unsigned short,short,IODBGNRL *);

/* write general signal image of software operator's panel */
FWLIBAPI short WINAPI cnc_wropnlgnrl(unsigned short,IODBGNRL *);

/* read general signal name of software operator's panel */
FWLIBAPI short WINAPI cnc_rdopnlgsname(unsigned short,short,IODBRDNA *);

/* write general signal name of software operator's panel */
FWLIBAPI short WINAPI cnc_wropnlgsname(unsigned short,IODBRDNA *);

/* get maximum valid figures and number of decimal places */
FWLIBAPI short WINAPI cnc_getfigure(unsigned short,short,short *,short *,short *);

/* read the loop gain for each axis */
FWLIBAPI short WINAPI cnc_rdloopgain(unsigned short,long *);

/* read the actual current for each axis */
FWLIBAPI short WINAPI cnc_rdcurrent(unsigned short,long *);

/* read the actual speed for each axis */
FWLIBAPI short WINAPI cnc_rdsrvspeed(unsigned short,long *);


/*------------------------------*/
/* MAXIS: Axis Movement Control */
/*------------------------------*/

/* cnc_opdi:signal operation command */
FWLIBAPI short WINAPI cnc_opdi(unsigned short,short,ODBOPDI *);

/* cnc_refpoint:reference point return */
FWLIBAPI short WINAPI cnc_refpoint(unsigned short,short,short,short,ODBEXEC *);

/* cnc_abspoint:absolute movement */
FWLIBAPI short WINAPI cnc_abspoint(unsigned short,short,short,short,long,ODBPOS *,ODBEXEC *);

/* cnc_incpoint:incremental movement */
FWLIBAPI short WINAPI cnc_incpoint(unsigned short,short,short,short,long,ODBPOS *,ODBEXEC *);

/* cnc_dwell:dwell */
FWLIBAPI short WINAPI cnc_dwell(unsigned short,short,short,short,ODBPOS *,ODBEXEC *);

/* cnc_coordre:coordinate establihment */
FWLIBAPI short WINAPI cnc_coordre(unsigned short,short,short,short,ODBPOS *,ODBEXEC *);

/* cnc_exebufstat:reading of the executive buffer condition */
FWLIBAPI short WINAPI cnc_exebufstat(unsigned short,ODBEXEC *);

/* cnc_finstate:Reading of the execution completion condition */
FWLIBAPI short WINAPI cnc_finstate(unsigned short,ODBFIN *);

/* cnc_setfin:Release of the reading mode of the execution completion condition */
FWLIBAPI short WINAPI cnc_setfin(unsigned short,ODBFIN *);


/*-----*/
/* PMC */
/*-----*/

/* read PMC data(area specified) */
FWLIBAPI short WINAPI pmc_rdpmcrng(unsigned short,short,short,short,short,short,IODBPMC *);

/* write PMC data(area specified) */
FWLIBAPI short WINAPI pmc_wrpmcrng(unsigned short,short,IODBPMC *);

/* read informations of PMC data */
FWLIBAPI short WINAPI pmc_rdpmcinfo(unsigned short,short,ODBPMCINF *);


/*--------------------------*/
/* HSSB multiple connection */
/*--------------------------*/

/* read number of node */
FWLIBAPI short WINAPI cnc_rdnodenum(long *);

/* read node informations */
FWLIBAPI short WINAPI cnc_rdnodeinfo(long,ODBNODE *);

/* set default node number */
FWLIBAPI short WINAPI cnc_setdefnode(long);

/* allocate library handle 2 */
FWLIBAPI short WINAPI cnc_allclibhndl2(long,unsigned short *);

#ifdef __cplusplus
}
#endif

#endif  /* _INC_FWLIB */
