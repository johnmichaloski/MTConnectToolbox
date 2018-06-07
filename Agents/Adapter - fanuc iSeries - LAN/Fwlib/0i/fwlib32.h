/*-------------------------------------------------------*/
/* fwlib32.h                                             */
/*                                                       */
/* CNC/PMC Data Window Library for series 0i             */
/*                                                       */
/* Copyright (C) 2000 by FANUC LTD. All rights reserved. */
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
#define MAX_AXIS        4
#define ALL_AXES        -1

/*
    Error Codes
*/
#define EW_NODLL        -15             /* DLL not exist error */
#define EW_BUS          -11             /* bus error */
#define EW_SYSTEM2      -10             /* system error */
#define EW_HSSB         -9              /* hssb communication error */
#define EW_HANDLE       -8              /* Windows library handle error */
#define EW_VERSION      -7              /* CNC/PMC version missmatch */
#define EW_UNEXP        -6              /* abnormal error */
#define EW_SYSTEM       -5              /* system error */
#define EW_PARITY       -4              /* shared RAM parity error */
#define EW_MMCSYS       -3              /* emm386 or mmcsys install error */
#define EW_RESET        -2              /* reset or stop occured error */
#define EW_BUSY         -1              /* busy error */
#define EW_OK           0               /* no problem */
#define EW_FUNC         1               /* command prepare error */
#define EW_NOPMC        1               /* pmc not exist */
#define EW_LENGTH       2               /* data block length error */
#define EW_NUMBER       3               /* data number error */
#define EW_RANGE        3               /* address range error */
#define EW_ATTRIB       4               /* data attribute error */
#define EW_TYPE         4               /* data type error */
#define EW_DATA         5               /* data error */
#define EW_NOOPT        6               /* no option error */
#define EW_PROT         7               /* write protect error */
#define EW_OVRFLOW      8               /* memory overflow error */
#define EW_PARAM        9               /* cnc parameter not correct error */
#define EW_BUFFER       10              /* buffer error */
#define EW_PATH         11              /* path error */
#define EW_MODE         12              /* cnc mode error */
#define EW_REJECT       13              /* execution rejected error */
#define EW_DTSRVR       14              /* data server error */

/*
   Result codes of DNC operation
*/
#define DNC_NORMAL      (-1)            /* normal completed */
#define DNC_CANCEL      (-32768)        /* DNC operation was canceled by CNC */
#define DNC_OPENERR     (-514)          /* file open error */
#define DNC_NOFILE      (-516)          /* file not found */
#define DNC_READERR     (-517)          /* read error */


/*--------------------*/
/*                    */
/* Structure Template */
/*                    */
/*--------------------*/
/*-------------------------------------*/
/* CNC: Control axis / spindle related */
/*-------------------------------------*/

#pragma pack(push, 4)

/* cnc_actf:read actual axis feedrate(F) */
/* cnc_acts:read actual spindle speed(S) */
typedef struct odbact {
    short   dummy[2] ;  /* dummy */
    long    data ;      /* actual feed / actual spindle */
} ODBACT ;

/* cnc_acts2:read actual spindle speed(S) */
/* (All or specified ) */
typedef struct odbact2 {
	 short	datano;		/* spindle number */
	 short	type;		/* dummy */
	 long	data[4];	/* spindle data */
} ODBACT2 ;

/* cnc_absolute:read absolute axis position */
/* cnc_machine:read machine axis position */
/* cnc_relative:read relative axis position */
/* cnc_distance:read distance to go */
/* cnc_skip:read skip position */
/* cnc_srvdelay:read servo delay value */
/* cnc_accdecdly:read acceleration/deceleration delay value */
/* cnc_absolute2:read absolute axis position 2 */
/* cnc_relative2:read relative axis position 2 */
typedef struct odbaxis {
    short   dummy ;             /* dummy */
    short   type ;              /* axis number */
    long    data[MAX_AXIS] ;    /* data value */
} ODBAXIS ;

/* cnc_rddynamic:read all dynamic data */
typedef struct odbdy {
    short   dummy ;
    short   axis ;      /* axis number */
    short   alarm ;     /* alarm status */
    short   prgnum ;    /* current program number */
    short   prgmnum ;   /* main program number */
    long    seqnum ;    /* current sequence number */
    long    actf ;      /* actual feedrate */
    long    acts ;      /* actual spindle speed */
    union {
        struct {
            long    absolute[MAX_AXIS] ;    /* absolute position */
            long    machine[MAX_AXIS] ;     /* machine position */
            long    relative[MAX_AXIS] ;    /* relative position */
            long    distance[MAX_AXIS] ;    /* distance to go */
        } faxis ;
        struct {
            long    absolute ;  /* absolute position */
            long    machine ;   /* machine position */
            long    relative ;  /* relative position */
            long    distance ;  /* distance to go */
        } oaxis ;   /* In case of no axis */
    } pos ;
} ODBDY ;

/* cnc_wrrelpos:set origin / preset relative axis position */
typedef struct idbwrr {
    short   datano;         /* dummy */
    short   type;           /* axis number */
    long    data[MAX_AXIS]; /* preset data */
} IDBWRR;

/* cnc_prstwkcd:preset work coordinate */
typedef struct idbwra {
    short   datano;         /* dummy */
    short   type;           /* axis number */
    long    data[MAX_AXIS]; /* preset data */
} IDBWRA;

/* cnc_rdmovrlap:read manual overlapped motion value */
typedef struct iodbovl {
    short   datano;             /* dummy */
    short   type;               /* axis number */
    long    data[2][MAX_AXIS];  /* data value */
} IODBOVL;

/* cnc_rdspload:read load information of serial spindle */
/* cnc_rdspmaxrpm:read maximum r.p.m. ratio of serial spindle */
/* cnc_rdspgear:read gear ratio of serial spindle */
typedef struct odbspn {
    short   datano;     /* spindle number */
    short   type;       /* dummy */
    short   data[4];    /* spindle data */
} ODBSPN;


/*----------------------*/
/* CNC: Program related */
/*----------------------*/

/* cnc_rddncdgndt:read the diagnosis data of DNC operation */
typedef struct odbdncdgn {
        short          ctrl_word;
        short          can_word;
        char           nc_file[16];
        unsigned short read_ptr;
        unsigned short write_ptr;
        unsigned short empty_cnt;
        unsigned long  total_size;
} ODBDNCDGN;

/* cnc_upload:upload NC program */
/* cnc_cupload:upload NC program(conditional) */
typedef struct odbup {
    short   dummy[2] ;  /* dummy */
    char    data[256] ; /* data */
} ODBUP ;   /* In case that the number of data is 256 */

/* cnc_buff:read buffer status for downloading/verification NC program */
typedef struct odbbuf {
    short   dummy[2] ;  /* dummy */
    short   data ;      /* buffer status */
} ODBBUF ;

/* cnc_rdprogdir:read program directory */
typedef struct prgdir {
    char    prg_data[256] ; /* directory data */
} PRGDIR ;  /* In case that the number of data is 256 */

/* cnc_rdproginfo:read program information */
typedef struct odbnc {
    union {
        struct {
            short   reg_prg ;       /* registered program number */
            short   unreg_prg ;     /* unregistered program number */
            long    used_mem ;      /* used memory area */
            long    unused_mem ;    /* unused memory area */
        } bin ;             /* binary data type */
        char    asc[31] ;   /* ASCII string type */
    } u ;
} ODBNC ;

/* cnc_rdprgnum:read program number under execution */
typedef struct odbpro {
    short   dummy[2] ;  /* dummy */
    short   data ;      /* running program number */
    short   mdata ;     /* main program number */
} ODBPRO ;

/* cnc_rdseqnum:read sequence number under execution */
typedef struct odbseq {
    short   dummy[2] ;  /* dummy */
    long    data ;      /* sequence number */
} ODBSEQ ;

/* cnc_rdmdipntr:read execution pointer for MDI operation */
typedef struct odbmdip {
    short   mdiprog;    /* exec. program number */
    long    mdipntr;    /* exec. pointer */
    short   crntprog;   /* prepare program number */
    long    crntpntr;   /* prepare pointer */
} ODBMDIP;

/* cnc_rdexecpt:read execution program pointer */
typedef struct prgpnt {
    long    prog_no;    /* program number */
    long    blk_no;     /* block number */
} PRGPNT;


/*---------------------------*/
/* CNC: NC file data related */
/*---------------------------*/

/* cnc_rdtofs:read tool offset value */
typedef struct odbtofs {
    short   datano ;    /* data number */
    short   type ;      /* data type */
    long    data ;      /* data */
} ODBTOFS ;

/* cnc_rdtofsr:read tool offset value(area specified) */
/* cnc_wrtofsr:write tool offset value(area specified) */
typedef struct iodbto {
    short   datano_s ;  /* start offset number */
    short   type ;      /* offset type */
    short   datano_e ;  /* end offset number */
    union {
        long    m_ofs[5] ;      /* M Each */
        long    m_ofs_a[5] ;    /* M-A All */
        long    m_ofs_b[10] ;   /* M-B All */
        long    m_ofs_c[20] ;   /* M-C All */
        short   t_tip[5] ;      /* T Each, 2-byte */
        long    t_ofs[5] ;      /* T Each, 4-byte */
        struct  {
            short   tip ;
            long    data[4] ;
        } t_ofs_a[5] ;          /* T-A All */
        struct {
            short   tip ;
            long    data[8] ;
        } t_ofs_b[5] ;          /* T-B All */
    } u ;   /* In case that the number of data is 5 */
} IODBTO ;

/* cnc_rdzofs:read work zero offset value */
/* cnc_wrzofs:write work zero offset value */
typedef struct iodbzofs {
    short   datano ;            /* offset NO. */
    short   type ;              /* axis number */
    long    data[MAX_AXIS] ;    /* data value */
} IODBZOFS ;

/* cnc_rdzofsr:read work zero offset value(area specified) */
/* cnc_wrzofsr:write work zero offset value(area specified) */
typedef struct iodbzor {
    short   datano_s ;  /* start offset number */
    short   type ;      /* axis number */
    short   datano_e ;  /* end offset number */
    long    data[56] ;  /* offset value */
} IODBZOR ; /* In case that the number of axes is MAX_AXIS(=8), the number of data is 7 */

/* cnc_rdparam:read parameter */
/* cnc_wrparam:write parameter */
/* cnc_rdset:read setting data */
/* cnc_wrset:write setting data */
/* cnc_rdparar:read parameter(area specified) */
/* cnc_wrparas:write parameter(plural specified) */
/* cnc_rdsetr:read setting data(area specified) */
/* cnc_wrsets:write setting data(plural specified) */
typedef struct iodbpsd {
    short   datano ;    /* data number */
    short   type ;      /* axis number */
    union {
        char    cdata ;     /* parameter / setting data */
        short   idata ;
        long    ldata ;
        char    cdatas[MAX_AXIS] ;
        short   idatas[MAX_AXIS] ;
        long    ldatas[MAX_AXIS] ;
    } u ;
} IODBPSD ;

/* cnc_rdpitchr:read pitch error compensation data(area specified) */
/* cnc_wrpitchr:write pitch error compensation data(area specified) */
typedef struct iodbpi {
    short   datano_s ;  /* start pitch number */
    short   dummy ;     /* dummy */
    short   datano_e ;  /* end pitch number */
    char    data[5] ;   /* offset value */
} IODBPI ;  /* In case that the number of data is 5 */

/* cnc_rdmacro:read custom macro variable */
typedef struct odbm {
    short   datano ;    /* variable number */
    short   dummy ;     /* dummy */
    long    mcr_val ;   /* macro variable */
    short   dec_val ;   /* decimal point */
} ODBM ;

/* cnc_rdmacror:read custom macro variables(area specified) */
/* cnc_wrmacror:write custom macro variables(area specified) */
typedef struct iodbmr {
    short   datano_s ;  /* start macro number */
    short   dummy ;     /* dummy */
    short   datano_e ;  /* end macro number */
    struct {
        long    mcr_val ;   /* macro variable */
        short   dec_val ;   /* decimal point */
    } data[5] ;
} IODBMR ;  /* In case that the number of data is 5 */

/* cnc_rdpmacro:read P code macro variable */
typedef struct odbpm {
    long    datano ;    /* variable number */
    short   dummy ;     /* dummy */
    long    mcr_val ;   /* macro variable */
    short   dec_val ;   /* decimal point */
} ODBPM ;

/* cnc_rdpmacror:read P code macro variables(area specified) */
/* cnc_wrpmacror:write P code macro variables(area specified) */
typedef struct iodbpr {
    long    datano_s ;  /* start macro number */
    short   dummy ;     /* dummy */
    long    datano_e ;  /* end macro number */
    struct {
        long    mcr_val ;   /* macro variable */
        short   dec_val ;   /* decimal point */
    } data[5] ;
} IODBPR ;  /* In case that the number of data is 5 */

/* cnc_rdtofsinfo:read tool offset information */
typedef struct odbtlinf {
    short   ofs_type;
    short   use_no;
} ODBTLINF;

/* cnc_rdmacroinfo:read custom macro variable information */
typedef struct odbmvinf {
    short   use_no1;
    short   use_no2;
} ODBMVINF;

/* cnc_rdpmacroinfo:read P code macro variable information */
typedef struct odbpminf {
    short               use_no1;
#ifdef PCD_UWORD
    unsigned short      use_no2;
#else
    short               use_no2;
#endif
    short               v2_type;
} ODBPMINF;


/*----------------------------------------*/
/* CNC: Tool life management data related */
/*----------------------------------------*/

/* cnc_rdgrpid:read tool life management data(tool group number) */
typedef struct odbtlife1 {
    short   dummy ; /* dummy */
    short   type ;  /* data type */
    long    data ;  /* data */
} ODBTLIFE1 ;

/* cnc_rdngrp:read tool life management data(number of tool groups) */
typedef struct odbtlife2 {
    short   dummy[2] ;  /* dummy */
    long    data ;      /* data */
} ODBTLIFE2 ;

/* cnc_rdntool:read tool life management data(number of tools) */
/* cnc_rdlife:read tool life management data(tool life) */
/* cnc_rdcount:read tool life management data(tool lift counter) */
typedef struct odbtlife3 {
    short   datano ;    /* data number */
    short   dummy ;     /* dummy */
    long    data ;      /* data */
} ODBTLIFE3 ;

/* cnc_rd1length:read tool life management data(tool length number-1) */
/* cnc_rd2length:read tool life management data(tool length number-2) */
/* cnc_rd1radius:read tool life management data(cutter compensation no.-1) */
/* cnc_rd2radius:read tool life management data(cutter compensation no.-2) */
/* cnc_t1info:read tool life management data(tool information-1) */
/* cnc_t2info:read tool life management data(tool information-2) */
/* cnc_toolnum:read tool life management data(tool number) */
typedef struct odbtlife4 {
    short   datano ;    /* data number */
    short   type ;      /* data type */
    long    data ;      /* data */
} ODBTLIFE4 ;

/* cnc_rdgrpid2:read tool life management data(tool group number) 2 */
typedef struct odbtlife5 {
    long    dummy ; /* dummy */
    long    type ;  /* data type */
    long    data ;  /* data */
} ODBTLIFE5 ;

/* cnc_rdtoolrng:read tool life management data(tool number, tool life, tool life counter)(area specified) */
typedef struct iodbtr {
    short   datano_s ;  /* start group number */
    short   dummy ;     /* dummy */
    short   datano_e ;  /* end group number */
    struct {
        long    ntool ;     /* tool number */
        long    life ;      /* tool life */
        long    count ;     /* tool life counter */
    } data[5] ;
} IODBTR ;  /* In case that the number of data is 5 */

/* cnc_rdtoolgrp:read tool life management data(all data within group) */
typedef struct odbtg {
    short   grp_num ;   /* start group number */
    short   dummy[2] ;  /* dummy */
    long    ntool ;     /* tool number */
    long    life ;      /* tool life */
    long    count ;     /* tool life counter */
    struct {
        long    tuse_num ;      /* tool number */
        long    tool_num ;      /* tool life */
        long    length_num ;    /* tool life counter */
        long    radius_num ;    /* tool life counter */
        long    tinfo ;         /* tool life counter */
    } data[5] ;
} ODBTG ;   /* In case that the number of data is 5 */

/* cnc_wrcountr:write tool life management data(tool life counter) (area specified) */
typedef struct idbwrc {
    short   datano_s ;  /* start group number */
    short   dummy ;     /* dummy */
    short   datano_e ;  /* end group number */
    struct {
        long    dummy[2] ;  /* dummy */
        long    count ;     /* tool life counter */
    } data[5] ;
} IDBWRC ;  /* In case that the number of data is 5 */

/* cnc_rdusegrpid:read tool life management data(used tool group number) */
typedef struct odbusegr {
    short   datano; /* dummy */
    short   type;   /* dummy */
    long    next;   /* next use group number */
    long    use;    /* using group number */
    long    slct;   /* selecting group number */
} ODBUSEGR;

/* cnc_rdmaxgrp:read tool life management data(max. number of tool groups) */
/* cnc_rdmaxtool:read tool life management data(maximum number of tool within group) */
typedef struct odblfno {
    short   datano; /* dummy */
    short   type;   /* dummy */
    short   data;   /* number of data */
} ODBLFNO;

/* cnc_rdusetlno:read tool life management data(used tool no within group) */
typedef struct odbtluse {
    short   s_grp;      /* start group number */
    short   dummy;      /* dummy */
    short   e_grp;      /* end group number */
    long    data[5];    /* tool using number */
} ODBTLUSE; /* In case that the number of group is 5 */

/* cnc_rd1tlifedata:read tool life management data(tool data1) */
/* cnc_rd2tlifedata:read tool life management data(tool data2) */
/* cnc_wr1tlifedata:write tool life management data(tool data1) */
/* cnc_wr2tlifedata:write tool life management data(tool data2) */
typedef struct iodbtd {
    short   datano;     /* tool group number */
    short   type;       /* tool using number */
    long    tool_num;   /* tool number */
    long    h_code;     /* H code */
    long    d_code;     /* D code */
    long    tool_inf;   /* tool information */
} IODBTD;

/* cnc_rd1tlifedat2:read tool life management data(tool data1) 2 */
/* cnc_wr1tlifedat2:write tool life management data(tool data1) 2 */
typedef struct iodbtd2 {
    short   datano;     /* tool group number */
    short   dummy;      /* dummy */
    long    type;       /* tool using number */
    long    tool_num;   /* tool number */
    long    h_code;     /* H code */
    long    d_code;     /* D code */
    long    tool_inf;   /* tool information */
} IODBTD2;

/* cnc_rdgrpinfo:read tool life management data(tool group information) */
/* cnc_wrgrpinfo:write tool life management data(tool group information) */
typedef struct iodbtgi {
    short   s_grp;  /* start group number */
    short   dummy;  /* dummy */
    short   e_grp;  /* end group number */
    struct {
        long    n_tool;         /* number of tool */
        long    count_value;    /* tool life */
        long    counter;        /* tool life counter */
        long    count_type;     /* tool life counter type */
    } data[5];
} IODBTGI;  /* In case that the number of group is 5 */

/* cnc_instlifedt:insert tool life management data(tool data) */
typedef struct idbitd {
    short   datano; /* tool group number */
    short   type;   /* tool using number */
    long    data;   /* tool number */
} IDBITD;


/*-------------------------------------*/
/* CNC: Operation history data related */
/*-------------------------------------*/

/* cnc_rdophistry:read operation history data */
typedef struct odbhis {
    unsigned short  s_no;   /* start number */
    short   type;           /* dummy */
    unsigned short  e_no;   /* end number */
    union {
        struct {
            short   rec_type;   /* record type */
            short   alm_grp;    /* alarm group */
            short   alm_no;     /* alarm number */
            char    axis_no;    /* axis number */
            char    dummy;
        } rec_alm;
        struct {
            short   rec_type;   /* record type */
            char    key_code;   /* key code */
            char    pw_flag;    /* power on flag */
            char    dummy[4];
        } rec_mdi;
        struct {
            short   rec_type;   /* record type */
            char    sig_name;   /* signal name */
            char    sig_old;    /* old signal bit pattern */
            char    sig_new;    /* new signal bit pattern */
            char    dummy;
            short   sig_no;     /* signal number */
        } rec_sgn;
        struct {
            short   rec_type;   /* record type */
            char    year;       /* year */
            char    month;      /* month */
            char    day;        /* day */
            char    pw_flag;    /* power on flag */
            char    dummy[2];
        } rec_date;
        struct {
            short   rec_type;   /* record flag */
            char    hour;       /* hour */
            char    minute;     /* minute */
            char    second;     /* second */
            char    pw_flag;    /* power on flag */
            char    dummy[2];
        } rec_time;
    } data[10];
} ODBHIS;   /* In case that the number of data is 10 */

/* cnc_rdalmhistry:read alarm history data */
typedef struct odbahis {
    unsigned short  s_no;   /* start number */
    short   type;           /* dummy */
    unsigned short  e_no;   /* end number */
    struct {
        short   dummy;
        short   alm_grp;        /* alarm group */
        short   alm_no;         /* alarm number */
        char    axis_no;        /* axis number */
        char    year;           /* year */
        char    month;          /* month */
        char    day;            /* day */
        char    hour;           /* hour */
        char    minute;         /* minute */
        char    second;         /* second */
        char    dummy2;
        short   len_msg;        /* alarm message length */
        char    alm_msg[32];    /* alarm message */
    } alm_his[10];
} ODBAHIS;  /* In case that the number of data is 10 */

/* cnc_rdhissgnl:read signals related operation history */
/* cnc_wrhissgnl:write signals related operation history */
typedef struct iodbsig {
    short   datano; /* dummy */
    short   type;   /* dummy */
    struct {
        short   ent_no;     /* entry number */
        short   sig_no;     /* signal number */
        char    sig_name;   /* signal name */
        char    mask_pat;   /* signal mask pattern */
    } data[20];
} IODBSIG;  /* In case that the number of data is 20 */


/*-------------*/
/* CNC: Others */
/*-------------*/

/* cnc_sysinfo:read CNC system information */
typedef struct odbsys {
    short   addinfo ;       /* additional information  */
    short   max_axis ;      /* maximum axis number */
    char    cnc_type[2] ;   /* cnc type <ascii char> */
    char    mt_type[2] ;    /* M/T/TT <ascii char> */
    char    series[4] ;     /* series NO. <ascii char> */
    char    version[4] ;    /* version NO.<ascii char> */
    char    axes[2] ;       /* axis number<ascii char> */
} ODBSYS ;

/* cnc_statinfo:read CNC status information */
typedef struct odbst {
    short   dummy ;     /* dummy */
    short   tmmode ;    /* T/M mode */
    short   aut ;       /* selected automatic mode */
    short   run ;       /* running status */
    short   motion ;    /* axis, dwell status */
    short   mstb ;      /* m, s, t, b status */
    short   emergency ; /* emergency stop status */
    short   alarm ;     /* alarm status */
    short   edit ;      /* editting status */
} ODBST ;

/* cnc_alarm:read alarm status */
typedef struct odbalm {
    short   dummy[2] ;  /* dummy */
    short   data ;      /* alarm status */
} ODBALM ;

/* cnc_rdalminfo:read alarm information */
typedef struct alminfo {
    union {
        struct {
            struct {
                short   axis ;
                short   alm_no ;
            } alm[5] ;
            short   data_end ;
        } alm1 ;
        struct {
            struct {
                short   axis ;
                short   alm_no ;
                short   msg_len ;
                char    alm_msg[32] ;
            } alm[5] ;
            short   data_end ;
        }alm2 ;
    } u ;
} ALMINFO ; /* In case that the number of alarm is 5 */

/* cnc_modal:read modal data */
typedef struct odbmdl {
    short   datano;
    short   type;
    union {
        char    g_data;
        char    g_rdata[21];
        char    g_1shot[4];
        struct {
            long    aux_data;
            char    flag1;
            char    flag2;
        }aux;
        struct {
            long    aux_data;
            char    flag1;
            char    flag2;
        }raux1[27];
        struct {
            long    aux_data;
            char    flag1;
            char    flag2;
        }raux2[MAX_AXIS];
    }modal;
} ODBMDL ;

/* cnc_diagnoss:read diagnosis data */
/* cnc_diagnosr:read diagnosis data(area specified) */
typedef struct odbdgn {
    short   datano ;    /* data number */
    short   type ;      /* axis number */
    union {
        char    cdata ; /* diagnosis data */
        short   idata ;
        long    ldata ;
        char    cdatas[MAX_AXIS] ;
        short   idatas[MAX_AXIS] ;
        long    ldatas[MAX_AXIS] ;
    } u ;
} ODBDGN ;

/* cnc_adcnv:read A/D conversion data */
typedef struct odbad {
    short   datano ;    /* input analog voltage type */
    short   type ;      /* analog voltage type */
    short   data ;      /* digital voltage data */
} ODBAD ;

/* cnc_rdopmsg:read operator's message */
typedef struct msg {
    short   datano ;    /* operator's message number */
    short   type ;      /* operator's message type */
    short   char_num ;  /* message string length */
    char    data[256] ; /* operator's message string */
} OPMSG ;   /* In case that the data length is 256 */

/* cnc_rdopmsg2:read operator's message */
typedef struct opmsg2 {
    short   datano ;    /* operator's message number */
    short   type ;      /* operator's message type */
    short   char_num ;  /* message string length */
    char    data[64] ;  /* operator's message string */
} OPMSG2 ;   /* In case that the data length is 64 */

/* cnc_rdopmsg3:read operator's message */
typedef struct opmsg3 {
    short   datano ;    /* operator's message number */
    short   type ;      /* operator's message type */
    short   char_num ;  /* message string length */
    char    data[256] ;  /* operator's message string */
} OPMSG3 ;   /* In case that the data length is 256 */

/* cnc_sysconfig:read CNC configuration information */
typedef struct odbsysc {
    char    slot_no_p[16];
    char    slot_no_l[16];
    short   mod_id[16];
    short   soft_id[16];
    char    s_series[16][5];
    char    s_version[16][5];
    char    dummy[16];
    short   m_rom;
    short   s_rom;
    char    svo_soft[8];
    char    pmc_soft[6];
    char    lad_soft[6];
    char    mcr_soft[8];
    char    spl1_soft[6];
    char    spl2_soft[6];
    short   frmmin;
    short   drmmin;
    short   srmmin;
    short   pmcmin;
    short   crtmin;
    short   sv1min;
    short   sv3min;
    short   sicmin;
    short   posmin;
    short   drmmrc;
    short   drmarc;
    short   pmcmrc;
    short   dmaarc;
    short   iopt;
    short   hdiio;
    short   frmsub;
    short   drmsub;
    short   srmsub;
    short   sv5sub;
    short   sv7sub;
    short   sicsub;
    short   possub;
    short   hamsub;
    short   gm2gr1;
    short   crtgr2;
    short   gm1gr2;
    short   gm2gr2;
    short   cmmrb;
    short   sv5axs;
    short   sv7axs;
    short   sicaxs;
    short   posaxs;
    short   hanaxs;
    short   romr64;
    short   srmr64;
    short   dr1r64;
    short   dr2r64;
    short   iopio2;
    short   hdiio2;
    short   cmmrb2;
    short   romfap;
    short   srmfap;
    short   drmfap;
} ODBSYSC ;

/* cnc_rdprstrinfo:read program restart information */
typedef struct odbprs {
    short   datano;         /* dummy */
    short   type;           /* dummy */
    short   data_info[5];   /* data setting information */
    long    rstr_bc;        /* block counter */
    long    rstr_m[35];     /* M code value */
    long    rstr_t[2];      /* T code value */
    long    rstr_s;         /* S code value */
    long    rstr_b;         /* B code value */
    long    dest[MAX_AXIS]; /* program re-start position */
    long    dist[MAX_AXIS]; /* program re-start distance */
} ODBPRS;

/* cnc_rdopnlsgnl:read output signal image of software operator's panel */
/* cnc_wropnlsgnl:write output signal of software operator's panel */
typedef struct iodbsgnl {
    short   datano;     /* dummy */
    short   type;       /* data select flag */
    short   mode;       /* mode signal */
    short   hndl_ax;    /* Manual handle feed axis selection signal */
    short   hndl_mv;    /* Manual handle feed travel distance selection signal */
    short   rpd_ovrd;   /* rapid traverse override signal */
    short   jog_ovrd;   /* manual feedrate override signal */
    short   feed_ovrd;  /* feedrate override signal */
    short   spdl_ovrd;  /* (not used) */
    short   blck_del;   /* optional block skip signal */
    short   sngl_blck;  /* single block signal */
    short   machn_lock; /* machine lock signal */
    short   dry_run;    /* dry run signal */
    short   mem_prtct;  /* memory protection signal */
    short   feed_hold;  /* automatic operation halt signal */
} IODBSGNL;

/* cnc_rdopnlgnrl:read general signal image of software operator's panel */
/* cnc_wropnlgnrl:write general signal image of software operator's panel */
typedef struct iodbgnrl {
    short   datano; /* dummy */
    short   type;   /* data select flag */
    char    sgnal;  /* general signal */
} IODBGNRL;

/* cnc_rdopnlgsname:read general signal name of software operator's panel */
/* cnc_wropnlgsname:write general signal name of software operator's panel*/
typedef struct iodbrdna {
    short   datano;         /* dummy */
    short   type;           /* data select flag */
    char    sgnl1_name[9];  /* general signal 1 name */
    char    sgnl2_name[9];  /* general signal 2 name */
    char    sgnl3_name[9];  /* general signal 3 name */
    char    sgnl4_name[9];  /* general signal 4 name */
    char    sgnl5_name[9];  /* general signal 5 name */
    char    sgnl6_name[9];  /* general signal 6 name */
    char    sgnl7_name[9];  /* general signal 7 name */
    char    sgnl8_name[9];  /* general signal 8 name */
} IODBRDNA;

/* cnc_getdtailerr:get detail error */
typedef struct odberr {
    short   err_no ;
    short   err_dtno ;
} ODBERR ;

/* cnc_rdparainfo:read informations of CNC parameter */
typedef struct odbparaif {
    unsigned short  info_no ;
    short   prev_no ;
    short   next_no ;
    struct {
        short   prm_no ;
        short   prm_type ;
    } info[10] ;    /* In case that the number of data is 10 */
} ODBPARAIF ;

/* cnc_rdsetinfo:read informations of CNC setting data */
typedef struct odbsetif {
    unsigned short  info_no ;
    short   prev_no ;
    short   next_no ;
    struct {
        short   set_no ;
        short   set_type ;
    } info[10] ;    /* In case that the number of data is 10 */
} ODBSETIF ;

/* cnc_rddiaginfo:read informations of CNC diagnose data */
typedef struct odbdiagif {
    unsigned short  info_no ;
    short   prev_no ;
    short   next_no ;
    struct {
        short   diag_no ;
        short   diag_type ;
    } info[10] ;    /* In case that the number of data is 10 */
} ODBDIAGIF ;

/* cnc_rdparanum:read maximum, minimum and total number of CNC parameter */
typedef struct odbparanum {
    unsigned short  para_min ;
    unsigned short  para_max ;
    unsigned short  total_no ;
} ODBPARANUM ;

/* cnc_rdsetnum:read maximum, minimum and total number of CNC setting data */
typedef struct odbsetnum {
    unsigned short  set_min ;
    unsigned short  set_max ;
    unsigned short  total_no ;
} ODBSETNUM ;

/* cnc_rddiagnum:read maximum, minimum and total number of CNC diagnose data */
typedef struct odbdiagnum {
    unsigned short  diag_min ;
    unsigned short  diag_max ;
    unsigned short  total_no ;
} ODBDIAGNUM ;

/* cnc_rdposerrs2:read the position deviation S1 and S2 */
typedef struct odbpser {
    long    poserr1 ;
    long    poserr2 ;
} ODBPSER ;

/* cnc_rdctrldi:read the control input signal */
typedef struct odbspdi {
    char    sgnl1 ;
    char    sgnl2 ;
    char    sgnl3 ;
    char    sgnl4 ;
} ODBSPDI ;

/* cnc_rdctrldo:read the control output signal */
typedef struct odbspdo {
    char    sgnl1 ;
    char    sgnl2 ;
    char    sgnl3 ;
    char    sgnl4 ;
} ODBSPDO ;

/* cnc_rdwaveprm:read the parameter of wave diagnosis */
/* cnc_wrwaveprm:write the parameter of wave diagnosis */
typedef struct iodbwave {
    short   condition ;
    char    trg_adr ;
    char    trg_bit ;
    short   trg_no ;
    short   delay ;
    short   t_range ;
    struct {
        short   kind ;
        union {
            short   axis ;
            struct {
                char    adr ;
                char    bit ;
                short   no ;
            } io ;
        } u ;
    } ch[12] ;
} IODBWAVE ;

/* cnc_rdwaveprm2:read the parameter of wave diagnosis 2 */
/* cnc_wrwaveprm2:write the parameter of wave diagnosis 2 */
typedef struct iodbwvprm {
    short   condition ;
    char    trg_adr ;
    char    trg_bit ;
    short   trg_no ;
    short   reserve1 ;
    long    delay ;
    long    t_range ;
    struct {
        short   kind ;
        union {
            long    axis ;
            struct {
                char    adr ;
                char    bit ;
                short   no ;
            } io ;
        } u ;
        long    reserve2 ;
    } ch[12] ;
} IODBWVPRM ;

/* cnc_rdwavedata:read the data of wave diagnosis */
typedef struct odbwvdt {
    short   channel ;
    short   kind ;
    union {
        short   axis ;
        struct {
            char    adr ;
            char    bit ;
            short   no ;
        } io ;
    } u ;
    char    year ;
    char    month ;
    char    day ;
    char    hour ;
    char    minute ;
    char    second ;
    short   t_cycle ;
    short   data[8192] ;
} ODBWVDT ;

/* cnc_rdprogdir2:read program directory 2 */
typedef struct prgdir2 {
        short   number ;
        long    length ;
        char    comment[51] ;
        char    dummy ;
} PRGDIR2;

/* cnc_rdwkcdshft:read work coordinate shift */
/* cnc_wrwkcdshft:write work coordinate shift */
/* cnc_rdwkcdsfms:read work coordinate shift measure */
/* cnc_wrwkcdsfms:write work coordinate shift measure */
typedef struct iodbwcsf {
     short      datano;         /* datano                       */
     short      type;           /* axis number                  */
     long       data[MAX_AXIS]; /* data                         */
} IODBWCSF;

/* cnc_rdomhisinfo:read operator message history information */
typedef struct odbomif {
        unsigned short  om_max ;   /* maximum operator message history */
        unsigned short  om_sum ;   /* actually operator message history */
        unsigned short  om_char ;  /* maximum character (include NULL) */
} ODBOMIF ;

/* cnc_rdomhistry:read operator message history */
typedef struct odbomhis {
        short   om_no;  /* operator message number */
        short   year;   /* year */
        short   month;  /* month */
        short   day;    /* day */
        short   hour;   /* hour */
        short   minute; /* mimute */
        short   second; /* second */
        char    om_msg[256];    /* operator message */
} ODBOMHIS ;

/* cnc_rdsyssoft:read CNC system soft series and version */
typedef struct odbsyss {
     char    slot_no_p[16];
     char    slot_no_l[16];
     short   module_id[16];
     short   soft_id[16];
     char    soft_series[16][5];
     char    soft_version[16][5];
     short   soft_inst;
     char    boot_ser[5];
     char    boot_ver[5];
     char    servo_ser[5];
     char    servo_ver[5];
     char    pmc_ser[5];
     char    pmc_ver[5];
     char    ladder_ser[5];
     char    ladder_ver[5];
     char    mcrlib_ser[5];
     char    mcrlib_ver[5];
     char    mcrapl_ser[5];
     char    mcrapl_ver[5];
     char    spl1_ser[5];
     char    spl1_ver[5];
     char    spl2_ser[5];
     char    spl2_ver[5];
     char    spl3_ser[5];
     char    spl3_ver[5];
     char    c_exelib_ser[5];
     char    c_exelib_ver[5];
     char    c_exeapl_ser[5];
     char    c_exeapl_ver[5];
     char    int_vga_ser[5];
     char    int_vga_ver[5];
     char    out_vga_ser[5];
     char    out_vga_ver[5];
     char    pmm_ser[5];
     char    pmm_ver[5];
     char    pmc_mng_ser[5];
     char    pmc_mng_ver[5];
     char    pmc_shin_ser[5];
     char    pmc_shin_ver[5];
     char    pmc_shout_ser[5];
     char    pmc_shout_ver[5];
     char    pmc_c_ser[5];
     char    pmc_c_ver[5];
     char    pmc_edit_ser[5];
     char    pmc_edit_ver[5];
     char    lddr_mng_ser[5];
     char    lddr_mng_ver[5];
     char    lddr_apl_ser[5];
     char    lddr_apl_ver[5];
     char    spl4_ser[5];
     char    spl4_ver[5];
     char    mcr2_ser[5];
     char    mcr2_ver[5];
     char    mcr3_ser[5];
     char    mcr3_ver[5];
     char    eth_boot_ser[5];
     char    eth_boot_ver[5];
     char    reserve[8][5];
} ODBSYSS ;

/* cnc_rdmdlconfig:read CNC module configuration information */
typedef struct odbmdlc {
     short   from;
     short   dram;
     short   sram;
     short   pmc;
     short   crtc;
     short   servo12;
     short   servo34;
     short   servo56;
     short   servo78;
     short   sic;
     short   pos_lsi;
     short   hi_aio;
     short   reserve[12];
     short   drmmrc;
     short   drmarc;
     short   pmcmrc;
     short   dmaarc;
     short   iopt;
     short   hdiio;
     short   gm2gr1;
     short   crtgr2;
     short   gm1gr2;
     short   gm2gr2;
     short   cmmrb;
     short   sv5axs;
     short   sv7axs;
     short   sicaxs;
     short   posaxs;
     short   hamaxs;
     short   romr64;
     short   srmr64;
     short   dr1r64;
     short   dr2r64;
     short   iopio2;
     short   hdiio2;
     short   cmmrb2;
     short   romfap;
     short   srmfap;
     short   drmfap;
     short   drmare;
     short   pmcmre;
     short   dmaare;
     short   frmbgg;
     short   drmbgg;
     short   asrbgg;
     short   edtpsc;
     short   slcpsc;
     short   reserve2[34];
} ODBMDLC ;

/* cnc_gettimer:get date and time from cnc */
/* cnc_settimer:set date and time for cnc */
typedef struct iodbtimer {
        short   type ;
        short   dummy ;
        union {
                struct {
                        short   year ;
                        short   month ;
                        short   date ;
                } date ;
                struct {
                        short   hour ;
                        short   minute ;
                        short   second ;
                } time ;
        } data ;
} IODBTIMER ;


/*-----------*/
/* Oxxxxxxxx */
/*-----------*/

/* cnc_rdprgnumo8:read program number under execution */
typedef struct odbproo8 {
     short   dummy[2] ;  /* dummy */
     long    data ;      /* running program number */
     long    mdata ;     /* main program number */
} ODBPROO8 ;

/* cnc_rddynamico8:read all dynamic data */
typedef struct odbdyo8 {
    short   dummy ;
    short   axis ;       /* axis number             */
    short   alarm ;      /* alarm status            */
    long    prgnum ;     /* current program number  */
    long    prgmnum ;    /* main program number     */
    long    seqnum ;     /* current sequence number */
    long    actf ;       /* actual feedrate         */
    long    acts ;       /* actual spindle speed    */
    union {
        struct {
            long    absolute[MAX_AXIS] ;    /* absolute position   */
            long    machine[MAX_AXIS] ;     /* machine position    */
            long    relative[MAX_AXIS] ;    /* relative position   */
            long    distance[MAX_AXIS] ;    /* distance to go      */
        } faxis ;
        struct {
            long    absolute ;  /* absolute position   */
            long    machine ;   /* machine position    */
            long    relative ;  /* relative position   */
            long    distance ;  /* distance to go      */
        } oaxis ;   /* In case of no axis  */
    } pos ;
} ODBDYO8 ;

/* (op)cnc_rdmdipntro8:read execution pointer for MDI operation */
typedef struct odbmdipo8 {
    long   mdiprog;     /* exec. program number */
    long   mdipntr;     /* exec. pointer */
    long   crntprog;    /* prepare program number */
    long   crntpntr;    /* prepare pointer */
} ODBMDIPO8;

/* cnc_rdprogdir2o8:read program directory 2 */
typedef struct prgdir2o8 {
        long    number ;
        long    length ;
        char    comment[51] ;
        char    dummy ;
} PRGDIR2O8;


/*-----*/
/* PMC */
/*-----*/

/* pmc_rdpmcrng:read PMC data(area specified) */
/* pmc_wrpmcrng:write PMC data(area specified) */
typedef struct iodbpmc {
    short   type_a ;    /* PMC address type */
    short   type_d ;    /* PMC data type */
    short   datano_s ;  /* start PMC address */
    short   datano_e ;  /* end PMC address */
    union {
        char    cdata[5] ;  /* PMC data */
        short   idata[5] ;
        long    ldata[5] ;
    } u ;
} IODBPMC ; /* In case that the number of data is 5 */

/* pmc_rdpmcinfo:read informations of PMC data */
typedef struct odbpmcinf {
    short   datano ;
    struct {
        char    pmc_adr ;
        char    adr_attr ;
        unsigned short  top_num ;
        unsigned short  last_num ;
    } info[64] ;
} ODBPMCINF ;

/* pmc_rdcntldata:read PMC parameter data table control data */
/* pmc_wrcntldata:write PMC parameter data table control data */
typedef struct iodbpmccntl {
    short   datano_s ;
    short   dummy ;
    short   datano_e ;
    struct {
        char    tbl_prm ;
        char    data_type ;
        unsigned short  data_size ;
        unsigned short  data_dsp ;
        short   dummy ;
    } info[100] ;
} IODBPMCCNTL ;

/* pmc_rdalmmsg:read PMC alarm message */
typedef struct  odbpmcalm {
    char    almmsg[128] ;    /* alarm message */
} ODBPMCALM ;

/* pmc_getdtailerr:get detail error for pmc */
typedef struct odbpmcerr {
    short   err_no ;
    short   err_dtno ;
} ODBPMCERR ;


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

/*-------------------------------------*/
/* CNC: Control axis / spindle related */
/*-------------------------------------*/

/* read actual axis feedrate(F) */
 FWLIBAPI short WINAPI cnc_actf( unsigned short, ODBACT * ) ;

/* read absolute axis position */
 FWLIBAPI short WINAPI cnc_absolute( unsigned short, short, short, ODBAXIS * ) ;

/* read machine axis position */
 FWLIBAPI short WINAPI cnc_machine( unsigned short, short, short, ODBAXIS * ) ;

/* read relative axis position */
 FWLIBAPI short WINAPI cnc_relative( unsigned short, short, short, ODBAXIS * ) ;

/* read distance to go */
 FWLIBAPI short WINAPI cnc_distance( unsigned short, short, short, ODBAXIS * ) ;

/* read skip position */
 FWLIBAPI short WINAPI cnc_skip( unsigned short, short, short, ODBAXIS * ) ;

/* read servo delay value */
 FWLIBAPI short WINAPI cnc_srvdelay( unsigned short, short, short, ODBAXIS * ) ;

/* read acceleration/deceleration delay value */
 FWLIBAPI short WINAPI cnc_accdecdly( unsigned short, short, short, ODBAXIS * ) ;

/* read all dynamic data */
 FWLIBAPI short WINAPI cnc_rddynamic( unsigned short, short, short, ODBDY * ) ;

/* read actual spindle speed(S) */
 FWLIBAPI short WINAPI cnc_acts( unsigned short, ODBACT * ) ;

/* read actual spindle speed(S) (All or spesified) */
 FWLIBAPI short WINAPI cnc_acts2( unsigned short, short, ODBACT2 * ) ;

/* set origin / preset relative axis position */
 FWLIBAPI short WINAPI cnc_wrrelpos( unsigned short, short, IDBWRR * ) ;

/* preset work coordinate */
 FWLIBAPI short WINAPI cnc_prstwkcd( unsigned short, short, IDBWRA * ) ;

/* read manual overlapped motion value */
 FWLIBAPI short WINAPI cnc_rdmovrlap( unsigned short, short, short, IODBOVL * ) ;

/* cancel manual overlapped motion value */
 FWLIBAPI short WINAPI cnc_canmovrlap( unsigned short, short ) ;

/* read load information of serial spindle */
 FWLIBAPI short WINAPI cnc_rdspload( unsigned short, short, ODBSPN * ) ;

/* read maximum r.p.m. ratio of serial spindle */
 FWLIBAPI short WINAPI cnc_rdspmaxrpm( unsigned short, short, ODBSPN * ) ;

/* read gear ratio of serial spindle */
 FWLIBAPI short WINAPI cnc_rdspgear( unsigned short, short, ODBSPN * ) ;

/* read absolute axis position 2 */
 FWLIBAPI short WINAPI cnc_absolute2( unsigned short, short, short, ODBAXIS * ) ;

/* read relative axis position 2 */
 FWLIBAPI short WINAPI cnc_relative2( unsigned short, short, short, ODBAXIS * ) ;


/*----------------------*/
/* CNC: Program related */
/*----------------------*/

/* start downloading NC program */
 FWLIBAPI short WINAPI cnc_dwnstart( unsigned short ) ;

/* download NC program */
 FWLIBAPI short WINAPI cnc_download( unsigned short, char *, short ) ;

/* download NC program(conditional) */
 FWLIBAPI short WINAPI cnc_cdownload( unsigned short, char *, short ) ;

/* end of downloading NC program */
 FWLIBAPI short WINAPI cnc_dwnend( unsigned short ) ;

/* end of downloading NC program        '980629 */
 FWLIBAPI short WINAPI cnc_dwnend2( unsigned short, char * ) ;

/* start verification of NC program */
 FWLIBAPI short WINAPI cnc_vrfstart( unsigned short ) ;

/* verify NC program */
 FWLIBAPI short WINAPI cnc_verify( unsigned short, char *, short ) ;

/* verify NC program(conditional) */
 FWLIBAPI short WINAPI cnc_cverify( unsigned short, char *, short ) ;

/* end of verification */
 FWLIBAPI short WINAPI cnc_vrfend( unsigned short ) ;

/* start downloading DNC program */
 FWLIBAPI short WINAPI cnc_dncstart( unsigned short ) ;

/* download DNC program */
 FWLIBAPI short WINAPI cnc_dnc( unsigned short, char *, unsigned short ) ;

/* download DNC program(conditional) */
 FWLIBAPI short WINAPI cnc_cdnc( unsigned short, char *, unsigned short ) ;

/* end of downloading DNC program */
 FWLIBAPI short WINAPI cnc_dncend( unsigned short ) ;

/* start downloading DNC program 2 */
 FWLIBAPI short WINAPI cnc_dncstart2( unsigned short, char * ) ;

/* download DNC program 2 */
 FWLIBAPI short WINAPI cnc_dnc2( unsigned short, long *, char * ) ;

/* end of downloading DNC program 2 */
 FWLIBAPI short WINAPI cnc_dncend2( unsigned short, short ) ;

/* read the diagnosis data of DNC operation */
 FWLIBAPI short WINAPI cnc_rddncdgndt( unsigned short, ODBDNCDGN * ) ;

/* start uploading NC program */
 FWLIBAPI short WINAPI cnc_upstart( unsigned short, short ) ;

/* upload NC program */
 FWLIBAPI short WINAPI cnc_upload( unsigned short, ODBUP *, unsigned short * ) ;

/* upload NC program(conditional) */
 FWLIBAPI short WINAPI cnc_cupload( unsigned short, ODBUP *, unsigned short * ) ;

/* end of uploading NC program */
 FWLIBAPI short WINAPI cnc_upend( unsigned short ) ;

/* read buffer status for downloading/verification NC program */
 FWLIBAPI short WINAPI cnc_buff( unsigned short, ODBBUF * ) ;

/* search specified program */
 FWLIBAPI short WINAPI cnc_search( unsigned short, short ) ;

/* delete all programs */
 FWLIBAPI short WINAPI cnc_delall( unsigned short ) ;

/* delete specified program */
 FWLIBAPI short WINAPI cnc_delete( unsigned short, short ) ;

/* read program directory */
 FWLIBAPI short WINAPI cnc_rdprogdir( unsigned short, short, short, short, unsigned short, PRGDIR * ) ;

/* read program information */
 FWLIBAPI short WINAPI cnc_rdproginfo( unsigned short, short, short, ODBNC * ) ;

/* read program number under execution */
 FWLIBAPI short WINAPI cnc_rdprgnum( unsigned short, ODBPRO * ) ;

/* read sequence number under execution */
 FWLIBAPI short WINAPI cnc_rdseqnum( unsigned short, ODBSEQ * ) ;

/* search specified sequence number */
 FWLIBAPI short WINAPI cnc_seqsrch( unsigned short, long ) ;

/* rewind cursor of NC program */
 FWLIBAPI short WINAPI cnc_rewind( unsigned short ) ;

/* read block counter */
 FWLIBAPI short WINAPI cnc_rdblkcount( unsigned short, long * ) ;

/* read program under execution */
 FWLIBAPI short WINAPI cnc_rdexecprog( unsigned short, unsigned short *, short *, char * ) ;

/* write program for MDI operation */
 FWLIBAPI short WINAPI cnc_wrmdiprog( unsigned short, short, char * ) ;

/* read execution pointer for MDI operation */
 FWLIBAPI short WINAPI cnc_rdmdipntr( unsigned short, ODBMDIP * ) ;

/* write execution pointer for MDI operation */
 FWLIBAPI short WINAPI cnc_wrmdipntr( unsigned short, long ) ;


/*---------------------------*/
/* CNC: NC file data related */
/*---------------------------*/

/* read tool offset value */
 FWLIBAPI short WINAPI cnc_rdtofs( unsigned short, short, short, short, ODBTOFS * ) ;

/* write tool offset value */
 FWLIBAPI short WINAPI cnc_wrtofs( unsigned short, short, short, short, long ) ;

/* read tool offset value(area specified) */
 FWLIBAPI short WINAPI cnc_rdtofsr( unsigned short, short, short, short, short, IODBTO * ) ;

/* write tool offset value(area specified) */
 FWLIBAPI short WINAPI cnc_wrtofsr( unsigned short, short, IODBTO * ) ;

/* read work zero offset value */
 FWLIBAPI short WINAPI cnc_rdzofs( unsigned short, short, short, short, IODBZOFS * ) ;

/* write work zero offset value */
 FWLIBAPI short WINAPI cnc_wrzofs( unsigned short, short, IODBZOFS * ) ;

/* read work zero offset value(area specified) */
 FWLIBAPI short WINAPI cnc_rdzofsr( unsigned short, short, short, short, short, IODBZOR * ) ;

/* write work zero offset value(area specified) */
 FWLIBAPI short WINAPI cnc_wrzofsr( unsigned short, short, IODBZOR * ) ;

/* read parameter */
 FWLIBAPI short WINAPI cnc_rdparam( unsigned short, short, short, short, IODBPSD * ) ;

/* write parameter */
 FWLIBAPI short WINAPI cnc_wrparam( unsigned short, short, IODBPSD * ) ;

/* read parameter(area specified) */
 FWLIBAPI short WINAPI cnc_rdparar( unsigned short, short *, short, short *, short *, void * ) ;

/* write parameter(area specified) */
 FWLIBAPI short WINAPI cnc_wrparas( unsigned short, short, void * ) ;

/* read setting data */
 FWLIBAPI short WINAPI cnc_rdset( unsigned short, short, short, short, IODBPSD * ) ;

/* write setting data */
 FWLIBAPI short WINAPI cnc_wrset( unsigned short, short, IODBPSD * ) ;

/* read setting data(area specified) */
 FWLIBAPI short WINAPI cnc_rdsetr( unsigned short, short *, short, short *, short *, void * ) ;

/* write setting data(area specified) */
 FWLIBAPI short WINAPI cnc_wrsets( unsigned short, short, void * ) ;

/* read pitch error compensation data(area specified) */
 FWLIBAPI short WINAPI cnc_rdpitchr( unsigned short, short, short, short, IODBPI * ) ;

/* write pitch error compensation data(area specified) */
 FWLIBAPI short WINAPI cnc_wrpitchr( unsigned short, short, IODBPI * ) ;

/* read custom macro variable */
 FWLIBAPI short WINAPI cnc_rdmacro( unsigned short, short, short, ODBM * ) ;

/* write custom macro variable */
 FWLIBAPI short WINAPI cnc_wrmacro( unsigned short, short, short, long, short ) ;

/* read custom macro variables(area specified) */
 FWLIBAPI short WINAPI cnc_rdmacror( unsigned short, short, short, short, IODBMR * ) ;

/* write custom macro variables(area specified) */
 FWLIBAPI short WINAPI cnc_wrmacror( unsigned short, short, IODBMR * ) ;

/* read P code macro variable */
 FWLIBAPI short WINAPI cnc_rdpmacro( unsigned short, long, ODBPM * ) ;

/* write P code macro variable */
 FWLIBAPI short WINAPI cnc_wrpmacro( unsigned short, long, long, short ) ;

/* read P code macro variables(area specified) */
 FWLIBAPI short WINAPI cnc_rdpmacror( unsigned short, long, long, unsigned short, IODBPR * ) ;

/* write P code macro variables(area specified) */
 FWLIBAPI short WINAPI cnc_wrpmacror( unsigned short, unsigned short, IODBPR * ) ;

/* read tool offset information */
 FWLIBAPI short WINAPI cnc_rdtofsinfo( unsigned short, ODBTLINF * ) ;

/* read work zero offset information */
 FWLIBAPI short WINAPI cnc_rdzofsinfo( unsigned short, short * ) ;

/* read pitch error compensation data information */
 FWLIBAPI short WINAPI cnc_rdpitchinfo( unsigned short, short * ) ;

/* read custom macro variable information */
 FWLIBAPI short WINAPI cnc_rdmacroinfo( unsigned short, ODBMVINF * ) ;

/* read P code macro variable information */
 FWLIBAPI short WINAPI cnc_rdpmacroinfo( unsigned short, ODBPMINF * ) ;


/*----------------------------------------*/
/* CNC: Tool life management data related */
/*----------------------------------------*/

/* read tool life management data(tool group number) */
 FWLIBAPI short WINAPI cnc_rdgrpid( unsigned short, short, ODBTLIFE1 * ) ;

/* read tool life management data(number of tool groups) */
 FWLIBAPI short WINAPI cnc_rdngrp( unsigned short, ODBTLIFE2 * ) ;

/* read tool life management data(number of tools) */
 FWLIBAPI short WINAPI cnc_rdntool( unsigned short, short, ODBTLIFE3 * ) ;

/* read tool life management data(tool life) */
 FWLIBAPI short WINAPI cnc_rdlife( unsigned short, short, ODBTLIFE3 * ) ;

/* read tool life management data(tool lift counter) */
 FWLIBAPI short WINAPI cnc_rdcount( unsigned short, short, ODBTLIFE3 * ) ;

/* read tool life management data(tool length number-1) */
 FWLIBAPI short WINAPI cnc_rd1length( unsigned short, short, short, ODBTLIFE4 * ) ;

/* read tool life management data(tool length number-2) */
 FWLIBAPI short WINAPI cnc_rd2length( unsigned short, short, short, ODBTLIFE4 * ) ;

/* read tool life management data(cutter compensation no.-1) */
 FWLIBAPI short WINAPI cnc_rd1radius( unsigned short, short, short, ODBTLIFE4 * ) ;

/* read tool life management data(cutter compensation no.-2) */
 FWLIBAPI short WINAPI cnc_rd2radius( unsigned short, short, short, ODBTLIFE4 * ) ;

/* read tool life management data(tool information-1) */
 FWLIBAPI short WINAPI cnc_t1info( unsigned short, short, short, ODBTLIFE4 * ) ;

/* read tool life management data(tool information-2) */
 FWLIBAPI short WINAPI cnc_t2info( unsigned short, short, short, ODBTLIFE4 * ) ;

/* read tool life management data(tool number) */
 FWLIBAPI short WINAPI cnc_toolnum( unsigned short, short, short, ODBTLIFE4 * ) ;

/* read tool life management data(tool number, tool life, tool life counter)(area specified) */
 FWLIBAPI short WINAPI cnc_rdtoolrng( unsigned short, short, short, short, IODBTR * ) ;

/* read tool life management data(all data within group) */
 FWLIBAPI short WINAPI cnc_rdtoolgrp( unsigned short, short, short, ODBTG * ) ;

/* write tool life management data(tool life counter) (area specified) */
 FWLIBAPI short WINAPI cnc_wrcountr( unsigned short, short, IDBWRC * ) ;

/* read tool life management data(used tool group number) */
 FWLIBAPI short WINAPI cnc_rdusegrpid( unsigned short, ODBUSEGR * ) ;

/* read tool life management data(max. number of tool groups) */
 FWLIBAPI short WINAPI cnc_rdmaxgrp( unsigned short, ODBLFNO * ) ;

/* read tool life management data(maximum number of tool within group) */
 FWLIBAPI short WINAPI cnc_rdmaxtool( unsigned short, ODBLFNO * ) ;

/* read tool life management data(used tool no. within group) */
 FWLIBAPI short WINAPI cnc_rdusetlno( unsigned short, short, short, short, ODBTLUSE * ) ;

/* read tool life management data(tool data1) */
 FWLIBAPI short WINAPI cnc_rd1tlifedata( unsigned short, short, short, IODBTD * ) ;

/* read tool life management data(tool data2) */
 FWLIBAPI short WINAPI cnc_rd2tlifedata( unsigned short, short, short, IODBTD * ) ;

/* write tool life management data(tool data1) */
 FWLIBAPI short WINAPI cnc_wr1tlifedata( unsigned short, IODBTD * ) ;

/* write tool life management data(tool data2) */
 FWLIBAPI short WINAPI cnc_wr2tlifedata( unsigned short, IODBTD * ) ;

/* read tool life management data(tool group information) */
 FWLIBAPI short WINAPI cnc_rdgrpinfo( unsigned short, short, short, short, IODBTGI * ) ;

/* write tool life management data(tool group information) */
 FWLIBAPI short WINAPI cnc_wrgrpinfo( unsigned short, short, IODBTGI * ) ;

/* delete tool life management data(tool group) */
 FWLIBAPI short WINAPI cnc_deltlifegrp( unsigned short, short ) ;

/* insert tool life management data(tool data) */
 FWLIBAPI short WINAPI cnc_instlifedt( unsigned short, IDBITD * ) ;

/* delete tool life management data(tool data) */
 FWLIBAPI short WINAPI cnc_deltlifedt( unsigned short, short, short ) ;

/* clear tool life management data(tool life counter, tool information)(area specified) */
 FWLIBAPI short WINAPI cnc_clrcntinfo( unsigned short, short, short ) ;

/* read tool life management data(tool group number) 2 */
 FWLIBAPI short WINAPI cnc_rdgrpid2( unsigned short, long, ODBTLIFE5 * ) ;

/* read tool life management data(tool data1) 2 */
 FWLIBAPI short WINAPI cnc_rd1tlifedat2( unsigned short, short, long, IODBTD2 * ) ;

/* write tool life management data(tool data1) 2 */
 FWLIBAPI short WINAPI cnc_wr1tlifedat2( unsigned short, IODBTD2 * ) ;


/*-------------------------------------*/
/* CNC: Operation history data related */
/*-------------------------------------*/

/* stop logging operation history data */
 FWLIBAPI short WINAPI cnc_stopophis( unsigned short ) ;

/* restart logging operation history data */
 FWLIBAPI short WINAPI cnc_startophis( unsigned short ) ;

/* read number of operation history data */
 FWLIBAPI short WINAPI cnc_rdophisno( unsigned short, unsigned short * ) ;

/* read operation history data */
 FWLIBAPI short WINAPI cnc_rdophistry( unsigned short, unsigned short, unsigned short, unsigned short, ODBHIS * ) ;

/* read number of alarm history data */
 FWLIBAPI short WINAPI cnc_rdalmhisno( unsigned short, unsigned short * ) ;

/* read alarm history data */
 FWLIBAPI short WINAPI cnc_rdalmhistry( unsigned short, unsigned short, unsigned short, unsigned short, ODBAHIS * ) ;

/* clear operation history data */
 FWLIBAPI short WINAPI cnc_clearophis( unsigned short, short ) ;

/* read signals related operation history */
 FWLIBAPI short WINAPI cnc_rdhissgnl( unsigned short, IODBSIG * ) ;

/* write signals related operation history */
 FWLIBAPI short WINAPI cnc_wrhissgnl( unsigned short, IODBSIG * ) ;


/*-------------*/
/* CNC: Others */
/*-------------*/

/* read CNC system information */
 FWLIBAPI short WINAPI cnc_sysinfo( unsigned short, ODBSYS * ) ;

/* read CNC status information */
 FWLIBAPI short WINAPI cnc_statinfo( unsigned short, ODBST * ) ;

/* read alarm status */
 FWLIBAPI short WINAPI cnc_alarm( unsigned short, ODBALM * ) ;

/* read alarm information */
 FWLIBAPI short WINAPI cnc_rdalminfo( unsigned short, short, short, short, ALMINFO * ) ;

/* clear CNC alarm */
 FWLIBAPI short WINAPI cnc_clralm( unsigned short hndl, short id ) ;

/* read modal data */
 FWLIBAPI short WINAPI cnc_modal( unsigned short, short, short, ODBMDL * ) ;

/* read diagnosis data */
 FWLIBAPI short WINAPI cnc_diagnoss( unsigned short, short, short, short, ODBDGN * ) ;

/* read diagnosis data(area specified) */
 FWLIBAPI short WINAPI cnc_diagnosr( unsigned short, short *, short, short *, short *, void * ) ;

/* read A/D conversion data */
 FWLIBAPI short WINAPI cnc_adcnv( unsigned short, short, short, ODBAD * ) ;

/* read operator's message */
 FWLIBAPI short WINAPI cnc_rdopmsg( unsigned short, short, short, OPMSG * ) ;

/* read operator's message */
 FWLIBAPI short WINAPI cnc_rdopmsg2( unsigned short, short, short, OPMSG2 * ) ;

/* read operator's message */
 FWLIBAPI short WINAPI cnc_rdopmsg3( unsigned short, short, short *, OPMSG3 * ) ;

/* set path number(for 4 axes lathes, multi-path) */
 FWLIBAPI short WINAPI cnc_setpath( unsigned short, short ) ;

/* get path number(for 4 axes lathes, multi-path) */
 FWLIBAPI short WINAPI cnc_getpath( unsigned short, short *, short * ) ;

/* allocate library handle */
 FWLIBAPI short WINAPI cnc_allclibhndl( unsigned short * ) ;

/* free library handle */
 FWLIBAPI short WINAPI cnc_freelibhndl( unsigned short ) ;

/* get custom macro type */
 FWLIBAPI short WINAPI cnc_getmactype( unsigned short, short * ) ;

/* set custom macro type */
 FWLIBAPI short WINAPI cnc_setmactype( unsigned short, short ) ;

/* get P code macro type */
 FWLIBAPI short WINAPI cnc_getpmactype( unsigned short, short * ) ;

/* set P code macro type */
 FWLIBAPI short WINAPI cnc_setpmactype( unsigned short, short ) ;

/* get screen status */
 FWLIBAPI short WINAPI cnc_getcrntscrn( unsigned short, short * ) ;

/* change screen mode */
 FWLIBAPI short WINAPI cnc_slctscrn( unsigned short, short ) ;

/* read CNC configuration information */
 FWLIBAPI short WINAPI cnc_sysconfig( unsigned short, ODBSYSC * ) ;

/* read program restart information */
 FWLIBAPI short WINAPI cnc_rdprstrinfo( unsigned short, ODBPRS * ) ;

/* search sequence number for program restart */
 FWLIBAPI short WINAPI cnc_rstrseqsrch( unsigned short, long, long, short, short ) ;

/* read output signal image of software operator's panel  */
 FWLIBAPI short WINAPI cnc_rdopnlsgnl( unsigned short, short, IODBSGNL * ) ;

/* write output signal of software operator's panel  */
 FWLIBAPI short WINAPI cnc_wropnlsgnl( unsigned short, IODBSGNL * ) ;

/* read general signal image of software operator's panel  */
 FWLIBAPI short WINAPI cnc_rdopnlgnrl( unsigned short, short, IODBGNRL * ) ;

/* write general signal image of software operator's panel  */
 FWLIBAPI short WINAPI cnc_wropnlgnrl( unsigned short, IODBGNRL * ) ;

/* read general signal name of software operator's panel  */
 FWLIBAPI short WINAPI cnc_rdopnlgsname( unsigned short, short, IODBRDNA * ) ;

/* write general signal name of software operator's panel  */
 FWLIBAPI short WINAPI cnc_wropnlgsname( unsigned short, IODBRDNA * ) ;

/* get detail error */
 FWLIBAPI short WINAPI cnc_getdtailerr( unsigned short, ODBERR * ) ;

/* read informations of CNC parameter */
 FWLIBAPI short WINAPI cnc_rdparainfo( unsigned short, short, unsigned short, ODBPARAIF * ) ;
/* read informations of CNC setting data */
 FWLIBAPI short WINAPI cnc_rdsetinfo( unsigned short, short, unsigned short, ODBSETIF * ) ;

/* read informations of CNC diagnose data */
 FWLIBAPI short WINAPI cnc_rddiaginfo( unsigned short, short, unsigned short, ODBDIAGIF * ) ;

/* read maximum, minimum and total number of CNC parameter */
 FWLIBAPI short WINAPI cnc_rdparanum( unsigned short, ODBPARANUM * ) ;

/* read maximum, minimum and total number of CNC setting data */
 FWLIBAPI short WINAPI cnc_rdsetnum( unsigned short, ODBSETNUM * ) ;

/* read maximum, minimum and total number of CNC diagnose data */
 FWLIBAPI short WINAPI cnc_rddiagnum( unsigned short, ODBDIAGNUM * ) ;

/* get maximum valid figures and number of decimal places */
 FWLIBAPI short WINAPI cnc_getfigure( unsigned short, short, short *, short *, short * ) ;

/* read the loop gain for each axis */
 FWLIBAPI short WINAPI cnc_rdloopgain( unsigned short, long * ) ;

/* read the actual current for each axis */
 FWLIBAPI short WINAPI cnc_rdcurrent( unsigned short, short * ) ;

/* read the actual speed for each axis */
 FWLIBAPI short WINAPI cnc_rdsrvspeed( unsigned short, long * ) ;

/* read the operation mode */
 FWLIBAPI short WINAPI cnc_rdopmode( unsigned short, short * ) ;

/* read the position deviation S */
 FWLIBAPI short WINAPI cnc_rdposerrs( unsigned short, long * ) ;

/* read the position deviation S1 and S2 */
 FWLIBAPI short WINAPI cnc_rdposerrs2( unsigned short, ODBPSER * ) ;

/* read the position deviation Z in the rigid tap mode */
 FWLIBAPI short WINAPI cnc_rdposerrz( unsigned short, long * ) ;

/* read the synchronous error in the synchronous control mode */
 FWLIBAPI short WINAPI cnc_rdsynerrsy( unsigned short, long * ) ;

/* read the synchronous error in the rigid tap mode */
 FWLIBAPI short WINAPI cnc_rdsynerrrg( unsigned short, long * ) ;

/* read the spindle alarm */
 FWLIBAPI short WINAPI cnc_rdspdlalm( unsigned short, char * ) ;

/* read the control input signal */
 FWLIBAPI short WINAPI cnc_rdctrldi( unsigned short, ODBSPDI * ) ;

/* read the control output signal */
 FWLIBAPI short WINAPI cnc_rdctrldo( unsigned short, ODBSPDO * ) ;

/* read the number of controled spindle */
 FWLIBAPI short WINAPI cnc_rdnspdl( unsigned short, short * ) ;

/* read data from FANUC BUS */
 FWLIBAPI short WINAPI cnc_rdfbusmem( unsigned short, short, short, long, long, void * ) ;

/* write data to FANUC BUS */
 FWLIBAPI short WINAPI cnc_wrfbusmem( unsigned short, short, short, long, long, void * ) ;

/* read the parameter of wave diagnosis */
 FWLIBAPI short WINAPI cnc_rdwaveprm( unsigned short, IODBWAVE * ) ;

/* write the parameter of wave diagnosis */
 FWLIBAPI short WINAPI cnc_wrwaveprm( unsigned short, IODBWAVE * ) ;

/* read the parameter of wave diagnosis 2 */
 FWLIBAPI short WINAPI cnc_rdwaveprm2( unsigned short, IODBWVPRM * ) ;

/* write the parameter of wave diagnosis 2 */
 FWLIBAPI short WINAPI cnc_wrwaveprm2( unsigned short, IODBWVPRM * ) ;

/* start the sampling for wave diagnosis */
 FWLIBAPI short WINAPI cnc_wavestart( unsigned short ) ;

/* stop the sampling for wave diagnosis */
 FWLIBAPI short WINAPI cnc_wavestop( unsigned short ) ;

/* read the status of wave diagnosis */
 FWLIBAPI short WINAPI cnc_wavestat( unsigned short, short * ) ;

/* read the data of wave diagnosis */
 FWLIBAPI short WINAPI cnc_rdwavedata( unsigned short, short, short, long, long *, ODBWVDT * ) ;

/* read program directory 2 */
 FWLIBAPI short WINAPI cnc_rdprogdir2( unsigned short, short, short *, short *, PRGDIR2 * ) ;

/* read work coordinate shift */
 FWLIBAPI short WINAPI cnc_rdwkcdshft( unsigned short, short, short, IODBWCSF * ) ;

/* write work coordinate shift */
 FWLIBAPI short WINAPI cnc_wrwkcdshft( unsigned short, short, IODBWCSF * ) ;

/* read work coordinate shift measure */
 FWLIBAPI short WINAPI cnc_rdwkcdsfms( unsigned short, short, short, IODBWCSF * ) ;

/* write work coordinate shift measure */
 FWLIBAPI short WINAPI cnc_wrwkcdsfms( unsigned short, short, IODBWCSF * ) ;

/* stop the sampling for operator message history */
 FWLIBAPI short WINAPI cnc_stopomhis( unsigned short ) ;

/* start the sampling for operator message history */
 FWLIBAPI short WINAPI cnc_startomhis( unsigned short ) ;

/* read operator message history information */
 FWLIBAPI short WINAPI cnc_rdomhisinfo( unsigned short, ODBOMIF * ) ;

/* read operator message history */
 FWLIBAPI short WINAPI cnc_rdomhistry( unsigned short, unsigned short, unsigned short *, ODBOMHIS * ) ;

/* clear operator message history */
 FWLIBAPI short WINAPI cnc_clearomhis( unsigned short ) ;

/* read CNC system soft series and version */
 FWLIBAPI short WINAPI cnc_rdsyssoft( unsigned short, ODBSYSS * ) ;

/* read CNC module configuration information */
 FWLIBAPI short WINAPI cnc_rdmdlconfig( unsigned short, ODBMDLC * ) ;

/* read CNC module configuration information 2 */
 FWLIBAPI short WINAPI cnc_rdmdlconfig2( unsigned short, char * ) ;

/* get date and time from cnc */
 FWLIBAPI short WINAPI cnc_gettimer( unsigned short, IODBTIMER * ) ;

/* set date and time for cnc */
 FWLIBAPI short WINAPI cnc_settimer( unsigned short, IODBTIMER * ) ;

/* converts from FANUC code to Shift JIS code */
FWLIBAPI short WINAPI cnc_ftosjis( unsigned short, char *, char * );

/* Get CNC Model */
FWLIBAPI short WINAPI cnc_getcncmodel( unsigned short, short * );


/*----------------------------*/
/* CNC : Graphic command data */
/*----------------------------*/

/* Start drawing position */
FWLIBAPI short WINAPI cnc_startdrawpos( unsigned short );

/* Stop drawing position */
FWLIBAPI short WINAPI cnc_stopdrawpos( unsigned short );

/* Start dynamic graphic */
FWLIBAPI short WINAPI cnc_startdyngrph( unsigned short );

/* Stop dynamic graphic */
FWLIBAPI short WINAPI cnc_stopdyngrph( unsigned short );

/* Read graphic command data */
FWLIBAPI short WINAPI cnc_rdgrphcmd( unsigned short, short *, short * );

/* Update graphic command read pointer */
FWLIBAPI short WINAPI cnc_wrgrphcmdptr( unsigned short, short );

/* Read cancel flag */
FWLIBAPI short WINAPI cnc_rdgrphcanflg( unsigned short, short * );

/* Clear graphic command */
FWLIBAPI short WINAPI cnc_clrgrphcmd( unsigned short );


/*----------------------------*/
/* CNC : NC display function  */
/*----------------------------*/

/* Start NC display */
FWLIBAPI short WINAPI cnc_startnccmd( unsigned short );

/* Start NC display (2) */
FWLIBAPI short WINAPI cnc_startnccmd2( unsigned short, char * );

/* Stop NC display */
FWLIBAPI short WINAPI cnc_stopnccmd( unsigned short );

/* Get NC display mode */
FWLIBAPI short WINAPI cnc_getdspmode( unsigned short, short * );


/*------------------------------------*/
/* CNC : Remote diagnostics function  */
/*------------------------------------*/

/* Start remote diagnostics function */
FWLIBAPI short WINAPI cnc_startrmtdgn( unsigned short );

/* Stop remote diagnostics function */
FWLIBAPI short WINAPI cnc_stoprmtdgn( unsigned short );

/* Read data from remote diagnostics I/F */
FWLIBAPI short WINAPI cnc_rdrmtdgn( unsigned short, long *, char * );

/* Write data to remote diagnostics I/F */
FWLIBAPI short WINAPI cnc_wrrmtdgn( unsigned short, long *, char * );

/* Set CommStatus of remote diagnostics I/F area */
FWLIBAPI short WINAPI cnc_wrcommstatus( unsigned short, short );

/* Check remote diagnostics I/F */
FWLIBAPI short WINAPI cnc_chkrmtdgn( unsigned short );


/*-----------*/
/* Oxxxxxxxx */
/*-----------*/

/* start uploading NC program */
FWLIBAPI short WINAPI cnc_upstarto8( unsigned short, long ) ;

/* search specified program */
FWLIBAPI short WINAPI cnc_searcho8( unsigned short, long ) ;

/* delete specified program */
FWLIBAPI short WINAPI cnc_deleteo8( unsigned short, long ) ;

/* read program directory */
FWLIBAPI short WINAPI cnc_rdprogdiro8( unsigned short, short, long, long, unsigned short, PRGDIR * ) ;

/* read program number under execution */
FWLIBAPI short WINAPI cnc_rdprgnumo8( unsigned short, ODBPROO8 * ) ;

/* read all dynamic data */
FWLIBAPI short WINAPI cnc_rddynamico8( unsigned short, short, short, ODBDYO8 * ) ;

/* read execution pointer for MDI operation */
FWLIBAPI short WINAPI cnc_rdmdipntro8( unsigned short, ODBMDIPO8 * ) ;

/* read program directory 2 */
FWLIBAPI short WINAPI cnc_rdprogdir2o8( unsigned short, short, long *, short *, PRGDIR2O8 * ) ;


/*-----*/
/* PMC */
/*-----*/

/* read message from PMC to MMC */
 FWLIBAPI short WINAPI pmc_rdmsg( unsigned short, short *, short * ) ;

/* write message from MMC to PMC */
 FWLIBAPI short WINAPI pmc_wrmsg( unsigned short, short, short * ) ;

/* read message from PMC to MMC(conditional) */
 FWLIBAPI short WINAPI pmc_crdmsg( unsigned short, short *, short * ) ;

/* write message from MMC to PMC(conditional) */
 FWLIBAPI short WINAPI pmc_cwrmsg( unsigned short, short, short * ) ;

/* read PMC data(area specified) */
 FWLIBAPI short WINAPI pmc_rdpmcrng( unsigned short, short, short, unsigned short, unsigned short, unsigned short, IODBPMC * ) ;

/* write PMC data(area specified) */
 FWLIBAPI short WINAPI pmc_wrpmcrng( unsigned short, unsigned short, IODBPMC * ) ;

/* read informations of PMC data */
 FWLIBAPI short WINAPI pmc_rdpmcinfo( unsigned short, short, ODBPMCINF * ) ;

/* read PMC parameter data table contorol data */
 FWLIBAPI short WINAPI pmc_rdcntldata( unsigned short, short, short, short, IODBPMCCNTL * ) ;

/* write PMC parameter data table contorol data */
 FWLIBAPI short WINAPI pmc_wrcntldata( unsigned short, short, IODBPMCCNTL * ) ;

/* read PMC parameter data table contorol data group number */
 FWLIBAPI short WINAPI pmc_rdcntlgrp( unsigned short, short * ) ;

/* write PMC parameter data table contorol data group number */
 FWLIBAPI short WINAPI pmc_wrcntlgrp( unsigned short, short ) ;

/* read PMC alarm message */
 FWLIBAPI short WINAPI pmc_rdalmmsg( unsigned short, short, short *, short *, ODBPMCALM * ) ;

/* get detail error for pmc */
 FWLIBAPI short WINAPI pmc_getdtailerr( unsigned short, ODBPMCERR * ) ;

/* read PMC memory data */
 FWLIBAPI short WINAPI pmc_rdpmcmem( unsigned short, short, long, long, void * ) ;

/* write PMC memory data */
 FWLIBAPI short WINAPI pmc_wrpmcmem( unsigned short, short, long, long, void * ) ;

/*--------------------------*/
/* HSSB multiple connection */
/*--------------------------*/

/* read number of node */
 FWLIBAPI short WINAPI cnc_rdnodenum( long * );

/* read node informations */
 FWLIBAPI short WINAPI cnc_rdnodeinfo( long, ODBNODE * );

/* set default node number */
 FWLIBAPI short WINAPI cnc_setdefnode( long );

/* allocate library handle 2 */
 FWLIBAPI short WINAPI cnc_allclibhndl2( long, unsigned short * );


/*---------------------*/
/* Macro for Oxxxxxxxx */
/*---------------------*/

#ifdef ONO8D
#define odbpro          odbproo8
#define odbdy           odbdyo8
#define odbmdip         odbmdipo8
#define prgdir2         prgdir2o8
#define ODBPRO          ODBPROO8
#define ODBDY           ODBDYO8
#define ODBMDIP         ODBMDIPO8
#define PRGDIR2         PRGDIR2O8

#define cnc_upstart     cnc_upstarto8
#define cnc_search      cnc_searcho8
#define cnc_delete      cnc_deleteo8
#define cnc_rdprogdir   cnc_rdprogdiro8
#define cnc_rdprgnum    cnc_rdprgnumo8
#define cnc_rddynamic   cnc_rddynamico8
#define cnc_rdmdipntr   cnc_rdmdipntro8
#define cnc_rdprogdir2  cnc_rdprogdir2o8
#endif


#ifdef __cplusplus
}
#endif

#endif  /* _INC_FWLIB */
