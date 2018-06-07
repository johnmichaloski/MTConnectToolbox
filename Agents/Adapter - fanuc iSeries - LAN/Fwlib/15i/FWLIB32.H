/*-------------------------------------------------------*/
/* fwlib32.h                                             */
/*                                                       */
/* CNC/PMC Data Window Library for series 150i           */
/*                                                       */
/* Copyright (C) 1999 by FANUC LTD. All rights reserved. */
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
#if defined(M_AXIS2)
#define MAX_AXIS        24
#else
#define MAX_AXIS        10
#endif
#define ALL_AXES        -1

#define MAX_SPINDLES    4
#define ALL_SPINDLES    -1

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
#define EW_ALARM        15              /* CNC alarm error */

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
#pragma pack(push, 4)

/*--------------------------------------*/
/* CNC: Library handle and node related */
/*--------------------------------------*/

/* cnc_rdnodeinfo:read node informations */
typedef struct odbnode {
        long node_no;
        long io_base;
        long status;
        long cnc_type;
        char node_name[20];
} ODBNODE;


/*-------------------------------------*/
/* CNC: Control axis / spindle related */
/*-------------------------------------*/

/* cnc_actf:read actual axis feedrate(F) */
/* cnc_acts:read actual spindle speed(S) */
typedef struct odbact {
    short   dummy[2] ;  /* dummy */
    long    data ;      /* actual feed / actual spindle */
} ODBACT ;

/* cnc_acts2:read actual spindle speed(S) */
/* (All or Specified)                     */
typedef struct odbact2 {
    short   datano;     /* spindle number */
    short   type;       /* dummy */
    long    data[4];    /* spindle data */
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
        } oaxis ;   /* In case of 1 axis */
    } pos ;
} ODBDY ;

/* cnc_rddynamic2:read all dynamic data */
typedef struct odbdy2 {
    short   dummy ;
    short   axis ;       /* axis number             */
    long    alarm ;      /* alarm status            */
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
        } oaxis ;   /* In case of 1 axis  */
    } pos ;
} ODBDY2 ;

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

/* cnc_rdposition:read tool position */
typedef struct poselm {
    long    data;       /* position data */
    short   dec;        /* decimal position */
    short   unit;       /* data unit */
    short   disp;       /* display flag */
    char    name;       /* axis name */
    char    suff;       /* suffix */
} POSELM;

typedef struct odbpos {
    POSELM  abs;        /* absolute position */
    POSELM  mach;       /* machine position */
    POSELM  rel;        /* relative position */
    POSELM  dist;       /* distance position */
} ODBPOS ;

/* cnc_rdspeed:read current speed */
typedef struct speedelm {
    long    data;       /* speed data */
    short   dec;        /* decimal position */
    short   unit;       /* data unit */
    short   disp;       /* display flag */
    char    name;       /* name of data */
    char    suff;       /* suffix */
} SPEEDELM ;

typedef struct odbspeed {
    SPEEDELM    actf;   /* actual feed rate */
    SPEEDELM    acts;   /* actual spindle speed */
} ODBSPEED ;

/* cnc_rd3dtooltip:read tip of tool for 3D handle */
/* cnc_rd3dmovrlap:read move overlap of tool for 3D handle */
typedef struct odb3dhdl {
        short   axes[5] ;
        long    data[5] ;
} ODB3DHDL ;

/* cnc_rd3dpulse:read pulse for 3D handle */
typedef struct odb3dpls {
        long    right_angle_x ;
        long    right_angle_y ;
        long    tool_axis     ;
        long    tool_tip_a_b  ;
        long    tool_tip_c    ;
} ODB3DPLS ;


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

/* cnc_rdprogdir2:read program directory 2 */
typedef struct prgdir2 {
        short   number ;
        long    length ;
        char    comment[51] ;
        char    dummy ;
} PRGDIR2;

/* cnc_rdproginfo:read program information */
typedef struct odbnc {
    union {
        struct {
            short   reg_prg ;       /* registered program number */
            short   unreg_prg ;     /* unregistered program number */
            long    used_mem ;      /* used memory area */
            long    unused_mem ;    /* unused memory area */
        } bin ;             /* binary data type */
        char    asc[35] ;   /* ASCII string type */
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

/* cnc_rdproctime:read processing time stamp data */
typedef struct odbptime {
        short   num;
        struct{
                long    prg_no;
                short   hour;
                char    minute;
                char    second;
        }data[10];
} ODBPTIME;

/* cnc_rdprgdirtime:read program directory for processing time data */
typedef struct prgdirtm {
        long    prg_no;
        char    comment[51];
        char    cuttime[13];
} PRGDIRTM;


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

/* cnc_rdtofsinfo:read tool offset information */
typedef struct odbtlinf {
    short   ofs_type;
    short   use_no;
} ODBTLINF;

/* cnc_rdzofs:read work zero offset value */
/* cnc_wrzofs:write work zero offset value */
/* cnc_rdactfixofs:read active fixture offset */
typedef struct iodbzofs {
    short   datano ;            /* offset NO. */
    short   type ;              /* axis number */
    long    data[MAX_AXIS] ;    /* data value */
} IODBZOFS ;

/* cnc_rdzofsr:read work zero offset value(area specified) */
/* cnc_wrzofsr:write work zero offset value(area specified) */
/* cnc_rdfixofs:read fixture offset */
/* cnc_wrfixofs:write fixture offset */
typedef struct iodbzor {
    short   datano_s ;  /* start offset number */
    short   type ;      /* axis number */
    short   datano_e ;  /* end offset number */
    long    data[70] ;  /* offset value */
} IODBZOR ; /* In case that the number of axes is MAX_AXIS(=10), the number of data is 7 */

/* cnc_rdparam:read parameter */
/* cnc_wrparam:write parameter */
/* cnc_rdset:read setting data */
/* cnc_wrset:write setting data */
/* cnc_rdparar:read parameter(area specified) */
/* cnc_wrparas:write parameter(plural specified) */
/* cnc_rdsetr:read setting data(area specified) */
/* cnc_wrsets:write setting data(plural specified) */
typedef struct realprm {
        long   prm_val;     /* data of real parameter */
        long   dec_val;     /* decimal point of real parameter */
} REALPRM;

typedef struct iodbpsd {
    short   datano ;    /* data number */
    short   type ;      /* axis number */
    union {
        char    cdata ;     /* parameter / setting data */
        short   idata ;
        long    ldata ;
                REALPRM rdata ;
        char    cdatas[MAX_AXIS] ;
        short   idatas[MAX_AXIS] ;
        long    ldatas[MAX_AXIS] ;
                REALPRM rdatas[MAX_AXIS] ;
    } u ;
} IODBPSD ;

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

/* cnc_rdparanum:read maximum, minimum and total number of CNC parameter */
typedef struct odbparanum {
    unsigned short  para_min ;
    unsigned short  para_max ;
    unsigned short  total_no ;
} ODBPARANUM ;

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

/* cnc_rdsetnum:read maximum, minimum and total number of CNC setting data */
typedef struct odbsetnum {
    unsigned short  set_min ;
    unsigned short  set_max ;
    unsigned short  total_no ;
} ODBSETNUM ;

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

/* cnc_rdmacro3:read custom macro variable (3)*/
typedef struct odbm3 {
    long    datano ;    /* variable number */
    long    mcr_val ;   /* macro variable */
    short   dec_val ;   /* decimal point */
} ODBM3 ;

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

/* cnc_rdmacroinfo:read custom macro variable information */
typedef struct odbmvinf {
    short   use_no1;
    short   use_no2;
} ODBMVINF;

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

/* cnc_rdmgrpdata:read M-code group data */
typedef struct odbmgrp {
        long  m_code ;
        short grp_no ;
        char  m_name[21] ;
        char  dummy ;
} ODBMGRP;

/* cnc_wrmgrpdata:write M-code group data */
typedef struct idbmgrp {
        short s_no ;
        short dummy ;
        short num ;
        short group[500] ;
} IDBMGRP ;

/* cnc_rdexecmcode:read executing M-code group data */
typedef struct odbexem {
    short   grp_no;
    short   mem_no;
    struct{
    long    no;
    short   flag;
    }m_code[5];
    char    m_name[21];
    char    dummy;
} ODBEXEM;

/* cnc_rdrstrmcode:read program re-start M-code group data */
typedef struct odbrstrm {
    short   grp_no;
    short   mem_no;
    struct{
    long    no;
    short   flag;
    }m_code[5];
} ODBRSTRM;

/* cnc_rdintchk:read interrupt check */
/* cnc_wrintchk:write interrupt check */
typedef struct  iodbint {
        short   datano_s;       /* start offset No. */
        short   type;           /* kind of position */
        short   datano_e;       /* end offset No. */
        long    data[24];       /* position value of area for not attach */
} IODBINT ;

/* cnc_rdhpccset:read hpcc setting data */
/* cnc_wrhpccset:write hpcc setting data */
typedef struct iodbhpst {
    short   slct ;
    short   hpcc ;
    short   multi ;
    short   ovr1 ;
    short   ign_f ;
    short   foward ;
    long    max_f ;
    short   ovr2 ;
    short   ovr3 ;
    short   ovr4 ;
    long    reserve[7] ;
} IODBHPST ;

/* cnc_rdhpcctupr:read hpcc tuning data ( parameter input ) */
/* cnc_wrhpcctupr:write hpcc tuning data ( parameter input ) */
typedef struct iodbhppr {
    struct {
    short   slct ;
    short   diff ;
    short   fine ;
    short   acc_lv ;
    long    max_f ;
    short   bipl ;
    short   aipl ;
    long    corner ;
    short   clamp ;
    long    radius ;
    long    max_cf ;
    long    min_cf ;
    long    foward ;
    long    reserve[5] ;
    }tune[3] ;
} IODBHPPR ;

/* cnc_rdhpcctuac:read hpcc tuning data ( acc input ) */
/* cnc_wrhpcctuac:write hpcc tuning data ( acc input ) */
typedef struct iodbhpac {
    struct {
    short   slct ;
    short   diff ;
    short   fine ;
    short   acc_lv ;
    long    bipl ;
    short   aipl ;
    long    corner ;
    long    clamp ;
    long    c_acc ;
    long    foward ;
    long    reserve[8] ;
    }tune[3] ;
} IODBHPAC ;

/* cnc_rdwkcdshft:read work coordinate shift */
/* cnc_wrwkcdshft:write work coordinate shift */
/* cnc_rdwkcdsfms:read work coordinate shift measure */
/* cnc_wrwkcdsfms:write work coordinate shift measure */
typedef struct iodbwcsf {
     short      datano;         /* datano                       */
     short      type;           /* axis number                  */
     long       data[MAX_AXIS]; /* data                         */
} IODBWCSF;

/* cnc_rdbtofsr:read b-axis tool offset value(area specified) */
/* cnc_wrbtofsr:write b-axis tool offset value(area specified) */
typedef struct  iodbbto {
        short datano_s;         /* start offset number */
        short type;             /* offset type */
        short datano_e;         /* end offset number */
        long  ofs[18];          /* offset */
} IODBBTO ;     /* In case that the number of data is 9 (B type) */

/* cnc_rdbtofsinfo:read b-axis tool offset information */
typedef struct  odbbtlinf {
        short   ofs_type;       /* memory type */
        short   use_no;         /* sum of b-axis offset */
        short   sub_no;         /* sub function number of offset cancel */
} ODBBTLINF ;

/* cnc_rdbaxis:read b-axis command */
typedef struct odbbaxis {
        short           flag ;        /* b-axis command exist or not */
        short           command ;     /* b-axis command */
        unsigned short  speed ;       /* b-axis speed */
        long            sub_data ;    /* b-axis sub data */
} ODBBAXIS ;


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

/* cnc_rdgrpinfo2:read tool life management data(tool group information 2) */
/* cnc_wrgrpinfo2:write tool life management data(tool group information 2) */
typedef struct iodbtgi2 {
    short   s_grp;              /* start group number */
    short   dummy;              /* dummy */
    short   e_grp;              /* end group number */
    long    opt_grpno[5];       /* optional group number of tool */
} IODBTGI2;  /* In case that the number of group is 5 */

/* cnc_rdgrpinfo3:read tool life management data(tool group information 3) */
/* cnc_wrgrpinfo3:write tool life management data(tool group information 3) */
typedef struct iodbtgi3 {
    short   s_grp;              /* start group number */
    short   dummy;              /* dummy */
    short   e_grp;              /* end group number */
    long    life_rest[5];       /* tool life rest count */
} IODBTGI3;  /* In case that the number of group is 5 */

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

/* cnc_rdophistry2:read operation history data */
typedef struct odbophis {
    short   rec_len;    /* length */
    short   rec_type;   /* record type */
    union {
        struct {
            char    key_code;   /* key code */
            char    pw_flag;    /* power on flag */
            short   dummy;
        } rec_mdi;
        struct {
            short   sig_name;   /* signal name */
            short   sig_no;     /* signal number */
            char    sig_old;    /* old signal bit pattern */
            char    sig_new;    /* new signal bit pattern */
            short   dummy;
        } rec_sgn;
        struct {
            short   alm_grp;    /* alarm group */
            short   alm_no;     /* alarm number */
            short   axis_no;    /* axis number */
            short   year;       /* year */
            short   month;      /* month */
            short   day;        /* day */
            short   hour;       /* hour */
            short   minute;     /* minute */
            short   second;     /* second */
            short   dummy;
        } rec_alm;
        struct {
            short   evnt_type;  /* event type */
            short   year;       /* year */
            short   month;      /* month */
            short   day;        /* day */
            short   hour;       /* hour */
            short   minute;     /* minute */
            short   second;     /* second */
            short   dummy;
        } rec_date;
    } u;
} ODBOPHIS;

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

/* cnc_rdalmhistry2:read alarm history data */
typedef struct odbahis2 {
    unsigned short  s_no;   /* start number */
    unsigned short  e_no;   /* end number */
    struct {
        short   alm_grp;        /* alarm group */
        short   alm_no;         /* alarm number */
        short   axis_no;        /* axis number */
        short   year;           /* year */
        short   month;          /* month */
        short   day;            /* day */
        short   hour;           /* hour */
        short   minute;         /* minute */
        short   second;         /* second */
        short   len_msg;        /* alarm message length */
        char    alm_msg[32];    /* alarm message */
    } alm_his[10];
} ODBAHIS2;  /* In case that the number of data is 10 */

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


/*----------------------------*/
/* CNC: Servo/Spindle related */
/*----------------------------*/

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
    short   data[5001] ;
} ODBWVDT ;


/*----------------------------------------------------*/
/* CNC: Data Server, DNC1, DNC2, OSI-Ethernet related */
/*----------------------------------------------------*/

/* cnc_dtsvrdpgdir:read file directory in Data Server */
typedef struct odbdsdir {
    long    file_num ;
    long    remainder ;
    short   data_num ;
    struct {
        char    file_name[16] ;
        char    comment[64] ;
        long    size ;
        char    date[16] ;
    } data[32] ;
} ODBDSDIR ;

/* cnc_dtsvrdset:read setting data for Data Server */
/* cnc_dtsvwrset:write setting data for Data Server */
typedef struct iodbdsset {
        char host_ip[16] ;
        char host_uname[32] ;
        char host_passwd[32] ;
        char host_dir[128] ;
        char dtsv_mac[13] ;
        char dtsv_ip[16] ;
        char dtsv_mask[16] ;
} IODBDSSET ;

/* cnc_dtsvmntinfo:read maintenance information for Data Server */
typedef struct odbdsmnt {
        long empty_cnt ;
        long total_size ;
        long read_ptr ;
        long write_ptr ;
} ODBDSMNT ;

/* cnc_rdcomparam:read communication parameter for DNC1, DNC2, OSI-Ethernet */
/* cnc_wrcomparam:write communication parameter for DNC1, DNC2, OSI-Ethernet */
typedef struct iodbcprm {
        char           NcApli[65] ;
        char           Dummy1 ;
        char           HostApli[65] ;
        char           Dummy2 ;
        unsigned long  StatPstv ;
        unsigned long  StatNgtv ;
        unsigned long  Statmask ;
        unsigned long  AlarmStat ;
        unsigned long  PsclHaddr ;
        unsigned long  PsclLaddr ;
        unsigned short SvcMode1 ;
        unsigned short SvcMode2 ;
        long           FileTout ;
        long           RemTout ;
} IODBCPRM ;


/*--------------------------*/
/* CNC: PUNCH PRESS related */
/*--------------------------*/

/* cnc_rdtlctldata: read tool controll data */
/* cnc_wrtlctldata: write tool controll data */
typedef struct iodbtlctl {
        short   slct;
        short   used_tool;
        short   turret_indx;
        long    zero_tl_no;
        long    t_axis_move;
        long    total_punch[2];
        short   reserve[11];
} IODBTLCTL ;

/* cnc_rdtooldata: read tool data */
/* cnc_wrtooldata: read tool data */
typedef struct iodbtldt {
        short   slct;
        long    tool_no;
        long    x_axis_ofs;
        long    y_axis_ofs;
        long    turret_pos;
        long    chg_tl_no;
        long    punch_count;
        long    tool_life;
        long    m_tl_radius;
        long    m_tl_angle;
        char    tl_shape;
        long    tl_size_i;
        long    tl_size_j;
        long    tl_angle;
        long    reserve[3];
} IODBTLDT ;

/* cnc_rdmultitldt: read multi tool data */
/* cnc_wrmultitldt: write multi tool data */
typedef struct iodbmlttl {
        short   slct;
        short   m_tl_no;
        long    m_tl_radius;
        long    m_tl_angle;
        long    x_axis_ofs;
        long    y_axis_ofs;
        char    tl_shape;
        long    tl_size_i;
        long    tl_size_j;
        long    tl_angle;
        long    reserve[7];
} IODBMLTTL ;

/* cnc_rdsafetyzone: read safetyzone data */
/* cnc_wrsafetyzone: write safetyzone data */
typedef struct iodbsafe {
        short   slct;
        long    data[3];
} IODBSAFE ;

/* cnc_rdtoolzone: read toolzone data */
/* cnc_wrtoolzone: write toolzone data */
typedef struct iodbtlzn {
        short   slct;
        long    data[2];
} IODBTLZN ;

/* cnc_rdacttlzone: read active toolzone data */
typedef struct odbacttlzn {
        short   act_no;
        long    data[2];
} ODBACTTLZN ;

/* cnc_rdaxisname: read axis name */
typedef struct odbaxisname {
    char name;          /* axis name */
    char suff;          /* suffix */
} ODBAXISNAME ;

/* cnc_rdspdlname: read spindle name */
typedef struct odbspdlname {
    char name;          /* spindle name */
    char suff1;         /* suffix */
    char suff2;         /* suffix */
    char suff3;         /* suffix */
} ODBSPDLNAME ;


/*--------------------*/
/* CNC: LASER related */
/*--------------------*/

/* cnc_rdpscdproc:read processing condition file (processing data) */
/* cnc_wrpscdproc:write processing condition file (processing data) */
typedef struct iodbpscd {
        short   slct;
        long    feed;
        short   power;
        short   freq;
        short   duty;
        short   g_press;
        short   g_kind;
        short   g_ready_t;
        short   displace;
        long    supple;
        short   edge_slt;
        short   appr_slt;
        short   reserve[5];
} IODBPSCD ;

/* cnc_rdpscdpirc:read processing condition file (piercing data) */
/* cnc_wrpscdpirc:write processing condition file (piercing data) */
typedef struct iodbpirc {
        short   slct;
        short   power;
        short   freq;
        short   duty;
        short   i_freq;
        short   i_duty;
        short   step_t;
        short   step_sum;
        long    pier_t;
        short   g_press;
        short   g_kind;
        short   g_time;
        short   def_pos;
        short   reserve[4];
} IODBPIRC ;

/* cnc_rdpscdedge:read processing condition file (edging data) */
/* cnc_wrpscdedge:write processing condition file (edging data) */
typedef struct iodbedge {
        short   slct;
        short   angle;
        short   power;
        short   freq;
        short   duty;
        long    pier_t;
        short   g_press;
        short   g_kind;
        long    r_len;
        short   r_feed;
        short   r_freq;
        short   r_duty;
        short   reserve[5];
} IODBEDGE ;

/* cnc_rdpscdslop:read processing condition file (slope data) */
/* cnc_wrpscdslop:write processing condition file (slope data) */
typedef struct iodbslop {
        long    slct;
        long    upleng;
        short   upsp[10];
        long    dwleng;
        short   dwsp[10];
        short   reserve[10];
} IODBSLOP ;

/* cnc_rdlpwrdty:read power controll duty data */
/* cnc_wrlpwrdty:write power controll duty data */
typedef struct iodblpwdt {
        short   slct;
        short   dty_const;
        short   dty_min;
        short   reserve[6];
} IODBLPWDT ;

/* cnc_rdlpwrdat:read laser power data */
typedef struct odblopdt {
        short   slct;
        short   pwr_mon;
        short   pwr_ofs;
        short   pwr_act;
        long    feed_act;
        short   reserve[4];
} ODBLOPDT ;

/* cnc_rdlagslt:read laser assist gas selection */
/* cnc_wrlagslt:write laser assist gas selection */
typedef struct iodblagsl {
        short   slct;
        short   ag_slt;
        short   agflow_slt;
        short   reserve[6];
} IODBLAGSL ;

/* cnc_rdlagst:read laser assist gas flow */
/* cnc_wrlagst:write laser assist gas flow */
typedef struct iodblagst {
        struct {
                short   slct;
                short   pre_time;
                short   pre_press;
                short   proc_press;
                short   end_time;
                short   end_press;
                short   reserve[3];
        } gasflow[3] ;
} IODBLAGST ;

/* cnc_rdledgprc:read laser power for edge processing */
/* cnc_wrledgprc:write laser power for edge processing */
typedef struct iodblegpr {
        short   slct;
        short   power;
        short   freq;
        short   duty;
        short   reserve[5];
} IODBLEGPR ;

/* cnc_rdlprcprc:read laser power for piercing */
/* cnc_wrlprcprc:write laser power for piercing */
typedef struct iodblpcpr {
        short   slct;
        short   power;
        short   freq;
        short   duty;
        long    time;
        short   reserve[4];
} IODBLPCPR ;

/* cnc_rdlcmddat:read laser command data */
typedef struct iodblcmdt {
        short   slct;
        long    feed;
        short   power;
        short   freq;
        short   duty;
        short   g_kind;
        short   g_ready_t;
        short   g_press;
        short   error;
        long    dsplc;
        short   reserve[7];
} ODBLCMDT ;

/* cnc_rdlactnum:read active number */
typedef struct odblactn {
        short   slct;
        short   act_proc;
        short   act_pirce;
        short   act_slop;
        short   reserve[5];
} ODBLACTN ;

/* cnc_rdlcmmt:read laser comment */
typedef struct odblcmmt {
        char    comment[25];
} ODBLCMMT ;


/*----------------------------------*/
/* CNC: Servo learning data related */
/*----------------------------------*/


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

/* cnc_rdsyssoft2:read CNC system soft series and version */
typedef struct odbsyss2 {
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
     char    embEthe_ser[5];
     char    embEthe_ver[5];
     char    reserve2[38][5];
} ODBSYSS2 ;

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

/* cnc_statinfo:read CNC status information */
typedef struct odbst {
    short dummy[2];     /* dummy                    */
    short aut;          /* selected automatic mode  */
    short manual;       /* selected manual mode     */
    short run;          /* running status           */
    short edit;         /* editting status          */
    short motion;       /* axis, dwell status       */
    short mstb;         /* m, s, t, b status        */
    short emergency;    /* emergency stop status    */
    short write;        /* writting status          */
    short labelskip;    /* label skip status        */
    short alarm;        /* alarm status             */
    short warning;      /* warning status           */
    short battery;      /* battery status           */
} ODBST ;

/* cnc_alarm:read alarm status */
typedef struct odbalm {
    short   dummy[2] ;  /* dummy */
    short   data ;      /* alarm status */
} ODBALM ;

/* cnc_rdalminfo:read alarm information */
typedef struct alminfo {
#ifdef M_AXIS2
    union {
        struct {
            struct {
                long  axis ;
                short alm_no ;
            } alm[5] ;
            long data_end ;
        } alm1 ;
        struct {
            struct {
                long  axis ;
                short alm_no ;
                short msg_len ;
                char alm_msg[32] ;
            } alm[5] ;
            long data_end ;
        }alm2 ;
    } u ;
#else
    union {
        struct {
            struct {
                short axis ;
                short alm_no ;
            } alm[5] ;
            short data_end ;
        } alm1 ;
        struct {
            struct {
                short axis ;
                short alm_no ;
                short msg_len ;
                char alm_msg[32] ;
            } alm[5] ;
            short data_end ;
        }alm2 ;
    } u ;
#endif
} ALMINFO ;               /*  In case that the number of alarm is 5 */

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

/* cnc_rdgcode: read G code */
typedef struct odbgcd {
        short   group ;
        short   flag ;
        char    code[8] ;
} ODBGCD;

/* cnc_rdcommand: read command value */
typedef struct odbcmd {
        char    adrs ;
        char    num ;
        short   flag ;
        long    cmd_val ;
        long    dec_val ;
} ODBCMD;

/* cnc_diagnoss:read diagnosis data */
/* cnc_diagnosr:read diagnosis data(area specified) */
typedef struct realdgn {
        long   dgn_val;     /* data of real diagnoss */
        long   dec_val;     /* decimal point of real diagnoss */
} REALDGN ;

typedef struct odbdgn {
    short   datano ;    /* data number */
    short   type ;      /* axis number */
    union {
        char    cdata ; /* diagnosis data */
        short   idata ;
        long    ldata ;
                REALDGN rdata ;
        char    cdatas[MAX_AXIS] ;
        short   idatas[MAX_AXIS] ;
        long    ldatas[MAX_AXIS] ;
                REALDGN rdatas[MAX_AXIS] ;
    } u ;
} ODBDGN ;

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

/* cnc_rddiagnum:read maximum, minimum and total number of CNC diagnose data */
typedef struct odbdiagnum {
    unsigned short  diag_min ;
    unsigned short  diag_max ;
    unsigned short  total_no ;
} ODBDIAGNUM ;

/* cnc_adcnv:read A/D conversion data */
typedef struct odbad {
    short   datano ;    /* input analog voltage type */
    short   type ;      /* analog voltage type */
    short   data ;      /* digital voltage data */
} ODBAD ;

/* cnc_rdopmsg:read operator's message */
typedef struct msg {
    short datano ;     /* operator's message number */
    short type ;       /* operator's message type   */
    short char_num ;   /* message string length   */
    char  data[129] ;  /* operator's message string */
} OPMSG ;               /* In case that the data length is 129 */

/* cnc_rdopmsg3:read operator's message */
typedef struct opmsg3 {
    short datano ;     /* operator's message number */
    short type ;       /* operator's message type   */
    short char_num ;   /* message string length   */
    char  data[256] ;  /* operator's message string */
} OPMSG3 ;              /* In case that the data length is 256 */

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

/* cnc_rdbrstrinfo:read block re-start information */
typedef struct odbbrs {
    long dest[MAX_AXIS];
    long dist[MAX_AXIS];
} ODBBRS ;              /*  In case that the number of axes is 10 */

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
    short   spdl_ovrd;  /* spindle override signal */
    short   blck_del;   /* optional block skip signal */
    short   sngl_blck;  /* single block signal */
    short   machn_lock; /* machine lock signal */
    short   dry_run;    /* dry run signal */
    short   mem_prtct;  /* memory protection signal */
    short   feed_hold;  /* automatic operation halt signal */
    short   manual_rpd; /* (not used) */
    short   dummy[2];   /* (not used) */
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

/* cnc_rdradofs:read tool radius offset for position data */
typedef struct odbrofs {
    short mode;
    short pln_axes[2];
    long ofsvct[2];
} ODBROFS ;

/* cnc_rdlenofs:read tool length offset for position data */
typedef struct odblofs {
    short mode;
    long ofsvct[MAX_AXIS];
} ODBLOFS ;             /*  In case that the number of axes is 10 */

/* cnc_rdfixcycle:read fixed cycle for position data */
typedef struct odbfix {
    short mode;
    short pln_axes[2];
    short drl_axes;
    long i_pos;
    long r_pos;
    long z_pos;
    long cmd_cnt;
    long act_cnt;
    long cut;
    long shift[2];
} ODBFIX ;

/* cnc_rdcdrotate:read coordinate rotate for position data */
typedef struct odbrot {
    short mode;
    short pln_axes[2];
    long center[2];
    long angle;
} ODBROT ;

/* cnc_rd3dcdcnv:read 3D coordinate convert for position data */
typedef struct odb3dcd {
    short mode;
    short dno;
    short cd_axes[3];
    long center[2][3];
    long direct[2][3];
    long angle[2];
} ODB3DCD ;

/* cnc_rdmirimage:read programable mirror image for position data */
typedef struct odbmir {
    short mode;
    long mir_flag;
    long mir_pos[MAX_AXIS];
} ODBMIR ;              /*  In case that the number of axes is 10 */

/* cnc_rdscaling:read scaling data for position data */
typedef struct odbscl {
    short mode;
    long center[MAX_AXIS];
    long magnif[MAX_AXIS];
} ODBSCL ;              /*  In case that the number of axes is 10 */

/* cnc_rd3dtofs:read 3D tool offset for position data */
typedef struct odb3dto {
    short mode;
    short ofs_axes[3];
    long ofsvct[3];
} ODB3DTO ;

/* cnc_rdposofs:read tool position offset for position data */
typedef struct odbpofs {
    short mode;
    long ofsvct[MAX_AXIS];
} ODBPOFS ;             /*  In case that the number of axes is 10 */

/* cnc_getdtailerr:get detail error */
typedef struct odberr {
    short   err_no ;
    short   err_dtno ;
} ODBERR ;

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

/* cnc_rdtimer:read timer data from cnc */
/* cnc_wrtimer:write timer data for cnc */
typedef struct iodbtime {
        long    minute ;
        long    msec ;
} IODBTIME ;


/*------------------*/
/* CNC : SERCOS I/F */
/*------------------*/

/* cnc_srcsrdidinfo:Read ID information of SERCOS I/F */
/* cnc_srcswridinfo:Write ID information of SERCOS I/F */
typedef struct iodbidinf {
        long  id_no;
        short drv_no;
        short acc_element;
        short err_general;
        short err_id_no;
        short err_id_name;
        short err_attr;
        short err_unit;
        short err_min_val;
        short err_max_val;
        short id_name_len;
        short id_name_max;
        char  id_name[60];
        long  attr;
        short unit_len;
        short unit_max;
        char  unit[12];
        long  min_val;
        long  max_val;
} IODBIDINF;

/* cnc_srcsrdexstat:Get execution status of reading/writing operation data of SERCOS I/F */
typedef struct odbsrcsst {
        short acc_element;
        short err_general;
        short err_id_no;
        short err_attr;
        short err_op_data;
} ODBSRCSST;

/* cnc_srcsrdlayout:Read drive assign of SERCOS I/F */
typedef struct odbsrcslyt {
        short spndl[4];
        short servo[8];
        char  axis_name[8];
} ODBSRCSLYT;


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
/* PROFIBUS function        */
/*--------------------------*/

/* pmc_prfrdconfig:read PROFIBUS configration data */
typedef struct odbprfcnf {
    char    master_ser[5] ;
    char    master_ver[3] ;
    char    slave_ser[5] ;
    char    slave_ver[3] ;
    char    cntl_ser[5] ;
    char    cntl_ver[3] ;
} ODBPRFCNF ;

/* pmc_prfrdbusprm:read bus parameter for master function */
/* pmc_prfwrbusprm:write bus parameter for master function */
typedef struct iodbbusprm {
    char           fdl_add ;
    char           baudrate ;
    unsigned short tsl ;
    unsigned short min_tsdr ;
    unsigned short max_tsdr ;
    unsigned char  tqui ;
    unsigned char  tset ;
    long           ttr ;
    char           gap ;
    char           hsa ;
    char           max_retry ;
    unsigned char  bp_flag ;
    unsigned short min_slv_int ;
    unsigned short poll_tout ;
    unsigned short data_cntl ;
    char           reserve1[6] ;
    char           cls2_name[32] ;
    short          user_dlen ;
    char           user_data[62] ;
    char           reserve2[96] ;
} IODBBUSPRM ;

/* pmc_prfrdslvprm:read slave parameter for master function */
/* pmc_prfwrslvprm:write slave parameter for master function */
typedef struct iodbslvprm {
    short          dis_enb ;
    unsigned short ident_no ;
    unsigned char  slv_flag ;
    unsigned char  slv_type ;
    char           reserve1[12] ;
    unsigned char  slv_stat ;
    unsigned char  wd_fact1 ;
    unsigned char  wd_fact2 ;
    unsigned char  min_tsdr ;
    char           reserve2 ;
    unsigned char  grp_ident ;
    short          user_plen ;
    char           user_pdata[32] ;
    short          cnfg_dlen ;
    char           cnfg_data[126] ;
    short          slv_ulen ;
    char           slv_udata[30] ;
    char           reserve3[8] ;
} IODBSLVPRM ;

/* pmc_prfrdallcadr:read allocation address for master function */
/* pmc_prfwrallcadr:set allocation address for master function */
typedef struct iodbprfadr {
    char           di_size ;
    char           di_type ;
    unsigned short di_addr ;
    short          reserve1;
    char           do_size ;
    char           do_type ;
    unsigned short do_addr ;
    short          reserve2;
    char           dgn_size ;
    char           dgn_type ;
    unsigned short dgn_addr ;
} IODBPRFADR ;

/* pmc_prfrdslvaddr:read allocation address for slave function */
/* pmc_prfwrslvaddr:set allocation address for slave function */
typedef struct iodbslvadr {
    char           slave_no ;
    char           di_size ;
    char           di_type ;
    unsigned short di_addr ;
    char           do_size ;
    char           do_type ;
    unsigned short do_addr ;
    char           reserve[7] ;
} IODBSLVADR ;

/* pmc_prfrdslvstat:read status for slave function */
typedef struct odbslvst {
    unsigned char  cnfg_stat ;
    unsigned char  prm_stat ;
    char           wdg_stat ;
    unsigned char  live_stat ;
    short          ident_no ;
} ODBSLVST ;


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
        } oaxis ;   /* In case of 1 axis  */
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


#pragma pack(pop)


/*--------------------------------------*/
/* CNC: Library handle and node related */
/*--------------------------------------*/

/* allocate library handle */
 FWLIBAPI short WINAPI cnc_allclibhndl( unsigned short * ) ;

/* free library handle */
 FWLIBAPI short WINAPI cnc_freelibhndl( unsigned short ) ;

/* allocate library handle 2 */
 FWLIBAPI short WINAPI cnc_allclibhndl2( long, unsigned short * );

/* read number of node */
 FWLIBAPI short WINAPI cnc_rdnodenum( long * );

/* read node informations */
 FWLIBAPI short WINAPI cnc_rdnodeinfo( long, ODBNODE * );

/* set default node number */
 FWLIBAPI short WINAPI cnc_setdefnode( long );


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

/* read all dynamic data */
 FWLIBAPI short WINAPI cnc_rddynamic2( unsigned short, short, short, ODBDY2 * ) ;

/* set origin / preset relative axis position */
 FWLIBAPI short WINAPI cnc_wrrelpos( unsigned short, short, IDBWRR * ) ;

/* preset work coordinate */
 FWLIBAPI short WINAPI cnc_prstwkcd( unsigned short, short, IDBWRA * ) ;

/* read manual overlapped motion value */
 FWLIBAPI short WINAPI cnc_rdmovrlap( unsigned short, short, short, IODBOVL * ) ;

/* cancel manual overlapped motion value */
 FWLIBAPI short WINAPI cnc_canmovrlap( unsigned short, short ) ;

/* read actual spindle speed(S) */
 FWLIBAPI short WINAPI cnc_acts( unsigned short, ODBACT * ) ;

/* read actual spindle speed(S) (All or spesified) */
 FWLIBAPI short WINAPI cnc_acts2( unsigned short, short, ODBACT2 * ) ;

/* read load information of serial spindle */
 FWLIBAPI short WINAPI cnc_rdspload( unsigned short, short, ODBSPN * ) ;

/* read maximum r.p.m. ratio of serial spindle */
 FWLIBAPI short WINAPI cnc_rdspmaxrpm( unsigned short, short, ODBSPN * ) ;

/* read gear ratio of serial spindle */
 FWLIBAPI short WINAPI cnc_rdspgear( unsigned short, short, ODBSPN * ) ;

/* read tip of tool for 3D hundle */
 FWLIBAPI short WINAPI cnc_rd3dtooltip( unsigned short, ODB3DHDL * ) ;

/* read pulse for 3D hundle */
 FWLIBAPI short WINAPI cnc_rd3dpulse( unsigned short, ODB3DPLS * ) ;

/* read move overrlap of tool for 3D hundle */
 FWLIBAPI short WINAPI cnc_rd3dmovrlap( unsigned short, ODB3DHDL * ) ;

/* read change offset for 3D hundle */
 FWLIBAPI short WINAPI cnc_rd3dofschg( unsigned short, long * ) ;

/* clear pulse and change offset for 3D hundle */
 FWLIBAPI short WINAPI cnc_clr3dplsmov( unsigned short, short ) ;

/* read absolute axis position 2 */
 FWLIBAPI short WINAPI cnc_absolute2( unsigned short, short, short, ODBAXIS * ) ;

/* read relative axis position 2 */
 FWLIBAPI short WINAPI cnc_relative2( unsigned short, short, short, ODBAXIS * ) ;

/* read tool position */
FWLIBAPI short WINAPI cnc_rdposition( unsigned short, short, short *, ODBPOS * );

/* read current speed */
FWLIBAPI short WINAPI cnc_rdspeed( unsigned short, short, ODBSPEED * );


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

/* read program directory 2 */
 FWLIBAPI short WINAPI cnc_rdprogdir2( unsigned short, short, short *, short *, PRGDIR2 * ) ;

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

/* read processing time stamp data */
 FWLIBAPI short WINAPI cnc_rdproctime( unsigned short, ODBPTIME * ) ;

/* read program directory for processing time data */
 FWLIBAPI short WINAPI cnc_rdprgdirtime( unsigned short, long *, short *, PRGDIRTM * ) ;

/* start downloading NC data */
 FWLIBAPI short WINAPI cnc_dwnstart3( unsigned short, short ) ;

/* download NC data */
 FWLIBAPI short WINAPI cnc_download3( unsigned short, long *, char * ) ;

/* end of downloading NC data */
 FWLIBAPI short WINAPI cnc_dwnend3( unsigned short ) ;

/* start uploading NC data */
 FWLIBAPI short WINAPI cnc_upstart3( unsigned short, short, long, long ) ;

/* upload NC data */
 FWLIBAPI short WINAPI cnc_upload3(unsigned short, long *, char * ) ;

/* end of uploading NC data */
 FWLIBAPI short WINAPI cnc_upend3( unsigned short ) ;


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

/* read tool offset information */
 FWLIBAPI short WINAPI cnc_rdtofsinfo( unsigned short, ODBTLINF * ) ;

/* read work zero offset value */
 FWLIBAPI short WINAPI cnc_rdzofs( unsigned short, short, short, short, IODBZOFS * ) ;

/* write work zero offset value */
 FWLIBAPI short WINAPI cnc_wrzofs( unsigned short, short, IODBZOFS * ) ;

/* read work zero offset value(area specified) */
 FWLIBAPI short WINAPI cnc_rdzofsr( unsigned short, short, short, short, short, IODBZOR * ) ;

/* write work zero offset value(area specified) */
 FWLIBAPI short WINAPI cnc_wrzofsr( unsigned short, short, IODBZOR * ) ;

/* read work zero offset information */
 FWLIBAPI short WINAPI cnc_rdzofsinfo( unsigned short, short * ) ;

/* read parameter */
 FWLIBAPI short WINAPI cnc_rdparam( unsigned short, short, short, short, IODBPSD * ) ;

/* write parameter */
 FWLIBAPI short WINAPI cnc_wrparam( unsigned short, short, IODBPSD * ) ;

/* read parameter(area specified) */
 FWLIBAPI short WINAPI cnc_rdparar( unsigned short, short *, short, short *, short *, void * ) ;

/* write parameter(area specified) */
 FWLIBAPI short WINAPI cnc_wrparas( unsigned short, short, void * ) ;

/* read informations of CNC parameter */
 FWLIBAPI short WINAPI cnc_rdparainfo( unsigned short, short, unsigned short, ODBPARAIF * ) ;

/* read maximum, minimum and total number of CNC parameter */
 FWLIBAPI short WINAPI cnc_rdparanum( unsigned short, ODBPARANUM * ) ;

/* read setting data */
 FWLIBAPI short WINAPI cnc_rdset( unsigned short, short, short, short, IODBPSD * ) ;

/* write setting data */
 FWLIBAPI short WINAPI cnc_wrset( unsigned short, short, IODBPSD * ) ;

/* read setting data(area specified) */
 FWLIBAPI short WINAPI cnc_rdsetr( unsigned short, short *, short, short *, short *, void * ) ;

/* write setting data(area specified) */
 FWLIBAPI short WINAPI cnc_wrsets( unsigned short, short, void * ) ;

/* read informations of CNC setting data */
 FWLIBAPI short WINAPI cnc_rdsetinfo( unsigned short, short, unsigned short, ODBSETIF * ) ;

/* read maximum, minimum and total number of CNC setting data */
 FWLIBAPI short WINAPI cnc_rdsetnum( unsigned short, ODBSETNUM * ) ;

/* read pitch error compensation data(area specified) */
 FWLIBAPI short WINAPI cnc_rdpitchr( unsigned short, short, short, short, IODBPI * ) ;

/* write pitch error compensation data(area specified) */
 FWLIBAPI short WINAPI cnc_wrpitchr( unsigned short, short, IODBPI * ) ;

/* read pitch error compensation data information */
 FWLIBAPI short WINAPI cnc_rdpitchinfo( unsigned short, short * ) ;

/* read custom macro variable */
 FWLIBAPI short WINAPI cnc_rdmacro( unsigned short, short, short, ODBM * ) ;

/* write custom macro variable */
 FWLIBAPI short WINAPI cnc_wrmacro( unsigned short, short, short, long, short ) ;

/* read custom macro variable (3)*/
 FWLIBAPI short WINAPI cnc_rdmacro3( unsigned short, long, short, ODBM3 * ) ;

/* write custom macro variable (3)*/
 FWLIBAPI short WINAPI cnc_wrmacro3( unsigned short, long, short, long, short ) ;

/* read custom macro variables(area specified) */
 FWLIBAPI short WINAPI cnc_rdmacror( unsigned short, short, short, short, IODBMR * ) ;

/* write custom macro variables(area specified) */
 FWLIBAPI short WINAPI cnc_wrmacror( unsigned short, short, IODBMR * ) ;

/* read custom macro variable information */
 FWLIBAPI short WINAPI cnc_rdmacroinfo( unsigned short, ODBMVINF * ) ;

/* get custom macro type */
 FWLIBAPI short WINAPI cnc_getmactype( unsigned short, short * ) ;

/* set custom macro type */
 FWLIBAPI short WINAPI cnc_setmactype( unsigned short, short ) ;

/* read P code macro variable */
 FWLIBAPI short WINAPI cnc_rdpmacro( unsigned short, long, ODBPM * ) ;

/* write P code macro variable */
 FWLIBAPI short WINAPI cnc_wrpmacro( unsigned short, long, long, short ) ;

/* read P code macro variables(area specified) */
 FWLIBAPI short WINAPI cnc_rdpmacror( unsigned short, long, long, unsigned short, IODBPR * ) ;

/* write P code macro variables(area specified) */
 FWLIBAPI short WINAPI cnc_wrpmacror( unsigned short, unsigned short, IODBPR * ) ;

/* read P code macro variable information */
 FWLIBAPI short WINAPI cnc_rdpmacroinfo( unsigned short, ODBPMINF * ) ;

/* get P code macro type */
 FWLIBAPI short WINAPI cnc_getpmactype( unsigned short, short * ) ;

/* set P code macro type */
 FWLIBAPI short WINAPI cnc_setpmactype( unsigned short, short ) ;

/* read M-code group data */
 FWLIBAPI short WINAPI cnc_rdmgrpdata( unsigned short, short, short *, ODBMGRP * ) ;

/* write M-code group data */
 FWLIBAPI short WINAPI cnc_wrmgrpdata( unsigned short, IDBMGRP * ) ;

/* read executing M-code group data */
 FWLIBAPI short WINAPI cnc_rdexecmcode( unsigned short, short, short *, ODBEXEM * );

/* read program re-start M-code group data */
 FWLIBAPI short WINAPI cnc_rdrstrmcode( unsigned short, short, short *, ODBRSTRM * );
/* read interrupt check */
 FWLIBAPI short WINAPI cnc_rdintchk( unsigned short, short, short, short, short, IODBINT * ) ;

/* write interrupt check */
 FWLIBAPI short WINAPI cnc_wrintchk( unsigned short, short, IODBINT * ) ;

/* read interrupt check information */
 FWLIBAPI short WINAPI cnc_rdintinfo( unsigned short, short * ) ;

/* read hpcc setting data */
 FWLIBAPI short WINAPI cnc_rdhpccset( unsigned short, IODBHPST * ) ;

/* write hpcc setting data */
 FWLIBAPI short WINAPI cnc_wrhpccset( unsigned short, IODBHPST * ) ;

/* hpcc data auto setting data */
 FWLIBAPI short WINAPI cnc_hpccatset( unsigned short ) ;

/* read hpcc tuning data ( parameter input ) */
 FWLIBAPI short WINAPI cnc_rdhpcctupr( unsigned short, IODBHPPR * ) ;

/* write hpcc tuning data ( parameter input ) */
 FWLIBAPI short WINAPI cnc_wrhpcctupr( unsigned short, IODBHPPR * ) ;

/* read hpcc tuning data ( acc input ) */
 FWLIBAPI short WINAPI cnc_rdhpcctuac( unsigned short, IODBHPAC * ) ;

/* write hpcc tuning data ( acc input ) */
 FWLIBAPI short WINAPI cnc_wrhpcctuac( unsigned short, IODBHPAC * ) ;

/* hpcc data auto tuning */
 FWLIBAPI short WINAPI cnc_hpccattune( unsigned short, short, short * ) ;

/* read hpcc fine level */
 FWLIBAPI short WINAPI cnc_hpccactfine( unsigned short, short * ) ;

/* select hpcc fine level */
 FWLIBAPI short WINAPI cnc_hpccselfine( unsigned short, short ) ;

/* read active fixture offset */
 FWLIBAPI short WINAPI cnc_rdactfixofs( unsigned short, short, IODBZOFS * ) ;

/* read fixture offset */
 FWLIBAPI short WINAPI cnc_rdfixofs( unsigned short, short, short, short, short, IODBZOR * ) ;

/* write fixture offset */
 FWLIBAPI short WINAPI cnc_wrfixofs( unsigned short, short, IODBZOR * ) ;

/* read work coordinate shift */
 FWLIBAPI short WINAPI cnc_rdwkcdshft( unsigned short, short, short, IODBWCSF * ) ;

/* write work coordinate shift */
 FWLIBAPI short WINAPI cnc_wrwkcdshft( unsigned short, short, IODBWCSF * ) ;

/* read work coordinate shift measure */
 FWLIBAPI short WINAPI cnc_rdwkcdsfms( unsigned short, short, short, IODBWCSF * ) ;

/* write work coordinate shift measure */
 FWLIBAPI short WINAPI cnc_wrwkcdsfms( unsigned short, short, IODBWCSF * ) ;

/* read b-axis tool offset value(area specified) */
 FWLIBAPI short WINAPI cnc_rdbtofsr( unsigned short, short, short, short, short, IODBBTO * ) ;

/* write b-axis tool offset value(area specified) */
 FWLIBAPI short WINAPI cnc_wrbtofsr( unsigned short, short, IODBBTO * ) ;

/* read b-axis tool offset information */
 FWLIBAPI short WINAPI cnc_rdbtofsinfo( unsigned short, ODBBTLINF * ) ;

/* read b-axis command */
 FWLIBAPI short WINAPI cnc_rdbaxis( unsigned short, ODBBAXIS * ) ;


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

/* read tool life management data(tool group information 2) */
 FWLIBAPI short WINAPI cnc_rdgrpinfo2( unsigned short, short, short, short, IODBTGI2 * ) ;

/* write tool life management data(tool group information 2) */
 FWLIBAPI short WINAPI cnc_wrgrpinfo2( unsigned short, short, IODBTGI2 * ) ;

/* read tool life management data(tool group information 3) */
 FWLIBAPI short WINAPI cnc_rdgrpinfo3( unsigned short, short, short, short, IODBTGI3 * ) ;

/* write tool life management data(tool group information 3) */
 FWLIBAPI short WINAPI cnc_wrgrpinfo3( unsigned short, short, IODBTGI3 * ) ;


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

/* read operation history data */
 FWLIBAPI short WINAPI cnc_rdophistry2( unsigned short, unsigned short, unsigned short *, unsigned short *, void * ) ;

/* read number of alarm history data */
 FWLIBAPI short WINAPI cnc_rdalmhisno( unsigned short, unsigned short * ) ;

/* read alarm history data */
 FWLIBAPI short WINAPI cnc_rdalmhistry( unsigned short, unsigned short, unsigned short, unsigned short, ODBAHIS * ) ;

/* read alarm history data */
 FWLIBAPI short WINAPI cnc_rdalmhistry2( unsigned short, unsigned short, unsigned short, unsigned short, ODBAHIS2 * ) ;

/* clear operation history data */
 FWLIBAPI short WINAPI cnc_clearophis( unsigned short, short ) ;

/* read signals related operation history */
 FWLIBAPI short WINAPI cnc_rdhissgnl( unsigned short, IODBSIG * ) ;

/* write signals related operation history */
 FWLIBAPI short WINAPI cnc_wrhissgnl( unsigned short, IODBSIG * ) ;

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


/*----------------------------*/
/* CNC: Servo/Spindle related */
/*----------------------------*/

/* read the loop gain for each axis */
 FWLIBAPI short WINAPI cnc_rdloopgain( unsigned short, long * ) ;

/* read the actual current for each axis */
 FWLIBAPI short WINAPI cnc_rdcurrent( unsigned short, short * ) ;

/* read the actual speed for each axis */
 FWLIBAPI short WINAPI cnc_rdsrvspeed( unsigned short, long * ) ;

/* read the number of controled spindle */
 FWLIBAPI short WINAPI cnc_rdnspdl( unsigned short, short * ) ;

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


/*----------------------------------------------------*/
/* CNC: Data Server, DNC1, DNC2, OSI-Ethernet related */
/*----------------------------------------------------*/

/* transfer a file from host computer to CNC by FTP */
 FWLIBAPI short WINAPI cnc_dtsvftpget( unsigned short, char *, char * ) ;

/* transfer a file from CNC to host computer by FTP */
 FWLIBAPI short WINAPI cnc_dtsvftpput( unsigned short, char *, char * ) ;

/* get transfer status for FTP */
 FWLIBAPI short WINAPI cnc_dtsvftpstat( unsigned short ) ;

/* read file directory in Data Server */
 FWLIBAPI short WINAPI cnc_dtsvrdpgdir( unsigned short, char *, short, ODBDSDIR * ) ;

/* delete files in Data Server */
 FWLIBAPI short WINAPI cnc_dtsvdelete( unsigned short, char * ) ;

/* down load from CNC (transfer a file from CNC to MMC) */
 FWLIBAPI short WINAPI cnc_dtsvdownload( unsigned short, char * ) ;

/* up load to CNC (transfer a file from MMC to CNC) */
 FWLIBAPI short WINAPI cnc_dtsvupload( unsigned short, char * ) ;

/* close upload/download between Data Server and CNC */
 FWLIBAPI short WINAPI cnc_dtsvcnclupdn( unsigned short ) ;

/* get transfer status for up/down load */
 FWLIBAPI short WINAPI cnc_dtsvupdnstat( unsigned short ) ;

/* get file name for DNC operation in Data Server */
 FWLIBAPI short WINAPI cnc_dtsvgetdncpg( unsigned short, char * ) ;

/* set program number of DNC oparation to CNC */
 FWLIBAPI short WINAPI cnc_dtsvsetdncpg( unsigned short, char * ) ;

/* read setting data for Data Server */
 FWLIBAPI short WINAPI cnc_dtsvrdset( unsigned short, IODBDSSET * ) ;

/* write setting data for Data Server */
 FWLIBAPI short WINAPI cnc_dtsvwrset( unsigned short, IODBDSSET * ) ;

/* check hard disk in Data Server */
 FWLIBAPI short WINAPI cnc_dtsvchkdsk( unsigned short ) ;

/* format hard disk in Data Server */
 FWLIBAPI short WINAPI cnc_dtsvhdformat( unsigned short ) ;

/* save interface area in Data Server */
 FWLIBAPI short WINAPI cnc_dtsvsavecram( unsigned short ) ;

/* get interface area in Data Server */
 FWLIBAPI short WINAPI cnc_dtsvrdcram( unsigned short, long, long *, char * ) ;

/* read maintenance information for Data Server */
 FWLIBAPI short WINAPI cnc_dtsvmntinfo( unsigned short, ODBDSMNT * ) ;

/* get Data Server mode */
 FWLIBAPI short WINAPI cnc_dtsvgetmode( unsigned short, short * ) ;

/* set Data Server mode */
 FWLIBAPI short WINAPI cnc_dtsvsetmode( unsigned short, short ) ;

/* read error message for Data Server */
 FWLIBAPI short WINAPI cnc_dtsvrderrmsg( unsigned short, short, char * ) ;

/* transfar file from Pc to Data Server */
 FWLIBAPI short WINAPI cnc_dtsvwrfile( unsigned short, char *, char *,short );

/* transfar file from Data Server to Pc */
 FWLIBAPI short WINAPI cnc_dtsvrdfile( unsigned short, char *, char *,short );

/* read DNC file name for DNC1, DNC2, OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rddncfname( unsigned short, char * ) ;

/* write DNC file name for DNC1, DNC2, OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_wrdncfname( unsigned short, char * ) ;

/* read communication parameter for DNC1, DNC2, OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rdcomparam( unsigned short, IODBCPRM * ) ;

/* write communication parameter for DNC1, DNC2, OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_wrcomparam( unsigned short, IODBCPRM * ) ;

/* read log message for DNC2 */
 FWLIBAPI short WINAPI cnc_rdcomlogmsg( unsigned short, char * ) ;

/* read operator message for DNC1, DNC2 */
 FWLIBAPI short WINAPI cnc_rdcomopemsg( unsigned short, char * ) ;

/* read recieve message for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rdrcvmsg( unsigned short, char * ) ;

/* read send message for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rdsndmsg( unsigned short, char * ) ;

/* send message for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_sendmessage( unsigned short, char * ) ;

/* clear message buffer for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_clrmsgbuff( unsigned short, short ) ;

/* read message recieve status for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rdrcvstat( unsigned short, unsigned short * ) ;

/* read data from FANUC BUS */
 FWLIBAPI short WINAPI cnc_rdfbusmem( unsigned short, short, short, long, long, void * ) ;

/* write data to FANUC BUS */
 FWLIBAPI short WINAPI cnc_wrfbusmem( unsigned short, short, short, long, long, void * ) ;


/*--------------------------*/
/* CNC: PUNCH PRESS related */
/*--------------------------*/

/* read tool controll data */
 FWLIBAPI short WINAPI cnc_rdtlctldata( unsigned short, IODBTLCTL * ) ;

/* write tool controll data */
 FWLIBAPI short WINAPI cnc_wrtlctldata( unsigned short, IODBTLCTL * ) ;

/* read tool data */
 FWLIBAPI short WINAPI cnc_rdtooldata( unsigned short, short, short *, IODBTLDT * ) ;

/* read tool data */
 FWLIBAPI short WINAPI cnc_wrtooldata( unsigned short, short, short *, IODBTLDT * ) ;

/* read multi tool data */
 FWLIBAPI short WINAPI cnc_rdmultitldt( unsigned short, short, short *, IODBMLTTL * ) ;

/* write multi tool data */
 FWLIBAPI short WINAPI cnc_wrmultitldt( unsigned short, short, short *, IODBMLTTL * ) ;

/* read safetyzone data */
 FWLIBAPI short WINAPI cnc_rdsafetyzone( unsigned short, short, short *, IODBSAFE * ) ;

/* write safetyzone data */
 FWLIBAPI short WINAPI cnc_wrsafetyzone( unsigned short, short, short *, IODBSAFE * ) ;

/* read toolzone data */
 FWLIBAPI short WINAPI cnc_rdtoolzone( unsigned short, short, short *, IODBTLZN * ) ;

/* write toolzone data */
 FWLIBAPI short WINAPI cnc_wrtoolzone( unsigned short, short, short *, IODBTLZN * ) ;

/* read active toolzone data */
 FWLIBAPI short WINAPI cnc_rdacttlzone( unsigned short, ODBACTTLZN * ) ;

/* read setzone number */
 FWLIBAPI short WINAPI cnc_rdsetzone( unsigned short, short * ) ;

/* write setzone number */
 FWLIBAPI short WINAPI cnc_wrsetzone( unsigned short, short ) ;

/* read axis name */
FWLIBAPI short WINAPI cnc_rdaxisname( unsigned short, short *, ODBAXISNAME *);

/* read spindle name */
FWLIBAPI short WINAPI cnc_rdspdlname( unsigned short, short *, ODBSPDLNAME *);


/*--------------------*/
/* CNC: LASER related */
/*--------------------*/

/* read processing condition file (processing data) */
 FWLIBAPI short WINAPI cnc_rdpscdproc( unsigned short, short, short *, IODBPSCD * ) ;

/* write processing condition file (processing data) */
 FWLIBAPI short WINAPI cnc_wrpscdproc( unsigned short, short, short *, IODBPSCD * ) ;

/* read processing condition file (piercing data) */
 FWLIBAPI short WINAPI cnc_rdpscdpirc( unsigned short, short, short *, IODBPIRC * ) ;

/* write processing condition file (piercing data) */
 FWLIBAPI short WINAPI cnc_wrpscdpirc( unsigned short, short, short *, IODBPIRC * ) ;

/* read processing condition file (edging data) */
 FWLIBAPI short WINAPI cnc_rdpscdedge( unsigned short, short, short *, IODBEDGE * ) ;

/* write processing condition file (edging data) */
 FWLIBAPI short WINAPI cnc_wrpscdedge( unsigned short, short, short *, IODBEDGE * ) ;

/* read processing condition file (slope data) */
 FWLIBAPI short WINAPI cnc_rdpscdslop( unsigned short, short, short *, IODBSLOP * ) ;

/* write processing condition file (slope data) */
 FWLIBAPI short WINAPI cnc_wrpscdslop( unsigned short, short, short *, IODBSLOP * ) ;

/* read power controll duty data */
 FWLIBAPI short WINAPI cnc_rdlpwrdty( unsigned short, IODBLPWDT * ) ;

/* write power controll duty data */
 FWLIBAPI short WINAPI cnc_wrlpwrdty( unsigned short, IODBLPWDT * ) ;

/* read laser power data */
 FWLIBAPI short WINAPI cnc_rdlpwrdat( unsigned short, ODBLOPDT * ) ;

/* read power complement */
 FWLIBAPI short WINAPI cnc_rdlpwrcpst( unsigned short, short * ) ;

/* write power complement */
 FWLIBAPI short WINAPI cnc_wrlpwrcpst( unsigned short, short ) ;

/* read laser assist gas selection */
 FWLIBAPI short WINAPI cnc_rdlagslt( unsigned short, IODBLAGSL * ) ;

/* write laser assist gas selection */
 FWLIBAPI short WINAPI cnc_wrlagslt( unsigned short, IODBLAGSL * ) ;

/* read laser assist gas flow */
 FWLIBAPI short WINAPI cnc_rdlagst( unsigned short, IODBLAGST * ) ;

/* write laser assist gas flow */
 FWLIBAPI short WINAPI cnc_wrlagst( unsigned short, IODBLAGST * ) ;

/* read laser power for edge processing */
 FWLIBAPI short WINAPI cnc_rdledgprc( unsigned short, IODBLEGPR * ) ;

/* write laser power for edge processing */
 FWLIBAPI short WINAPI cnc_wrledgprc( unsigned short, IODBLEGPR * ) ;

/* read laser power for piercing */
 FWLIBAPI short WINAPI cnc_rdlprcprc( unsigned short, IODBLPCPR * ) ;

/* write laser power for piercing */
 FWLIBAPI short WINAPI cnc_wrlprcprc( unsigned short, IODBLPCPR * ) ;

/* read laser command data */
 FWLIBAPI short WINAPI cnc_rdlcmddat( unsigned short, ODBLCMDT * ) ;

/* read displacement */
 FWLIBAPI short WINAPI cnc_rdldsplc( unsigned short, short * ) ;

/* write displacement */
 FWLIBAPI short WINAPI cnc_wrldsplc( unsigned short, short ) ;

/* read error for axis z */
 FWLIBAPI short WINAPI cnc_rdlerrz( unsigned short, short * ) ;

/* read active number */
 FWLIBAPI short WINAPI cnc_rdlactnum( unsigned short, ODBLACTN * ) ;

/* read laser comment */
 FWLIBAPI short WINAPI cnc_rdlcmmt( unsigned short, ODBLCMMT * ) ;

/* read laser power select */
 FWLIBAPI short WINAPI cnc_rdlpwrslt( unsigned short, short * ) ;

/* write laser power select */
 FWLIBAPI short WINAPI cnc_wrlpwrslt( unsigned short, short ) ;

/* read laser power controll */
 FWLIBAPI short WINAPI cnc_rdlpwrctrl( unsigned short, short * ) ;

/* write laser power controll */
 FWLIBAPI short WINAPI cnc_wrlpwrctrl( unsigned short, short ) ;


/*---------------------------*/
/* CNC : Servo learning data */
/*---------------------------*/

/* Servo learning data read start */
FWLIBAPI short WINAPI cnc_svdtstartrd( unsigned short, short );

/* Servo learning data read from I/F buffer */
FWLIBAPI short WINAPI cnc_svdtrddata( unsigned short, short *, long *, void * );

/* Servo learning data read end */
FWLIBAPI short WINAPI cnc_svdtendrd( unsigned short );

/* Servo learning data write start */
FWLIBAPI short WINAPI cnc_svdtstartwr( unsigned short, short );

/* Servo learning data write to I/F buffer */
FWLIBAPI short WINAPI cnc_svdtwrdata( unsigned short, short *, long *, void * );

/* Servo learning data write end */
FWLIBAPI short WINAPI cnc_svdtendwr( unsigned short );

/* Servo learning data read/write stop */
FWLIBAPI short WINAPI cnc_svdtstopexec( unsigned short );


/*-------------*/
/* CNC: Others */
/*-------------*/

/* read CNC system information */
 FWLIBAPI short WINAPI cnc_sysinfo( unsigned short, ODBSYS * ) ;

/* read CNC configuration information */
 FWLIBAPI short WINAPI cnc_sysconfig( unsigned short, ODBSYSC * ) ;

/* read CNC system soft series and version */
 FWLIBAPI short WINAPI cnc_rdsyssoft( unsigned short, ODBSYSS * ) ;

/* read CNC system soft series and version */
 FWLIBAPI short WINAPI cnc_rdsyssoft2( unsigned short, ODBSYSS2 * ) ;

/* read CNC module configuration information */
 FWLIBAPI short WINAPI cnc_rdmdlconfig( unsigned short, ODBMDLC * ) ;

/* read CNC module configuration information 2 */
 FWLIBAPI short WINAPI cnc_rdmdlconfig2( unsigned short, char * ) ;

/* read CNC status information */
 FWLIBAPI short WINAPI cnc_statinfo( unsigned short, ODBST * ) ;

/* read alarm status */
 FWLIBAPI short WINAPI cnc_alarm( unsigned short, ODBALM * ) ;

/* read alarm status */
 FWLIBAPI short WINAPI cnc_alarm2( unsigned short, long * ) ;

/* read alarm information */
 FWLIBAPI short WINAPI cnc_rdalminfo( unsigned short, short, short, short, ALMINFO * ) ;

/* read modal data */
 FWLIBAPI short WINAPI cnc_modal( unsigned short, short, short, ODBMDL * ) ;

/* read modal G code */
 FWLIBAPI short WINAPI cnc_rdgcode( unsigned short, short, short, short *, ODBGCD * ) ;

/* read command value */
 FWLIBAPI short WINAPI cnc_rdcommand( unsigned short, short, short, short *, ODBCMD * ) ;

/* read diagnosis data */
 FWLIBAPI short WINAPI cnc_diagnoss( unsigned short, short, short, short, ODBDGN * ) ;

/* read diagnosis data(area specified) */
 FWLIBAPI short WINAPI cnc_diagnosr( unsigned short, short *, short, short *, short *, void * ) ;

/* read informations of CNC diagnose data */
 FWLIBAPI short WINAPI cnc_rddiaginfo( unsigned short, short, unsigned short, ODBDIAGIF * ) ;

/* read maximum, minimum and total number of CNC diagnose data */
 FWLIBAPI short WINAPI cnc_rddiagnum( unsigned short, ODBDIAGNUM * ) ;

/* read A/D conversion data */
 FWLIBAPI short WINAPI cnc_adcnv( unsigned short, short, short, ODBAD * ) ;

/* read operator's message */
 FWLIBAPI short WINAPI cnc_rdopmsg( unsigned short, short, short, OPMSG * ) ;

/* read operator's message 3 */
 FWLIBAPI short WINAPI cnc_rdopmsg3( unsigned short, short, short *, OPMSG3 * ) ;

/* read program restart information */
 FWLIBAPI short WINAPI cnc_rdprstrinfo( unsigned short, ODBPRS * ) ;

/* search sequence number for program restart */
 FWLIBAPI short WINAPI cnc_rstrseqsrch( unsigned short, long, long, short, short ) ;

/* search sequence number for program restart 2 */
 FWLIBAPI short WINAPI cnc_rstrseqsrch2( unsigned short, long, long, short, short, long ) ;

/* read block re-start information */
 FWLIBAPI short WINAPI cnc_rdbrstrinfo( unsigned short, ODBBRS * ) ;

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

/* read menu switch signal */
 FWLIBAPI short WINAPI cnc_rdmenuswitch( unsigned short, short * ) ;

/* write menu switch signal */
 FWLIBAPI short WINAPI cnc_wrmenuswitch( unsigned short, short, short ) ;

 /* read tool radius offset for position data */
 FWLIBAPI short WINAPI cnc_rdradofs( unsigned short, ODBROFS * ) ;

/* read tool length offset for position data */
 FWLIBAPI short WINAPI cnc_rdlenofs( unsigned short, ODBLOFS * ) ;

/* read fixed cycle for position data */
 FWLIBAPI short WINAPI cnc_rdfixcycle( unsigned short, ODBFIX * ) ;

/* read coordinate rotate for position data */
 FWLIBAPI short WINAPI cnc_rdcdrotate( unsigned short, ODBROT * ) ;

/* read 3D coordinate convert for position data */
 FWLIBAPI short WINAPI cnc_rd3dcdcnv( unsigned short, ODB3DCD * ) ;

/* read programable mirror image for position data */
 FWLIBAPI short WINAPI cnc_rdmirimage( unsigned short, ODBMIR * ) ;

/* read scaling for position data */
 FWLIBAPI short WINAPI cnc_rdscaling( unsigned short, ODBSCL * ) ;

/* read 3D tool offset for position data */
 FWLIBAPI short WINAPI cnc_rd3dtofs( unsigned short, ODB3DTO * ) ;

/* read tool position offset for position data */
 FWLIBAPI short WINAPI cnc_rdposofs( unsigned short, ODBPOFS * ) ;

/* get maximum valid figures and number of decimal places */
 FWLIBAPI short WINAPI cnc_getfigure( unsigned short, short, short *, short *, short * ) ;

/* get path number(for 4 axes lathes, multi-path) */
 FWLIBAPI short WINAPI cnc_getpath( unsigned short, short *, short * ) ;

/* set path number(for 4 axes lathes, multi-path) */
 FWLIBAPI short WINAPI cnc_setpath( unsigned short, short ) ;

 /* get detail error */
 FWLIBAPI short WINAPI cnc_getdtailerr( unsigned short, ODBERR * ) ;

/* get date and time from cnc */
 FWLIBAPI short WINAPI cnc_gettimer( unsigned short, IODBTIMER * ) ;

/* set date and time for cnc */
 FWLIBAPI short WINAPI cnc_settimer( unsigned short, IODBTIMER * ) ;

/* clear CNC alarm */
 FWLIBAPI short WINAPI cnc_clralm( unsigned short hndl, short id ) ;

/* read timer data from cnc */
 FWLIBAPI short WINAPI cnc_rdtimer( unsigned short, short, IODBTIME * ) ;

/* write timer data for cnc */
 FWLIBAPI short WINAPI cnc_wrtimer( unsigned short, short, IODBTIME * ) ;


/*------------------*/
/* CNC : SERCOS I/F */
/*------------------*/

/* Get reservation of service channel for SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsrsvchnl( unsigned short );

/* Read ID information of SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsrdidinfo( unsigned short, long, short, short, IODBIDINF * );

/* Write ID information of SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcswridinfo( unsigned short, IODBIDINF * );

/* Start of reading operation data from drive of SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsstartrd( unsigned short, long, short );

/* Start of writing operation data to drive of SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsstartwrt( unsigned short, long, short );

/* Stop of reading/writing operation data from/to drive of SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsstopexec( unsigned short );

/* Get execution status of reading/writing operation data of SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsrdexstat( unsigned short, ODBSRCSST * );

/* Read operation data from data buffer for SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsrdopdata( unsigned short, long, long *, void * );

/* Write operation data to data buffer for SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcswropdata( unsigned short, long, long, void * );

/* Free reservation of service channel for SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsfreechnl( unsigned short );

/* Read drive assign of SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsrdlayout( unsigned short, ODBSRCSLYT * );

/* Read communication phase of drive of SERCOS I/F */
FWLIBAPI short WINAPI cnc_srcsrddrvcp( unsigned short, short * );


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

/* read data from extended backup memory */
 FWLIBAPI short WINAPI pmc_rdkpm( unsigned short, unsigned long, char *, unsigned short ) ;

/* write data to extended backup memory */
 FWLIBAPI short WINAPI pmc_wrkpm( unsigned short, unsigned long, char *, unsigned short ) ;

/* read maximum size of extended backup memory */
 FWLIBAPI short WINAPI pmc_kpmsiz( unsigned short, unsigned long * ) ;

/* read PMC alarm message */
 FWLIBAPI short WINAPI pmc_rdalmmsg( unsigned short, short, short *, short *, ODBPMCALM * ) ;

/* get detail error for pmc */
 FWLIBAPI short WINAPI pmc_getdtailerr( unsigned short, ODBPMCERR * ) ;

/* read PMC memory data */
 FWLIBAPI short WINAPI pmc_rdpmcmem( unsigned short, short, long, long, void * ) ;

/* write PMC memory data */
 FWLIBAPI short WINAPI pmc_wrpmcmem( unsigned short, short, long, long, void * ) ;


/*----------------------------*/
/* PMC : PROFIBUS function    */
/*----------------------------*/

/* read PROFIBUS configration data */
 FWLIBAPI short WINAPI pmc_prfrdconfig( unsigned short, ODBPRFCNF * ) ;

/* read bus parameter for master function */
 FWLIBAPI short WINAPI pmc_prfrdbusprm( unsigned short, IODBBUSPRM * ) ;

/* write bus parameter for master function */
 FWLIBAPI short WINAPI pmc_prfwrbusprm( unsigned short, IODBBUSPRM * ) ;

/* read slave parameter for master function */
 FWLIBAPI short WINAPI pmc_prfrdslvprm( unsigned short, short, IODBSLVPRM * ) ;

/* write slave parameter for master function */
 FWLIBAPI short WINAPI pmc_prfwrslvprm( unsigned short, short, IODBSLVPRM * ) ;

/* read allocation address for master function */
 FWLIBAPI short WINAPI pmc_prfrdallcadr( unsigned short, short, IODBPRFADR * ) ;

/* set allocation address for master function */
 FWLIBAPI short WINAPI pmc_prfwrallcadr( unsigned short, short, IODBPRFADR * ) ;

/* read allocation address for slave function */
 FWLIBAPI short WINAPI pmc_prfrdslvaddr( unsigned short, IODBSLVADR * ) ;

/* set allocation address for slave function */
 FWLIBAPI short WINAPI pmc_prfwrslvaddr( unsigned short, IODBSLVADR * ) ;

/* read status for slave function */
 FWLIBAPI short WINAPI pmc_prfrdslvstat( unsigned short, ODBSLVST * ) ;


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
