/*-------------------------------------------------------*/
/* fwlib32.h                                             */
/*                                                       */
/* CNC/PMC Data Window Library for series 150            */
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
#if defined(M_AXIS2)
#define MAX_AXIS        24
#elif defined(M_AXIS1)
#define MAX_AXIS        15
#else
#define MAX_AXIS        10
#endif

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


/*----------------------*/
/*                      */
/*  Structure Template  */
/*                      */
/*----------------------*/
/*-------------------------------------*/
/* CNC: Control axis / spindle related */
/*-------------------------------------*/

#pragma pack(push, 4)

/* cnc_actf:read actual axis feedrate(F) */
/* cnc_acts:read actual spindle speed(S) */
typedef struct odbact {
     short dummy[2] ; /* dummy */
     long  data     ; /* actual feed / actual spindle  */
} ODBACT ;

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
    short   dummy ;      /* dummy       */
    short   type ;       /* axis number */
    long    data[MAX_AXIS] ;   /* data value  */
} ODBAXIS ;

/* cnc_rddynamic:read all dynamic data */
typedef struct odbdy {
    short   dummy ;
    short   axis ;       /* axis number             */
    short   alarm ;      /* alarm status            */
    short   prgnum ;     /* current program number  */
    short   prgmnum ;    /* dummy                   */
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
            long    absolute ;       /* absolute position   */
            long    machine ;        /* machine position    */
            long    relative ;       /* relative position   */
            long    distance ;       /* distance to go      */
        } oaxis ;                   /* In case of no axis  */
    } pos ;
} ODBDY ;

/* cnc_wrrelpos:set origin / preset relative axis position */
typedef struct idbwrr {
    short       datano;      /* dummy                     */
    short       type;        /* axis number               */
    long        data[MAX_AXIS];    /* preset data               */
} IDBWRR;

/* cnc_prstwkcd:preset work coordinate */
typedef struct idbwra {
    short       datano;      /* dummy                     */
    short       type;        /* axis number               */
    long        data[MAX_AXIS];    /* preset data               */
} IDBWRA;

/* cnc_rdmovrlap:read manual overlapped motion value */
typedef struct iodbovl {
    short       datano;         /* dummy                        */
    short       type;           /* axis number                  */
    long        data[2][MAX_AXIS];      /* data value                   */
} IODBOVL;

/* cnc_rdspload:read load information of serial spindle */
/* cnc_rdspmaxrpm:read maximum r.p.m. ratio of serial spindle */
/* cnc_rdspgear:read gear ratio of serial spindle */
typedef struct odbspn {
    short       datano;         /* spindle number               */
    short       type;           /* dummy                        */
    short       data[2];        /* spindle data                 */
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
    short dummy[2] ;   /*  dummy                  */
    char  data[256] ;  /*  data                   */
} ODBUP ;                     /* In case that the number of data is 256 */

/* cnc_buff:read buffer status for downloading/verification NC program */
typedef struct odbbuf {
    short dummy[2] ;   /* dummy         */
    short data ;       /* buffer status */
} ODBBUF ;

/* cnc_rdprogdir:read program directory */
typedef struct prgdir {
    char        prg_data[256] ;         /* directory data */
} PRGDIR ;                     /* In case that the number of data is 256 */

/* cnc_rdproginfo:read program information */
typedef struct odbnc {
    union {
        struct {
            short   reg_prg ;     /*  registered program number    */
            short   unreg_prg ;   /*  unregistered program number  */
            long    used_mem ;    /*  used memory area             */
            long    unused_mem ;  /*  unused memory area           */
        } bin ;                    /*  binary data type             */
        char asc[31] ;             /*  ASCII string type            */
    } u ;
} ODBNC ;

/* cnc_rdprgnum:read program number under execution */
typedef struct odbpro {
    short dummy[2] ;   /* dummy                   */
    short data ;       /* running program number */
    short mdata ;      /* main program number    */
} ODBPRO ;

/* cnc_rdseqnum:read sequence number under execution */
typedef struct odbseq {
    short dummy[2] ; /* dummy           */
    long  data     ; /* sequence number */
} ODBSEQ ;

/* cnc_rdmdipntr:read execution pointer for MDI operation */
typedef struct odbmdip {
    short       mdiprog;        /* exec. program number   */
    long        mdipntr;        /* exec. pointer          */
    short       crntprog;       /* prepare program number */
    long        crntpntr;       /* prepare pointer        */
} ODBMDIP;


/*---------------------------*/
/* CNC: NC file data related */
/*---------------------------*/

/* cnc_rdtofs:read tool offset value */
typedef struct odbtofs {
    short datano ;     /* data number */
    short type ;       /* data type   */
    long  data ;       /* data        */
} ODBTOFS ;

/* cnc_rdtofsr:read tool offset value(area specified) */
/* cnc_wrtofsr:write tool offset value(area specified) */
typedef struct iodbto {
    short datano_s ;   /* start offset number */
    short type ;       /* offset type         */
    short datano_e ;   /* end offset number   */
    union {
        long    m_ofs[5] ;      /* M Each         */
        long    m_ofs_a[5] ;    /* M-A All        */
        long    m_ofs_b[10] ;   /* M-B All        */
        long    m_ofs_c[20] ;   /* M-C All        */
        short   t_tip[5] ;      /* T Each, 2-byte */
        long    t_ofs[5] ;      /* T Each, 4-byte */
        struct {
            short   tip ;
            long    data[4] ;
        } t_ofs_a[5] ;          /* T-A All */
        struct {
            short   tip ;
            long    data[8] ;
        } t_ofs_b[5] ;          /* T-B All */
    } u ;                       /* In case that the number of data is 5 */
} IODBTO ;

/* cnc_rdzofs:read work zero offset value */
/* cnc_wrzofs:write work zero offset value */
typedef struct iodbzofs {
    short datano ;     /* offset NO.          */
    short type ;       /* axis number         */
    long  data[MAX_AXIS] ;   /* data value          */
} IODBZOFS ;

/* cnc_rdzofsr:read work zero offset value(area specified) */
/* cnc_wrzofsr:write work zero offset value(area specified) */
typedef struct iodbzor {
    short datano_s ;   /* start offset number */
    short type ;       /* axis number         */
    short datano_e ;   /* end offset number   */
    long  data[70] ;   /* offset value        */
} IODBZOR ; /* In case that the number of axes is MAX_AXIS(=10), the number of data is 7 */

/* cnc_rdparam:read parameter */
/* cnc_wrparam:write parameter */
/* cnc_rdset:read setting data */
/* cnc_wrset:write setting data */
/* cnc_rdparar:read parameter(area specified) */
/* cnc_wrparas:write parameter(plural specified) */
/* cnc_rdsetr:read setting data(area specified) */
/* cnc_wrsets:write setting data(plural specified) */
typedef struct iodbpsd {
    short datano ;      /*  data number              */
    short type ;        /*  axis number              */
    union {
        char cdata ;  /*  parameter / setting data */
        short idata ;
        long ldata ;
        char cdatas[MAX_AXIS] ;
        short idatas[MAX_AXIS] ;
        long ldatas[MAX_AXIS] ;
    } u ;
} IODBPSD ;

/* cnc_rdpitchr:read pitch error compensation data(area specified) */
/* cnc_wrpitchr:write pitch error compensation data(area specified) */
typedef struct iodbpi {
    short datano_s ;    /* start pitch number  */
    short dummy ;      /* dummy               */
    short datano_e ;   /* end pitch number    */
    char  data[5] ;     /* offset value        */
} IODBPI ;              /* In case that the number of data is 5 */

/* cnc_rdmacro:read custom macro variable */
typedef struct odbm {
    short datano ;     /* variable number     */
    short dummy ;      /* dummy               */
    long  mcr_val ;    /* macro variable      */
    short dec_val ;    /* decimal point       */
} ODBM ;

/* cnc_rdmacror:read custom macro variables(area specified) */
/* cnc_wrmacror:write custom macro variables(area specified) */
typedef struct iodbmr {
    short datano_s ;   /* start macro number  */
    short dummy ;      /* dummy               */
    short datano_e ;   /* end macro number    */
    struct {
        long  mcr_val ;   /* macro variable */
        short dec_val ;   /* decimal point  */
    } data[5] ;
} IODBMR ;              /* In case that the number of data is 5 */

/* cnc_rdpmacro:read P code macro variable */
typedef struct odbpm {
    long  datano ;     /* variable number     */
    short dummy ;      /* dummy               */
    long  mcr_val ;    /* macro variable      */
    short dec_val ;    /* decimal point       */
} ODBPM ;

/* cnc_rdpmacror:read P code macro variables(area specified) */
/* cnc_wrpmacror:write P code macro variables(area specified) */
typedef struct iodbpr {
    long  datano_s ;   /* start macro number  */
    short dummy ;      /* dummy               */
    long  datano_e ;   /* end macro number    */
    struct {
        long  mcr_val ;   /* macro variable */
        short dec_val ;   /* decimal point  */
    } data[5] ;
} IODBPR ;              /* In case that the number of data is 5 */

/* cnc_rdtofsinfo:read tool offset information */
typedef struct odbtlinf {
    short       ofs_type;
    short       use_no;
} ODBTLINF;

/* cnc_rdmacroinfo:read custom macro variable information */
typedef struct odbmvinf {
    short       use_no1;
    short       use_no2;
} ODBMVINF;

/* cnc_rdpmacroinfo:read P code macro variable information */
typedef struct odbpminf {
    short               use_no1;
#ifdef PCD_UWORD
    unsigned short      use_no2;
#else
    short               use_no2;
#endif
    short               v2_type1;
    short               v2_type2;
} ODBPMINF;


/*----------------------------------------*/
/* CNC: Tool life management data related */
/*----------------------------------------*/

/* cnc_rdgrpid:read tool life management data(tool group number) */
typedef struct odbtlife1 {
    short dummy ;      /* dummy     */
    short type ;       /* data type */
    long  data ;       /* data      */
} ODBTLIFE1 ;

/* cnc_rdngrp:read tool life management data(number of tool groups) */
typedef struct odbtlife2 {
    short dummy[2] ;   /* dummy     */
    long  data ;       /* data      */
} ODBTLIFE2 ;

/* cnc_rdntool:read tool life management data(number of tools) */
/* cnc_rdlife:read tool life management data(tool life) */
/* cnc_rdcount:read tool life management data(tool lift counter) */
typedef struct odbtlife3 {
    short datano ;     /* data number */
    short dummy ;      /* dummy       */
    long  data ;       /* data        */
} ODBTLIFE3 ;

/* cnc_rd1length:read tool life management data(tool length number-1) */
/* cnc_rd2length:read tool life management data(tool length number-2) */
/* cnc_rd1radius:read tool life management data(cutter compensation no.-1) */
/* cnc_rd2radius:read tool life management data(cutter compensation no.-2) */
/* cnc_t1info:read tool life management data(tool information-1) */
/* cnc_t2info:read tool life management data(tool information-2) */
/* cnc_toolnum:read tool life management data(tool number) */
typedef struct odbtlife4 {
    short datano ;     /* data number */
    short type ;       /* data type   */
    long  data ;       /* data        */
} ODBTLIFE4 ;

/* cnc_rdtoolrng:read tool life management data(tool number, tool life,    */
/*                                      tool life counter)(area specified) */
typedef struct iodbtr {
    short datano_s ;   /* start group number       */
    short dummy ;      /* dummy                    */
    short datano_e ;   /* end group number         */
    struct {
        long  ntool ;     /* tool number       */
        long  life ;      /* tool life         */
        long  count ;     /* tool life counter */
    } data[5] ;
} IODBTR ;              /* In case that the number of data is 5 */

/* cnc_rdtoolgrp:read tool life management data(all data within group) */
typedef struct odbtg {
    short grp_num ;    /*  start group number       */
    short dummy[2] ;   /*  dummy                    */
    long  ntool ;      /*  tool number              */
    long  life ;       /*  tool life                */
    long  count ;      /*  tool life counter        */
    struct {
        long  tuse_num ;     /* tool number        */
        long  tool_num ;     /* tool life          */
        long  length_num ;   /* tool life counter  */
        long  radius_num ;   /* tool life counter  */
        long  tinfo ;        /* tool life counter  */
    } data[5] ;
} ODBTG ;                      /* In case that the number of data is 5 */

/* cnc_wrcountr:write tool life management data(tool life counter)          */
/*                                                         (area specified) */
typedef struct idbwrc {
    short datano_s ;   /* start group number       */
    short dummy ;      /* dummy                    */
    short datano_e ;   /* end group number         */
    struct {
        long  dummy[2] ;  /* dummy             */
        long  count ;     /* tool life counter */
    } data[5] ;
} IDBWRC ;              /* In case that the number of data is 5 */

/* cnc_rdusegrpid:read tool life management data(used tool group number) */
typedef struct odbusegr {
    short       datano;         /* dummy                        */
    short       type;           /* dummy                        */
    long        next;           /* next use group number        */
    long        use;            /* using group number           */
    long        slct;           /* selecting group number       */
} ODBUSEGR;

/* cnc_rdmaxgrp:read tool life management data(max. number of tool groups)*/
/* cnc_rdmaxtool:read tool life management data                           */
/*                                   (maximum number of tool within group)*/
typedef struct odblfno {
    short     datano;         /* dummy */
    short     type;           /* dummy */
    short     data;           /* number of data */
} ODBLFNO;

/* cnc_rdusetlno:read tool life management data(used tool no within group)*/
typedef struct odbtluse {
    short     s_grp;           /* start group number */
    short     dummy;           /* dummy              */
    short     e_grp;           /* end group number   */
    long      data[5];        /* tool using number  */
} ODBTLUSE;                    /* In case that the number of group is 5 */

/* cnc_rd1tlifedata:read tool life management data(tool data1) */
/* cnc_rd2tlifedata:read tool life management data(tool data2) */
/* cnc_wr1tlifedata:write tool life management data(tool data1) */
/* cnc_wr2tlifedata:write tool life management data(tool data2) */
typedef struct iodbtd {
    short       datano;         /* tool group number            */
    short       type;           /* tool using number            */
    long        tool_num;       /* tool number                  */
    long        h_code;         /* H code                       */
    long        d_code;         /* D code                       */
    long        tool_inf;       /* tool information             */
} IODBTD;

/* cnc_rdgrpinfo:read tool life management data(tool group information) */
/* cnc_wrgrpinfo:write tool life management data(tool group information) */
typedef struct iodbtgi {
    short       s_grp;          /* start group number           */
    short       dummy;          /* dummy                        */
    short       e_grp;          /* end group number             */
    struct {
        long    n_tool;      /* number of tool          */
        long    count_value; /* tool life               */
        long    counter;     /* tool life counter       */
        long    count_type;  /* tool life counter type  */
    } data[5];
} IODBTGI;                    /* In case that the number of group is 5 */

/* (op)cnc_instlifedt:insert tool life management data(tool data) */
typedef struct idbitd {
    short       datano;         /* tool group number            */
    short       type;           /* tool using number            */
    long        data;           /* tool number                  */
} IDBITD;


/*-------------------------------------*/
/* CNC: Operation history data related */
/*-------------------------------------*/

/* cnc_rdophistry:read operation history data */
typedef struct odbhis {
    unsigned short      s_no;           /* start number */
    short               type;           /* dummy        */
    unsigned short      e_no;           /* end number   */
    union {
        struct {
            short       rec_type;       /* record type */
            short       alm_grp;        /* alarm group */
            short       alm_no;         /* alarm number */
            char        axis_no;        /* axis number */
            char        dummy;
        } rec_alm;
        struct {
            short       rec_type;       /* record type */
            char        key_code;       /* key code */
            char        pw_flag;        /* power on flag */
            char        dummy[4];
        } rec_mdi;
        struct {
            short       rec_type;       /* record type */
            char        sig_name;       /* signal name */
            char        sig_old;        /* old signal bit pattern */
            char        sig_new;        /* new signal bit pattern */
            char        dummy;
            short       sig_no;         /* signal number */
        } rec_sgn;
        struct {
            short       rec_type;       /* record type */
            char        year;           /* year */
            char        month;          /* month */
            char        day;            /* day */
            char        pw_flag;        /* power on flag */
            char        dummy[2];
        } rec_date;
        struct {
            short       rec_type;       /* record flag */
            char        hour;           /* hour */
            char        minute;         /* minute */
            char        second;         /* second */
            char        pw_flag;        /* power on flag */
            char        dummy[2];
        } rec_time;
    } data[10];
} ODBHIS;                    /* In case that the number of data is 10 */

/* cnc_rdalmhistry:read alarm history data */
typedef struct odbahis {
    unsigned short      s_no;           /* start number */
    short               type;           /* dummy        */
    unsigned short      e_no;           /* end number   */
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
} ODBAHIS;                    /* In case that the number of data is 10 */

/* cnc_rdhissgnl:read signals related operation history */
/* cnc_wrhissgnl:write signals related operation history */
typedef struct iodbsig {
    short       datano;           /* dummy              */
    short       type;             /* dummy              */
    struct {
        short   ent_no;   /* entry number       */
        short   sig_no;   /* signal number      */
        char    sig_name; /* signal name        */
        char    mask_pat; /* signal mask pattern */
    } data[20];
} IODBSIG;                    /* In case that the number of data is 20 */


/*---------------------------------*/
/* CNC: Wave diagnoss data related */
/*---------------------------------*/

/* cnc_rdwaveprm:read wave diagnoss data parameter */
/* cnc_wrwaveprm:write wave diagnoss data parameter */
typedef struct iodbwave {
    short       condition ;
    char        trg_adr ;
    char        trg_bit ;
    short       trg_no ;
    short       delay ;
    short       t_range ;
    struct {
        short   kind ;
        union {
#ifdef M_AXIS2
            long        axis ;
#else
            short       axis ;
#endif
            struct {
                char    adr ;
                char    bit ;
                short   no ;
            }io ;
        }u ;
    }ch[12] ;
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

/* cnc_rdwavedata:read wave diagnoss data */
typedef struct odbwvdt {
    short       channel ;
    short       kind ;
    union {
        short   axis ;
        struct {
            char        adr ;
            char        bit ;
            short       no ;
        }io ;
    }u ;
    char        year ;
    char        month ;
    char        day ;
    char        hour ;
    char        minute ;
    char        second ;
    short       t_cycle ;
    short       data[5000] ;
} ODBWVDT ;


/*-------------*/
/* CNC: Others */
/*-------------*/

/* cnc_sysinfo:read CNC system information */
typedef struct odbsys {
    short dummy ;       /* dummy                   */
    char  max_axis[2] ; /* maximum axis number     */
    char  cnc_type[2] ; /* cnc type <ascii char>   */
    char  mt_type[2] ;  /* M/T/TT <ascii char>     */
    char  series[4] ;   /* series NO. <ascii char> */
    char  version[4] ;  /* version NO.<ascii char> */
    char  axes[2] ;     /* axis number<ascii char> */
} ODBSYS ;

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
    short dummy[2] ;    /* dummy        */
    short data ;        /* alarm status */
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
    short datano;
    short type;
    union {
        char    g_data;
        char    g_rdata[21];
        char    g_1shot[4];
        struct {
            long        aux_data;
            char        flag1;
            char        flag2;
        }aux;
        struct {
            long        aux_data;
            char        flag1;
            char        flag2;
        }raux1[27];
        struct {
            long        aux_data;
            char        flag1;
            char        flag2;
        }raux2[MAX_AXIS];
    }modal;
} ODBMDL ;

/* cnc_diagnoss:read diagnosis data */
/* cnc_diagnosr:read diagnosis data(area specified) */
typedef struct odbdgn {
    short datano ;       /*  data number    */
    short type ;         /*  axis number    */
    union {
        char cdata ;  /*  diagnosis data */
        short idata ;
        long ldata ;
        char cdatas[MAX_AXIS] ;
        short idatas[MAX_AXIS] ;
        long ldatas[MAX_AXIS] ;
    } u ;
} ODBDGN ;               /*  In case that the number of axes is 10 */

/* cnc_adcnv:read A/D conversion data */
typedef struct odbad {
    short datano ;     /*  input analog voltage type */
    short type ;       /*  analog voltage type       */
    char  data ;       /*  digital voltage data      */
} ODBAD ;

/* cnc_rdopmsg:read operator's message */
typedef struct msg {
    short datano ;     /* operator's message number */
    short type ;       /* operator's message type   */
    short char_num ;   /* message string length   */
    char  data[129] ;  /* operator's message string */
} OPMSG ;               /* In case that the data length is 129 */

typedef struct opmsg3 {
    short datano ;     /* operator's message number */
    short type ;       /* operator's message type   */
    short char_num ;   /* message string length   */
    char  data[256] ;  /* operator's message string */
} OPMSG3 ;              /* In case that the data length is 256 */

/* cnc_sysconfig:read CNC configuration information */
typedef struct odbsysc {
    char        slot_no_p[16];
    char        slot_no_l[16];
    short       mod_id[16];
    short       soft_id[16];
    char        s_series[16][5];
    char        s_version[16][5];
    char        sys_id[16];
    char        sys_ser[5];
    char        sys_ver[5];
    char        basic_ver[5];
    char        opta1_ver[5];
    char        opta2_ver[5];
    char        opta3_ver[5];
    char        opta4_ver[5];
    char        sub_ver[5];
    char        opts1_ver[5];
    char        tcopt_ver[5];
    char        axis_ser[5];
    char        axis_ver[5];
    char        help_ser[5];
    char        help_ver[5];
    char        boot_ser[5];
    char        boot_ver[5];
    char        servo_ser[5];
    char        servo_ver[5];
    char        cmpl_ser[5];
    char        cmpl_ver[5];
    char        sral1_ser[5];
    char        sral1_ver[5];
    char        sral2_ser[5];
    char        sral2_ver[5];
    char        sral3_ser[5];
    char        sral3_ver[5];
    char        sral4_ser[5];
    char        sral4_ver[5];
    short       pcb_info[20];
    short       pcb_note[20][16];
} ODBSYSC ;

/* cnc_rdprstrinfo:read program restart information */
typedef struct odbprs {
    short    datano;            /* dummy                        */
    short    type;              /* dummy                        */
    short    data_info[5];      /* data setting information     */
    long     rstr_bc;           /* block counter                */
    long     rstr_m[35];        /* M code value                 */
    long     rstr_t[2]; /* T code value                 */
    long     rstr_s;            /* S code value                 */
    long     rstr_b;            /* B code value                 */
    long     dest[MAX_AXIS];    /* program re-start position    */
    long     dist[MAX_AXIS];            /* program re-start distance    */
} ODBPRS;

/* cnc_rdopnlsgnl:read output signal image of software operator's panel */
/* cnc_wropnlsgnl:write output signal of software operator's panel */
typedef struct iodbsgnl {
    short    datano;         /* dummy */
    short    type;           /* data select flag */
    short    mode;           /* mode signal */
    short    hndl_ax;        /* Manual handle feed axis selection signal */
    short    hndl_mv;        /* Manual handle feed travel distance selection signal */
    short    rpd_ovrd;       /* rapid traverse override signal */
    short    jog_ovrd;       /* manual feedrate override signal */
    short    feed_ovrd;      /* feedrate override signal */
    short    spdl_ovrd;      /* (not used) */
    short    blck_del;       /* optional block skip signal */
    short    sngl_blck;      /* single block signal */
    short    machn_lock;     /* machine lock signal */
    short    dry_run;        /* dry run signal */
    short    mem_prtct;      /* memory protection signal */
    short    feed_hold;      /* automatic operation halt signal */
    short    manual_rpd;     /* manual rapid traverse selection signal */
    short    dummy[2];       /* (reserve) */
} IODBSGNL;

/* cnc_rdopnlgnrl:read general signal image of software operator's panel */
/* cnc_wropnlgnrl:write general signal image of software operator's panel */
typedef struct iodbgnrl {
    short    datano;         /* dummy */
    short    type;           /* data select flag */
    char     sgnal;           /* general signal */
} IODBGNRL;

/* cnc_rdopnlgsname:read general signal name of software operator's panel */
/* cnc_wropnlgsname:write general signal name of software operator's panel*/
typedef struct iodbrdna {
    short    datano;         /* dummy */
    short    type;           /* data select flag */
    char     sgnl1_name[9];  /* general signal 1 name */
    char     sgnl2_name[9];  /* general signal 2 name */
    char     sgnl3_name[9];  /* general signal 3 name */
    char     sgnl4_name[9];  /* general signal 4 name */
    char     sgnl5_name[9];  /* general signal 5 name */
    char     sgnl6_name[9];  /* general signal 6 name */
    char     sgnl7_name[9];  /* general signal 7 name */
    char     sgnl8_name[9];  /* general signal 8 name */
} IODBRDNA;

/* cnc_getdtailerr:get detail error */
typedef struct odberr {
    short    err_no ;
    short    err_dtno ;
} ODBERR ;

/* cnc_rdparainfo:read informations of CNC parameter */
typedef struct odbparaif {
    unsigned short      info_no ;
    short               prev_no ;
    short               next_no ;
    struct {
        short           prm_no ;
        short           prm_type ;
    } info[10] ;        /* In case that the number of data is 10 */
} ODBPARAIF ;

/* cnc_rdsetinfo:read informations of CNC setting data */
typedef struct odbsetif {
    unsigned short      info_no ;
    short               prev_no ;
    short               next_no ;
    struct {
        short           set_no ;
        short           set_type ;
    } info[10] ;        /* In case that the number of data is 10 */
} ODBSETIF ;

/* cnc_rddiaginfo:read informations of CNC diagnose data */
typedef struct odbdiagif {
    unsigned short      info_no ;
    short               prev_no ;
    short               next_no ;
    struct {
        short           diag_no ;
        short           diag_type ;
    } info[10] ;        /* In case that the number of data is 10 */
} ODBDIAGIF ;

/* cnc_rdparanum:read maximum, minimum and total number of CNC parameter */
typedef struct odbparanum {
    unsigned short      para_min ;
    unsigned short      para_max ;
    unsigned short      total_no ;
} ODBPARANUM ;

/* cnc_rdsetnum:read maximum, minimum and total number of CNC setting data */
typedef struct odbsetnum {
    unsigned short      set_min ;
    unsigned short      set_max ;
    unsigned short      total_no ;
} ODBSETNUM ;

/* cnc_rddiagnum:read maximum, minimum and total number of CNC diagnose data */
typedef struct odbdiagnum {
    unsigned short      diag_min ;
    unsigned short      diag_max ;
    unsigned short      total_no ;
} ODBDIAGNUM ;

/* cnc_rdposerrs2:read the position deviation S1 and S2 */
typedef struct odbpser {
    long        poserr1 ;
    long        poserr2 ;
} ODBPSER ;

/* cnc_rdctrldi:read the control input signal */
typedef struct odbspdi {
    char        sgnl1 ;
    char        sgnl2 ;
    char        sgnl3 ;
    char        sgnl4 ;
} ODBSPDI ;

/* cnc_rdctrldo:read the control output signal */
typedef struct odbspdo {
    char        sgnl1 ;
    char        sgnl2 ;
    char        sgnl3 ;
    char        sgnl4 ;
} ODBSPDO ;

/* cnc_rdprogdir2:read program directory 2 */
typedef struct prgdir2 {
    short       number;
    long        length;
    char        comment[51];
    char        dummy;
} PRGDIR2;

/* cnc_rdmgrpdata:read M-code group data */
typedef struct odbmgrp {
    long        m_code;
    short       grp_no;
    char        m_name[21];
    char        dummy;
} ODBMGRP;

/* cnc_wrmgrpdata:write M-code group data */
typedef struct idbmgrp {
    short       s_no;
    short       dummy;
    short       num;
    short       group[10];
} IDBMGRP;              /* In case that the number of data is 10 */

/* cnc_rdexecmcode:read executing M-code group data */
typedef struct odbexem {
    short       grp_no;
    short       mem_no;
    struct{
        long    no;
        short   flag;
    }m_code[5];
    char        m_name[21];
    char        dummy;
} ODBEXEM;

/* cnc_rdrstrmcode:read program re-start M-code group data */
typedef struct odbrstrm {
    short       grp_no;
    short       mem_no;
    struct{
        long    no;
        short   flag;
    }m_code[5];
} ODBRSTRM;

/* cnc_rdproctime:read process time stamp data */
typedef struct odbptime {
    short       num;
    struct{
        long    prg_no;
        short   hour;
        char    minute;
        char    second;
    }data[10];
} ODBPTIME;

/* cnc_rdprgdirtime:read program directory with process time stamp */
typedef struct prgdirtm {
    long        prg_no;
    char        comment[51];
    char        cuttime[13];
} PRGDIRTM;

/* cnc_rdcomparam:read communication parameter for DNC1, DNC2 and OSI-Ethernet */
/* cnc_wrcomparam:write communication parameter for DNC1, DNC2 and OSI-Ethernet*/
typedef struct iodbcprm {
    char           NcApli[65];
    char           Dummy1;
    char           HostApli[65];
    char           Dummy2;
    unsigned long  StatPstv;
    unsigned long  StatNgtv;
    unsigned long  Statmask;
    unsigned long  AlarmStat;
    unsigned long  PsclHaddr;
    unsigned long  PsclLaddr;
    unsigned short SvcMode1;
    unsigned short SvcMode2;
    long           FileTout;
    long           RemTout;
} IODBCPRM;

/* cnc_dtsvrdpgdir:read directory in data server */
typedef struct odbdsdir {
    long    file_num;
    long    remainder;
    short   data_num;
    struct {
        char    file_name[16];
        char    comment[64];
        long    size;
        char    date[16];
    } data[32];
} ODBDSDIR;

/* cnc_dtsvrdset:read setting data for data server */
/* cnc_dtsvwrset:write setting data for data server */
typedef struct iodbdsset {
    char host_ip[16];
    char host_uname[32];
    char host_passwd[32];
    char host_dir[128];
    char dtsv_mac[13];
    char dtsv_ip[16];
    char dtsv_mask[16];
} IODBDSSET ;

/* cnc_dtsvmntinfo:read maintanance information for data server */
typedef struct odbdsmnt {
    long empty_cnt;
    long total_size;
    long read_ptr;
    long write_ptr;
} ODBDSMNT ;

/* cnc_rdbrstrinfo:read block re-start information */
typedef struct odbbrs {
    long dest[MAX_AXIS];
    long dist[MAX_AXIS];
} ODBBRS ;              /*  In case that the number of axes is 10 */

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

/* cnc_rdhpccset:read hpcc setting data */
/* cnc_wrhpccset:write hpcc setting data */
typedef struct iodbhpst {
    short       slct ;
    short       hpcc ;
    short       multi ;
    short       ovr1 ;
    short       ign_f ;
    short       foward ;
    long        max_f ;
    short       ovr2 ;
    short       ovr3 ;
    short       ovr4 ;
    long        reserve[7] ;
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

/* cnc_rdintchk:read interrupt check */
/* cnc_wrintchk:write interrupt check */
/* cnc_rdintinfo:read interrupt check information */
typedef struct  iodbint {
    short   datano_s;   /* start offset No. */
    short   type;               /* kind of position */
    short   datano_e;   /* end offset No. */
    long        data[24];       /* position value of area for not attach */
} IODBINT ;

/* cnc_rd3dtooltip:read tip of tool for 3D hundle */
/* cnc_rd3dmovrlap:read move overrlap of tool for 3D hundle */
typedef struct odb3dhdl {
        short   axes[5] ;
        long    data[5] ;
} ODB3DHDL ;

/* cnc_rd3dpulse:read pulse for 3D hundle */
typedef struct odb3dpls {
        long    right_angle_x ;
        long    right_angle_y ;
        long    tool_axis     ;
        long    tool_tip_a_b  ;
        long    tool_tip_c    ;
} ODB3DPLS ;

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
     short type_a ;     /*  PMC address type         */
     short type_d ;     /*  PMC data type            */
     short datano_s ;   /*  start PMC address        */
     short datano_e ;   /*  end PMC address          */
     union {
        char    cdata[5] ;      /* PMC data */
        short   idata[5] ;
        long    ldata[5] ;
     } u ;
} IODBPMC ;        /* In case that the number of data is 5 */

/* pmc_rdpmcinfo:read informations of PMC data */
typedef struct odbpmcinf {
    short               datano ;
    struct {
        char            pmc_adr ;
        char            adr_attr ;
        unsigned short  top_num ;
        unsigned short  last_num ;
    } info[64] ;
} ODBPMCINF ;

/* pmc_rdcntldata:read PMC parameter data table control data */
/* pmc_wrcntldata:write PMC parameter data table control data */
typedef struct iodbpmccntl {
    short               datano_s ;
    short               dummy ;
    short               datano_e ;
    struct {
        char            tbl_prm ;
        char            data_type ;
        unsigned short  data_size ;
        unsigned short  data_dsp ;
        short           dummy ;
    } info[100] ;
} IODBPMCCNTL ;

/* pmc_rdalmmsg:read PMC alarm message */
typedef struct  odbpmcalm {
    char    almmsg[128] ;    /* alarm message */
} ODBPMCALM ;

/* pmc_getdtailerr:get detail error for pmc */
typedef struct odbpmcerr {
    short    err_no ;
    short    err_dtno ;
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

/* end of downloading NC program */
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
 FWLIBAPI short WINAPI cnc_dnc( unsigned short, char *, short ) ;

/* download DNC program(conditional) */
 FWLIBAPI short WINAPI cnc_cdnc( unsigned short, char *, short ) ;

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

/* read custom macro variable 2 */
 FWLIBAPI short WINAPI cnc_rdmacro2( unsigned short, short, short, ODBM * ) ;

/* read custom macro variables(area specified) 2 */
 FWLIBAPI short WINAPI cnc_rdmacror2( unsigned short, short, short, short, IODBMR * ) ;


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


/*---------------------------------*/
/* CNC: Wave diagnoss data related */
/*---------------------------------*/

/* read wave diagnoss data parameter */
 FWLIBAPI short WINAPI cnc_rdwaveprm( unsigned short, IODBWAVE * );

/* write wave diagnoss data parameter */
 FWLIBAPI short WINAPI cnc_wrwaveprm( unsigned short, IODBWAVE * );

/* read wave diagnoss data parameter 2 */
 FWLIBAPI short WINAPI cnc_rdwaveprm2( unsigned short, IODBWVPRM * );

/* write wave diagnoss data parameter 2 */
 FWLIBAPI short WINAPI cnc_wrwaveprm2( unsigned short, IODBWVPRM * );

/* wave diagnoss data sampling start */
 FWLIBAPI short WINAPI cnc_wavestart( unsigned short ) ;

/* wave diagnoss data sampling stop */
 FWLIBAPI short WINAPI cnc_wavestop( unsigned short ) ;

/* wave diagnoss data sampling status */
 FWLIBAPI short WINAPI cnc_wavestat( unsigned short, short * ) ;

/* read wave diagnoss data */
 FWLIBAPI short WINAPI cnc_rdwavedata( unsigned short, short, short, long, long *, ODBWVDT * ) ;


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

/* read operator's message 3 */
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
/* search sequence number for program restart 2 */
 FWLIBAPI short WINAPI cnc_rstrseqsrch2( unsigned short, long, long, short, short, long ) ;     // t.m

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

/* read data from the FANUC BUS */
 FWLIBAPI short WINAPI cnc_rdfbusmem( unsigned short, short, short, long, long, void * ) ;

/* write data to the FANUC BUS */
 FWLIBAPI short WINAPI cnc_wrfbusmem( unsigned short, short, short, long, long, void * ) ;

/* read program directory 2 data */
 FWLIBAPI short WINAPI cnc_rdprogdir2( unsigned short, short, short *, short *, PRGDIR2 * );

/* read M-code group data */
 FWLIBAPI short WINAPI cnc_rdmgrpdata( unsigned short, short, short *, ODBMGRP * );

/* write M-code group data */
 FWLIBAPI short WINAPI cnc_wrmgrpdata( unsigned short, IDBMGRP * );

/* read executing M-code group data */
 FWLIBAPI short WINAPI cnc_rdexecmcode( unsigned short, short, short *, ODBEXEM * );

/* read program re-start M-code group data */
 FWLIBAPI short WINAPI cnc_rdrstrmcode( unsigned short, short, short *, ODBRSTRM * );

/* read process time stamp data */
 FWLIBAPI short WINAPI cnc_rdproctime( unsigned short, ODBPTIME * );

/* read program directory with process time stamp */
 FWLIBAPI short WINAPI cnc_rdprgdirtime( unsigned short, long *, short *, PRGDIRTM * );

/* read DNC file name for DNC1, DNC2 and OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rddncfname( unsigned short, char * ) ;

/* write DNC file name for DNC1, DNC2 and OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_wrdncfname( unsigned short, char * ) ;

/* read communication parameter for DNC1, DNC2 and OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rdcomparam( unsigned short, IODBCPRM * ) ;

/* write communication parameter for DNC1, DNC2 and OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_wrcomparam( unsigned short, IODBCPRM * ) ;

/* read log message for DNC2 */
 FWLIBAPI short WINAPI cnc_rdcomlogmsg( unsigned short, char * ) ;

/* read operater message for DNC1 and DNC2 */
 FWLIBAPI short WINAPI cnc_rdcomopemsg( unsigned short, char * ) ;

/* read receive message for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rdrcvmsg( unsigned short, char * ) ;

/* read send message for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rdsndmsg( unsigned short, char * ) ;

/* sending message for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_sendmessage( unsigned short, char * ) ;

/* clear message buffer for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_clrmsgbuff( unsigned short, short ) ;

/* read receive message status for OSI-Ethernet */
 FWLIBAPI short WINAPI cnc_rdrcvstat( unsigned short, unsigned short * ) ;

/* transfer file from host to data server */
 FWLIBAPI short WINAPI cnc_dtsvftpget( unsigned short, char *, char * ) ;

/* transfer file from data server to host */
 FWLIBAPI short WINAPI cnc_dtsvftpput( unsigned short, char *, char * ) ;

/* read transfer file status both host and data server */
 FWLIBAPI short WINAPI cnc_dtsvftpstat( unsigned short ) ;

/* read directory in data server */
 FWLIBAPI short WINAPI cnc_dtsvrdpgdir( unsigned short, char *, short, ODBDSDIR * ) ;

/* delete file in data server */
 FWLIBAPI short WINAPI cnc_dtsvdelete( unsigned short, char * ) ;

/* download from data server to cnc */
 FWLIBAPI short WINAPI cnc_dtsvdownload( unsigned short, char * ) ;

/* upload from cnc to data server */
 FWLIBAPI short WINAPI cnc_dtsvupload( unsigned short, char * ) ;

/* cancel upload/download both data server and cnc */
 FWLIBAPI short WINAPI cnc_dtsvcnclupdn( unsigned short ) ;

/* read upload/download status both data server and cnc */
 FWLIBAPI short WINAPI cnc_dtsvupdnstat( unsigned short ) ;

/* read file name for data server dnc */
 FWLIBAPI short WINAPI cnc_dtsvgetdncpg( unsigned short, char * ) ;

/* write file name for data server dnc */
 FWLIBAPI short WINAPI cnc_dtsvsetdncpg( unsigned short, char * ) ;

/* read setting data for data server */
 FWLIBAPI short WINAPI cnc_dtsvrdset( unsigned short, IODBDSSET * ) ;

/* write setting data for data server */
 FWLIBAPI short WINAPI cnc_dtsvwrset( unsigned short, IODBDSSET * ) ;

/* check hard disk for data server */
 FWLIBAPI short WINAPI cnc_dtsvchkdsk( unsigned short ) ;

/* format hard disk for data server */
 FWLIBAPI short WINAPI cnc_dtsvhdformat( unsigned short ) ;

/* write interface area for data server */
 FWLIBAPI short WINAPI cnc_dtsvsavecram( unsigned short ) ;

/* read  interface area for data server */
 FWLIBAPI short WINAPI cnc_dtsvrdcram( unsigned short, long, long *, char * ) ;

/* read maintanance information for data server */
 FWLIBAPI short WINAPI cnc_dtsvmntinfo( unsigned short, ODBDSMNT * ) ;

/* read data server mode */
 FWLIBAPI short WINAPI cnc_dtsvgetmode( unsigned short, short * ) ;

/* change data server mode */
 FWLIBAPI short WINAPI cnc_dtsvsetmode( unsigned short, short ) ;

/* read error message for data server */
 FWLIBAPI short WINAPI cnc_dtsvrderrmsg( unsigned short, short, char * ) ;

/* read block re-start information */
 FWLIBAPI short WINAPI cnc_rdbrstrinfo( unsigned short, ODBBRS * ) ;

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

/* read interrupt check */
 FWLIBAPI short WINAPI cnc_rdintchk( unsigned short, short, short, short, short, IODBINT * ) ;

/* write interrupt check */
 FWLIBAPI short WINAPI cnc_wrintchk( unsigned short, short, IODBINT * ) ;

/* read interrupt check information */
 FWLIBAPI short WINAPI cnc_rdintinfo( unsigned short, short * ) ;

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

/* get date and time from cnc */
 FWLIBAPI short WINAPI cnc_gettimer( unsigned short, IODBTIMER * ) ;

/* set date and time for cnc */
 FWLIBAPI short WINAPI cnc_settimer( unsigned short, IODBTIMER * ) ;


/*----------------------------*/
/* CNC : NC display function  */
/*----------------------------*/

/* Start NC display */
FWLIBAPI short WINAPI cnc_startnccmd( unsigned short ) ;

/* Start NC display (2) */
FWLIBAPI short WINAPI cnc_startnccmd2( unsigned short, char * ) ;

/* Stop NC display */
FWLIBAPI short WINAPI cnc_stopnccmd( unsigned short ) ;

/* Get NC display mode */
FWLIBAPI short WINAPI cnc_getdspmode( unsigned short, short * ) ;


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

/* read data from extended backup memory */
 FWLIBAPI short WINAPI pmc_rdkpm( unsigned short, unsigned long, char *, unsigned short ) ;

/* write data to extended backup memory */
 FWLIBAPI short WINAPI pmc_wrkpm( unsigned short, unsigned long, char *, unsigned short ) ;

/* read maximum size of extended backup memory */
 FWLIBAPI short WINAPI pmc_kpmsiz( unsigned short, unsigned long * ) ;

/* read informations of PMC data */
 FWLIBAPI short WINAPI pmc_rdpmcinfo( unsigned short, short, ODBPMCINF * ) ;

/* read PMC parameter data table contorol data */
 FWLIBAPI short WINAPI pmc_rdcntldata( unsigned short, short, short, short, IODBPMCCNTL * ) ;   //add t.m

/* write PMC parameter data table contorol data */
 FWLIBAPI short WINAPI pmc_wrcntldata( unsigned short, short, IODBPMCCNTL * ) ;         //add t.m

/* read PMC parameter data table contorol data group number */
 FWLIBAPI short WINAPI pmc_rdcntlgrp( unsigned short, short * ) ;                                 //add t.m

/* write PMC parameter data table contorol data group number */
 FWLIBAPI short WINAPI pmc_wrcntlgrp( unsigned short, short ) ;                                   //add t.m

/* read PMC alarm message */
 FWLIBAPI short WINAPI pmc_rdalmmsg( unsigned short, short, short *, short *, ODBPMCALM * ) ;

/* get detail error for pmc */
 FWLIBAPI short WINAPI pmc_getdtailerr( unsigned short, ODBPMCERR * ) ;                 //add t.m

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
