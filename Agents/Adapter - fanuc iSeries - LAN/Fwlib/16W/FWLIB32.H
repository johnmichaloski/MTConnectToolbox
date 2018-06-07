//-------------------------------------------------------
// fwlib32.h                                             
//                                                       
// CNC/PMC Data Window Library for series 160i/180i-W    
//                                                       
// Copyright (C) 1998,99 by FANUC LTD. All rights reserved. 
//                                                       
//-------------------------------------------------------

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


//
//    Axis define
//
#define MAX_AXIS        8
#define ALL_AXES        -1


//
//    Error Codes
//
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


//
//   Result codes of DNC operation
//
#define DNC_NORMAL      (-1)            /* normal completed */
#define DNC_CANCEL      (-32768)        /* DNC operation was canceled by CNC */
#define DNC_OPENERR     (-514)          /* file open error */
#define DNC_NOFILE      (-516)          /* file not found */
#define DNC_READERR     (-517)          /* read error */


//--------------------
//                    
// Structure Template 
//                    
//--------------------

//---------------------------
// CNC: Control axis related 
//---------------------------

#pragma pack(push, 4)

/* cnc_actf:read actual axis feedrate(F) */
typedef struct odbact {
    short   dummy[2] ;  /* dummy */
    long    data ;      /* actual feed / actual spindle */
} ODBACT ;

/* cnc_absolute:read absolute axis position */
/* cnc_machine:read machine axis position */
/* cnc_relative:read relative axis position */
/* cnc_skip:read skip position */
/* cnc_srvdelay:read servo delay value */
/* cnc_accdecdly:read acceleration/deceleration delay value */
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
            long    distance[MAX_AXIS] ;    /* dummy */
        } faxis ;
        struct {
            long    absolute ;  /* absolute position */
            long    machine ;   /* machine position */
            long    relative ;  /* relative position */
            long    distance ;  /* dummy */
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


//----------------------
// CNC: Program related 
//----------------------

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
    short   crntprog;   /* dummy */
    long    crntpntr;   /* dummy */
} ODBMDIP;


//---------------------------
// CNC: NC file data related 
//---------------------------

/* cnc_rdzofsr:read work zero offset value(area specified) */
/* cnc_wrzofsr:write work zero offset value(area specified) */
typedef struct iodbzor {
    short   datano_s ;  /* start offset number */
    short   type ;      /* axis number */
    short   datano_e ;  /* end offset number */
    long    data[56] ;  /* offset value */
} IODBZOR ; /* In case that the number of axes is MAX_AXIS(=8), the number of data is 7 */

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


//-------------------------------------
// CNC: Operation history data related 
//-------------------------------------

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


//-------------
// CNC: Others 
//-------------

/* cnc_rdwaveprm:read the parameter of wave diagnosis */
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

/* cnc_statinfo:read CNC status information */
typedef struct odbst {
    short   dummy[2] ;  /* dummy */
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
        char    g_rdata[12];
        char    g_1shot;
        struct {
            long    aux_data;
            char    flag1;
            char    flag2;
        }aux;
        struct {
            long    aux_data;
            char    flag1;
            char    flag2;
        }raux1[25];
    }modal;
} ODBMDL ;

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

/* cnc_rdopmsg:read operator's message */
typedef struct msg {
    short   datano ;    /* operator's message number */
    short   type ;      /* operator's message type */
    short   char_num ;  /* message string length */
    char    data[256] ; /* operator's message string */
} OPMSG ;   /* In case that the data length is 256 */

/* cnc_rdopmsg3:read operator's message */
typedef struct opmsg3 {
    short   datano ;    /* operator's message number */
    short   type ;      /* operator's message type */
    short   char_num ;  /* message string length */
    char    data[256] ;  /* operator's message string */
} OPMSG3 ;   /* In case that the data length is 256 */

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


//-----
// PMC 
//-----

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


//--------------------------
// HSSB multiple connection 
//--------------------------

/* cnc_rdnodeinfo:read node informations */
typedef struct odbnode {
        long node_no;
        long io_base;
        long status;
        long cnc_type;
        char node_name[20];
} ODBNODE;

#pragma pack(pop)


//---------------------------
// CNC: Control axis related
//---------------------------

/* read actual axis feedrate(F) */
 FWLIBAPI short WINAPI cnc_actf( unsigned short, ODBACT * );

/* read absolute axis position */
 FWLIBAPI short WINAPI cnc_absolute( unsigned short, short, short, ODBAXIS * );

/* read machine axis position */
 FWLIBAPI short WINAPI cnc_machine( unsigned short, short, short, ODBAXIS * );

/* read relative axis position */
 FWLIBAPI short WINAPI cnc_relative( unsigned short, short, short, ODBAXIS * );

/* read skip position */
 FWLIBAPI short WINAPI cnc_skip( unsigned short, short, short, ODBAXIS * );

/* read servo delay value */
 FWLIBAPI short WINAPI cnc_srvdelay( unsigned short, short, short, ODBAXIS * );

/* read acceleration/deceleration delay value */
 FWLIBAPI short WINAPI cnc_accdecdly( unsigned short, short, short, ODBAXIS * );

/* read all dynamic data */
 FWLIBAPI short WINAPI cnc_rddynamic( unsigned short, short, short, ODBDY * );

/* set origin / preset relative axis position */
 FWLIBAPI short WINAPI cnc_wrrelpos( unsigned short, short, IDBWRR * );

/* preset work coordinate */
 FWLIBAPI short WINAPI cnc_prstwkcd( unsigned short, short, IDBWRA * );

/* set wire vertival position */
 FWLIBAPI short WINAPI cnc_setvrtclpos( unsigned short, short );

/* set wire threading position */
 FWLIBAPI short WINAPI cnc_setthrdngpos( unsigned short );


//----------------------
// CNC: Program related 
//----------------------

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

/* read program under execution */
 FWLIBAPI short WINAPI cnc_rdexecprog( unsigned short, unsigned short *, short *, char * ) ;

/* write program for MDI operation */
 FWLIBAPI short WINAPI cnc_wrmdiprog( unsigned short, short, char * ) ;

/* read execution pointer for MDI operation */
 FWLIBAPI short WINAPI cnc_rdmdipntr( unsigned short, ODBMDIP * ) ;

/* write execution pointer for MDI operation */
 FWLIBAPI short WINAPI cnc_wrmdipntr( unsigned short, long ) ;


//---------------------------
// CNC: NC file data related 
//---------------------------

/* read work zero offset value(area specified) */
 FWLIBAPI short WINAPI cnc_rdzofsr( unsigned short, short, short, short, short, IODBZOR * ) ;

/* write work zero offset value(area specified) */
 FWLIBAPI short WINAPI cnc_wrzofsr( unsigned short, short, IODBZOR * ) ;

/* read work zero offset information */
 FWLIBAPI short WINAPI cnc_rdzofsinfo( unsigned short, short * ) ;

/* read parameter(area specified) */
 FWLIBAPI short WINAPI cnc_rdparar( unsigned short, short *, short, short *, short *, void * ) ;

/* write parameter(area specified) */
 FWLIBAPI short WINAPI cnc_wrparas( unsigned short, short, void * ) ;

/* read informations of CNC parameter */
 FWLIBAPI short WINAPI cnc_rdparainfo( unsigned short, short, unsigned short, ODBPARAIF * ) ;

/* read maximum, minimum and total number of CNC parameter */
 FWLIBAPI short WINAPI cnc_rdparanum( unsigned short, ODBPARANUM * ) ;

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


//-------------------------------------
// CNC: Operation history data related 
//-------------------------------------

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


//-------------------------
// CNC: Servo data related
//-------------------------

/* read the loop gain for each axis */
 FWLIBAPI short WINAPI cnc_rdloopgain( unsigned short, long * ) ;

/* read the actual current for each axis */
 FWLIBAPI short WINAPI cnc_rdcurrent( unsigned short, short * ) ;

/* read the actual speed for each axis */
 FWLIBAPI short WINAPI cnc_rdsrvspeed( unsigned short, long * ) ;

/* read the parameter of wave diagnosis */
 FWLIBAPI short WINAPI cnc_rdwaveprm( unsigned short, IODBWAVE * ) ;

/* write the parameter of wave diagnosis */
 FWLIBAPI short WINAPI cnc_wrwaveprm( unsigned short, IODBWAVE * ) ;

/* start the sampling for wave diagnosis */
 FWLIBAPI short WINAPI cnc_wavestart( unsigned short ) ;

/* stop the sampling for wave diagnosis */
 FWLIBAPI short WINAPI cnc_wavestop( unsigned short ) ;

/* read the status of wave diagnosis */
 FWLIBAPI short WINAPI cnc_wavestat( unsigned short, short * ) ;

/* read the data of wave diagnosis */
 FWLIBAPI short WINAPI cnc_rdwavedata( unsigned short, short, short, long, long *, ODBWVDT * ) ;


//-------------
// CNC: Others 
//-------------

/* read CNC system information */
 FWLIBAPI short WINAPI cnc_sysinfo( unsigned short, ODBSYS * ) ;

/* read CNC system soft series and version */
 FWLIBAPI short WINAPI cnc_rdsyssoft( unsigned short, ODBSYSS * ) ;

/* read CNC system soft series and version */
 FWLIBAPI short WINAPI cnc_rdsyssoft2( unsigned short, ODBSYSS2 * ) ;

/* read CNC module configuration information 2 */
 FWLIBAPI short WINAPI cnc_rdmdlconfig2( unsigned short, char * ) ;

/* read CNC status information */
 FWLIBAPI short WINAPI cnc_statinfo( unsigned short, ODBST * ) ;

/* read alarm status */
 FWLIBAPI short WINAPI cnc_alarm( unsigned short, ODBALM * ) ;

/* read alarm information */
 FWLIBAPI short WINAPI cnc_rdalminfo( unsigned short, short, short, short, ALMINFO * ) ;

/* read modal data */
 FWLIBAPI short WINAPI cnc_modal( unsigned short, short, short, ODBMDL * ) ;

/* read diagnosis data(area specified) */
 FWLIBAPI short WINAPI cnc_diagnosr( unsigned short, short *, short, short *, short *, void * ) ;

/* read informations of CNC diagnose data */
 FWLIBAPI short WINAPI cnc_rddiaginfo( unsigned short, short, unsigned short, ODBDIAGIF * ) ;

/* read maximum, minimum and total number of CNC diagnose data */
 FWLIBAPI short WINAPI cnc_rddiagnum( unsigned short, ODBDIAGNUM * ) ;

/* read operator's message */
 FWLIBAPI short WINAPI cnc_rdopmsg( unsigned short, short, short, OPMSG * ) ;

/* read operator's message */
 FWLIBAPI short WINAPI cnc_rdopmsg3( unsigned short, short, short *, OPMSG3 * ) ;

/* get maximum valid figures and number of decimal places */
 FWLIBAPI short WINAPI cnc_getfigure( unsigned short, short, short *, short *, short * ) ;

/* allocate library handle */
 FWLIBAPI short WINAPI cnc_allclibhndl( unsigned short * ) ;

/* free library handle */
 FWLIBAPI short WINAPI cnc_freelibhndl( unsigned short ) ;

/* get detail error */
 FWLIBAPI short WINAPI cnc_getdtailerr( unsigned short, ODBERR * ) ;

/* get date and time from cnc */
 FWLIBAPI short WINAPI cnc_gettimer( unsigned short, IODBTIMER * ) ;

/* set date and time for cnc */
 FWLIBAPI short WINAPI cnc_settimer( unsigned short, IODBTIMER * ) ;

/* read data from FANUC BUS */
 FWLIBAPI short WINAPI cnc_rdfbusmem( unsigned short, short, short, long, long, void * ) ;

/* write data to FANUC BUS */
 FWLIBAPI short WINAPI cnc_wrfbusmem( unsigned short, short, short, long, long, void * ) ;

/* Get CNC Model */
FWLIBAPI short WINAPI cnc_getcncmodel( unsigned short, short * );


//-----
// PMC 
//-----

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

/* read PMC memory data */
 FWLIBAPI short WINAPI pmc_rdpmcmem( unsigned short, short, long, long, void * ) ;

/* write PMC memory data */
 FWLIBAPI short WINAPI pmc_wrpmcmem( unsigned short, short, long, long, void * ) ;

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


//--------------------------
// HSSB multiple connection 
//--------------------------

/* read number of node */
 FWLIBAPI short WINAPI cnc_rdnodenum( long * );

/* read node informations */
 FWLIBAPI short WINAPI cnc_rdnodeinfo( long, ODBNODE * );

/* set default node number */
 FWLIBAPI short WINAPI cnc_setdefnode( long );

/* allocate library handle 2 */
 FWLIBAPI short WINAPI cnc_allclibhndl2( long, unsigned short * );


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


/*-----------------*/
/* Dummy function  */
/*-----------------*/
/* set path number(for 4 axes lathes, multi-path) */
 FWLIBAPI short WINAPI cnc_setpath( unsigned short, short ) ;

/* get path number(for 4 axes lathes, multi-path) */
 FWLIBAPI short WINAPI cnc_getpath( unsigned short, short *, short * ) ;

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


#ifdef __cplusplus
}
#endif

#endif  /* _INC_FWLIB */
