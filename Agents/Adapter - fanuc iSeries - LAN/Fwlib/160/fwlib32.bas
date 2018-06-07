Attribute VB_Name = "Module1"
'------------------------------------------------------
'
' CNC/PMC Data Window Library for series 160/180/210
'
' Copyright (C) 1996 by FANUC LTD. All rights reserved.
'
'------------------------------------------------------


'----------------------
'  Axis Define
'----------------------
Global Const MAX_AXIS = 8
Global Const ALL_AXES = -1

'----------------------
'  Error Codes
'----------------------
Global Const EW_NODLL = -15             ' DLL not exist error
Global Const EW_BUS = -11               ' bus error
Global Const EW_SYSTEM2 = -10           ' system error
Global Const EW_HSSB = -9               ' hssb communication error
Global Const EW_HANDLE = -8             ' Windows library handle error
Global Const EW_VERSION = -7            ' CNC/PMC version missmatch
Global Const EW_UNEXP = -6              ' abnormal error
Global Const EW_SYSTEM = -5             ' system error
Global Const EW_PARITY = -4             ' shared RAM parity error
Global Const EW_MMCSYS = -3             ' emm386 or mmcsys install error
Global Const EW_RESET = -2              ' reset or stop occured error
Global Const EW_BUSY = -1               ' busy error
Global Const EW_OK = 0                  ' no problem
Global Const EW_FUNC = 1                ' command prepare error
Global Const EW_NOPMC = 1               ' pmc not exist
Global Const EW_LENGTH = 2              ' data block length error
Global Const EW_NUMBER = 3              ' data number error
Global Const EW_RANGE = 3               ' address range error
Global Const EW_ATTRIB = 4              ' data attribute error
Global Const EW_TYPE = 4                ' data type error
Global Const EW_DATA = 5                ' data error
Global Const EW_NOOPT = 6               ' no option error
Global Const EW_PROT = 7                ' write protect error
Global Const EW_OVRFLOW = 8             ' memory overflow error
Global Const EW_PARAM = 9               ' cnc parameter not correct error
Global Const EW_BUFFER = 10             ' buffer error
Global Const EW_PATH = 11               ' path error
Global Const EW_MODE = 12               ' cnc mode error
Global Const EW_REJECT = 13             ' execution rejected error
Global Const EW_DTSRVR = 14             ' data server error
Global Const EW_ALARM = 15              ' alarm has been occurred
Global Const EW_STOP = 16               ' CNC is not running
Global Const EW_PASSWD = 17             ' protection data error

'---------------------------------
'  Result codes of DNC operation
'---------------------------------
Global Const DNC_NORMAL = -1            ' normal completed
Global Const DNC_CANCEL = -32768        ' DNC operation was canceled by CNC
Global Const DNC_OPENERR = -514         ' file open error
Global Const DNC_NOFILE = -516          ' file not found
Global Const DNC_READERR = -517         ' read error


'----------------------
'
'  Structure Template
'
'----------------------
'-------------------------------------
' CNC: Control axis / spindle related
'-------------------------------------

' cnc_actf:read actual axis feedrate(F)
' cnc_acts:read actual spindle speed(S)
Type ODBACT
  nDummy(0 To 1) As Integer     ' dummy
  lData As Long                 ' actual feed / actual spindle
End Type

' cnc_acts2:read actual spindle speed(S)
' (All or specified)
Type ODBACT2
  nDatano As Integer            ' spindle number
  nType As Integer              ' dummy
  lData(0 To 3) As Long         ' spindle data
End Type

' cnc_absolute:read absolute axis position
' cnc_machine:read machine axis position
' cnc_relative:read relative axis position
' cnc_distance:read distance to go
' cnc_skip:read skip position
' cnc_srvdelay:read servo delay value
' cnc_accdecdly:read acceleration/deceleration delay value
' cnc_absolute2:read absolute axis position 2
' cnc_relative2:read relative axis position 2
Type ODBAXIS
  nDummy As Integer             ' dummy
  nType As Integer              ' axis number
  lData(0 To 7) As Long         ' data value
End Type                        ' In case that the number of axes is 8

' cnc_rddynamic:read all dynamic data
#If ONO8D Then

Type ODBDY1
  nDummy As Integer
  nAxis As Integer              ' axis number
  nAlarm As Integer             ' alarm status
  lPrgnum As Long               ' current program number
  lPrgmnum As Long              ' main program number
  lSeqnum As Long               ' current sequence number
  lActf As Long                 ' actual feedrate
  lActs As Long                 ' actual spindle speed
  lAbsolute As Long             ' absolute position
  lMachine As Long              ' machine position
  lRelative As Long             ' relative position
  lDistance As Long             ' distance to go
End Type                        ' In case of no axis

Type ODBDY2
  nDummy As Integer
  nAxis As Integer              ' axis number
  nAlarm As Integer             ' alarm status
  lPrgnum As Long               ' current program number
  lPrgmnum As Long              ' main program number
  lSeqnum As Long               ' current sequence number
  lActf As Long                 ' actual feedrate
  lActs As Long                 ' actual spindle speed
  lAbsolute(0 To 7) As Long     ' absolute position
  lMachine(0 To 7) As Long      ' machine position
  lRelative(0 To 7) As Long     ' relative position
  lDistance(0 To 7) As Long     ' distance to go
End Type                        ' In case that the number of axes is 8

#Else

Type ODBDY1
  nDummy As Integer
  nAxis As Integer              ' axis number
  nAlarm As Integer             ' alarm status
  nPrgnum As Integer            ' current program number
  nPrgmnum As Integer           ' main program number
  lSeqnum As Long               ' current sequence number
  lActf As Long                 ' actual feedrate
  lActs As Long                 ' actual spindle speed
  lAbsolute As Long             ' absolute position
  lMachine As Long              ' machine position
  lRelative As Long             ' relative position
  lDistance As Long             ' distance to go
End Type                        ' In case of no axis

Type ODBDY2
  nDummy As Integer
  nAxis As Integer              ' axis number
  nAlarm As Integer             ' alarm status
  nPrgnum As Integer            ' current program number
  nPrgmnum As Integer           ' main program number
  lSeqnum As Long               ' current sequence number
  lActf As Long                 ' actual feedrate
  lActs As Long                 ' actual spindle speed
  lAbsolute(0 To 7) As Long     ' absolute position
  lMachine(0 To 7) As Long      ' machine position
  lRelative(0 To 7) As Long     ' relative position
  lDistance(0 To 7) As Long     ' distance to go
End Type                        ' In case that the number of axes is 8

#End If

' cnc_wrrelpos:set origin / preset relative axis position
Type IDBWRR
  nDatano As Integer            ' dummy
  nType As Integer              ' axis number
  lData(0 To 7) As Long         ' preset data
End Type

' cnc_prstwkcd:preset work coordinate
Type IDBWRA
  nDatano As Integer            ' dummy
  nType As Integer              ' axis number
  lData(0 To 7) As Long         ' preset data
End Type

' cnc_rdmovrlap:read manual overlapped motion value
Type IODBOVL
  nDatano As Integer            ' dummy
  nType As Integer              ' axis number
  lData(0 To 1, 0 To 7) As Long ' data value
End Type

' cnc_rdspload:read load information of serial spindle
' cnc_rdspmaxrpm:read maximum r.p.m. ratio of serial spindle
' cnc_rdspgear:read gear ratio of serial spindle
Type ODBSPN
  nDatano As Integer            ' spindle number
  nType As Integer              ' dummy
  nData(0 To 3) As Integer      ' spindle data
End Type

' cnc_rdposition:read tool position
Type POSELM
  nData As Long                 ' position data
  nDec As Integer               ' decimal position
  nUnit As Integer              ' data unit
  nDisp As Integer              ' display flag
  sName As String * 1           ' axis name
  sSuff As String * 1           ' suffix
End Type

Type ODBPOS
  abs As POSELM                 ' absolute position
  mach As POSELM                ' machine position
  rel As POSELM                 ' relative position
  dist As POSELM                ' distance position
End Type

' cnc_rdspeed:read current speed
Type SPEEDELM
  nData As Long                 ' speed data
  nDec As Integer               ' decimal position
  nUnit As Integer              ' data unit
  nDisp As Integer              ' display flag
  sName As String * 1           ' name
  sSuff As String * 1           ' suffix
End Type

Type ODBSPEED
    actf As SPEEDELM            ' actual feed rate
    acts As SPEEDELM            ' actual spindle speed
End Type

' cnc_rdsvmeter:read servo load meter
' cnc_rdspmeter:read spindle load meter
Type LOADELM
  nData As Long                 ' speed data
  nDec As Integer               ' decimal position
  nUnit As Integer              ' data unit
  sName As String * 1           ' axis name
  sSuff1 As String * 1          ' suffix
  sSuff2 As String * 1          ' suffix
  sReserve As String * 1        ' reserve
End Type

Type ODBSVLOAD
  svload As LOADELM             ' servo load meter
End Type

Type ODBSPLOAD
  spload As LOADELM             ' spindle load meter
  spspeed As LOADELM            ' spindle speed
End Type

' cnc_rdspcss:constantly spindle speed
Type ODBCSS
  lSrpm As Long                 ' order spindle speed
  lSspm As Long                 ' order constant spindle speed
  lSmax As Long                 ' order maximum spindle speed
End Type

' cnc_rdexecpt:read execution program pointer
Type PRGPNT
  lProg_no As Long              ' program number
  lBlk_no As Long               ' block number
End Type

' cnc_rdhsprminfo:read the information for function cnc_rdhsparam()
Type HSPINFO
    sPrminfo(0 To 7) As String * 16
End Type

' cnc_rdhsparam:read parameters at the high speed
Type HSPDATA1
    sCdata(0 To 7) As Byte
End Type

Type HSPDATA2
    nIdata(0 To 7) As Integer
End Type

Type HSPDATA3
    lLdata(0 To 7) As Long
End Type


'----------------------
' CNC: Program related
'----------------------

' cnc_rddncdgndt:read the diagnosis data of DNC operation
Type ODBDNCDGN
  nCtrl_word As Integer
  nCan_word As Integer
  sNc_file As String * 16
  nRead_ptr As Integer
  nWrite_ptr As Integer
  nEmpty_cnt As Integer
  nTotal_size As Long
End Type

' cnc_upload:upload NC program
' cnc_cupload:upload NC program(conditional)
Type ODBUP
  nDummy(0 To 1) As Integer     ' dummy
  sData As String * 256         ' data
End Type                        ' In case that the number of data is 256

' cnc_buff:read buffer status for downloading/verification NC program
Type ODBBUF
  nDummy(0 To 1) As Integer     ' dummy
  nData As Integer              ' buffer status
End Type

' cnc_rdprogdir:read program directory
Type PRGDIR
  sPrg_data As String * 256     ' directory data
End Type                        ' In case that the number of data is 256

' cnc_rdproginfo:read program information
Type ODBNC1
  nReg_prg As Integer           ' registered program number
  nUnreg_prg As Integer         ' unregistered program number
  lUsed_mem As Long             ' used memory area
  lUnused_mem As Long           ' unused memory area
End Type

Type ODBNC2
  sAsc As String * 31           ' ASCII string type
End Type

' cnc_rdprgnum:read program number under execution
#If ONO8D Then

Type ODBPRO
  nDummy(0 To 1) As Integer     ' dummy
  lData As Long                 ' running program number
  lMdata As Long                ' main program number
End Type

#Else

Type ODBPRO
  nDummy(0 To 1) As Integer     ' dummy
  nData As Integer              ' running program number
  nMdata As Integer             ' main program number
End Type

#End If

' cnc_rdseqnum:read sequence number under execution
Type ODBSEQ
  nDummy(0 To 1) As Integer     ' dummy
  lData As Long                 ' sequence number
End Type

' cnc_rdmdipntr:read execution pointer for MDI operation
#If ONO8D Then

Type ODBMDIP
  lMdiprog As Long              ' exec. program number
  lMdipntr As Long              ' exec. pointer
  lCrntprog As Long             ' prepare program number
  lCrntpntr As Long             ' prepare pointer
End Type

#Else

Type ODBMDIP
  nMdiprog As Integer           ' exec. program number
  lMdipntr As Long              ' exec. pointer
  nCrntprog As Integer          ' prepare program number
  lCrntpntr As Long             ' prepare pointer
End Type

#End If


'---------------------------
' CNC: NC file data related
'---------------------------

' cnc_rdtofs:read tool offset value
Type ODBTOFS
  nDatano As Integer            ' data number
  nType As Integer              ' data type
  lData As Long                 ' data
End Type

' cnc_rdtofsr:read tool offset value(area specified)
' cnc_wrtofsr:write tool offset value(area specified)
Type IODBTO1
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' offset type
  nDatano_e As Integer          ' end offset number
  dummy As Integer
  lM_ofs(0 To 4) As Long        ' M Each
End Type                        ' In case that the number of data is 5

Type IODBTO2
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' offset type
  nDatano_e As Integer          ' end offset number
  dummy As Integer
  lM_ofs_a(0 To 4) As Long      ' M-A All
End Type                        ' In case that the number of data is 5

Type IODBTO3
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' offset type
  nDatano_e As Integer          ' end offset number
  dummy As Integer
  lM_ofs_b(0 To 9) As Long      ' M-B All
End Type                        ' In case that the number of data is 5

Type IODBTO4
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' offset type
  nDatano_e As Integer          ' end offset number
  dummy As Integer
  lM_ofs_c(0 To 19) As Long     ' M-C All
End Type                        ' In case that the number of data is 5

Type IODBTO5
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' offset type
  nDatano_e As Integer          ' end offset number
  dummy As Integer
  nT_tip(0 To 4) As Integer     ' T Each, 2-byte
End Type                        ' In case that the number of data is 5

Type IODBTO6
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' offset type
  nDatano_e As Integer          ' end offset number
  dummy As Integer
  lT_ofs(0 To 4) As Long        ' T Each, 4-byte
End Type                        ' In case that the number of data is 5

Type IODBTOdata1
  nTip As Integer
  lData(0 To 3) As Long
End Type

Type IODBTO7
  nDatano_s As Integer                  ' start offset number
  nType As Integer                      ' offset type
  nDatano_e As Integer                  ' end offset number
  dummy As Integer
  nT_ofs_a(0 To 4) As IODBTOdata1       ' T-A All
End Type                                ' In case that the number of data is 5

Type IODBTOdata2
  nTip As Integer
  lData(0 To 7) As Long
End Type

Type IODBTO8
  nDatano_s As Integer                  ' start offset number
  nType As Integer                      ' offset type
  nDatano_e As Integer                  ' end offset number
  dummy As Integer
  nT_ofs_b(0 To 4) As IODBTOdata2       ' T-B All
End Type                                ' In case that the number of data is 5

' cnc_rdzofs:read work zero offset value
' cnc_wrzofs:write work zero offset value
Type IODBZOFS
  nDatano As Integer            ' offset NO.
  nType As Integer              ' axis number
  lData(0 To 7) As Long         ' data value
End Type                        ' In case that the number of axes is 8

' cnc_rdzofsr:read work zero offset value(area specified)
' cnc_wrzofsr:write work zero offset value(area specified)
Type IODBZOR
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' axis number
  nDatano_e As Integer          ' end offset number
  lData(0 To 55) As Long        ' offset value
End Type                        ' In case that the number of axes is 8,
                                ' the number of data is 7

' cnc_rdparam:read parameter
' cnc_wrparam:write parameter
' cnc_rdset:read setting data
' cnc_wrset:write setting data
' cnc_rdparar:read parameter(area specified)
' cnc_wrparas:write parameter(area specified)
' cnc_rdsetr:read setting data(area specified)
' cnc_wrsets:write setting data(area specified)
Type IODBPSD1
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  sCdata As Byte
End Type

Type IODBPSD2
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  nIdata As Integer
End Type

Type IODBPSD3
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  lLdata As Long
End Type

Type IODBPSD4
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  sCdatas(0 To 7) As Byte
End Type                        ' In case that the number of axes is 8

Type IODBPSD5
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  nIdatas(0 To 7) As Integer
End Type                        ' In case that the number of axes is 8

Type IODBPSD6
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  lLdatas(0 To 7) As Long
End Type                        ' In case that the number of axes is 8

' cnc_rdpitchr:read pitch error compensation data(area specified)
' cnc_wrpitchr:write pitch error compensation data(area specified)
Type IODBPI
  nDatano_s As Integer          ' start pitch number
  nDummy As Integer             ' dummy
  nDatano_e As Integer          ' end pitch number
  sData(0 To 4) As Byte         ' offset value
End Type                        ' In case that the number of data is 5

' cnc_rdmacro:read custom macro variable
Type ODBM
  nDatano As Integer            ' variable number
  nDummy As Integer             ' dummy
  lMcr_val As Long              ' macro variable
  nDec_val As Integer           ' decimal point
End Type

' cnc_rdmacror:read custom macro variables(area specified)
' cnc_wrmacror:write custom macro variables(area specified)
Type IODBMRdata
  lMcr_val As Long              ' macro variable
  nDec_val As Integer           ' decimal point
End Type

Type IODBMR
  nDatano_s As Integer          ' start macro number
  nDummy As Integer             ' dummy
  nDatano_e As Integer          ' end macro number
  nData(0 To 4) As IODBMRdata
End Type                        ' In case that the number of data is 5

' cnc_rdpmacro:read P code macro variable
Type ODBPM
  lDatano As Long               ' variable number
  nDummy As Integer             ' dummy
  lMcr_val As Long              ' macro variable
  nDec_val As Integer           ' decimal point
End Type

' cnc_rdpmacror:read P code macro variables(area specified)
' cnc_wrpmacror:write P code macro variables(area specified)
Type IODBPRdata
  lMcr_val As Long              ' macro variable
  nDec_val As Integer           ' decimal point
End Type

Type IODBPR
  lDatano_s As Long             ' start macro number
  nDummy As Integer             ' dummy
  lDatano_e As Long             ' end macro number
  nData(0 To 4) As IODBPRdata
End Type                        ' In case that the number of data is 5

' cnc_rdtofsinfo:read tool offset information
Type ODBTLINF
  nOfs_type As Integer
  nUse_no As Integer
End Type

' cnc_rdmacroinfo:read custom macro variable information
Type ODBMVINF
  nUse_no1 As Integer
  nUse_no2 As Integer
End Type

' cnc_rdpmacroinfo:read P code macro variable information
Type ODBPMINF
  nUse_no1 As Integer
  nUse_no2 As Integer
  nV2_type As Integer
End Type


' cnc_rdfixoffs:read rotary table dynamic fixture offset
Type ODBFOFS
  lMcrval As Long
  nDecval As Integer
End Type


'----------------------------------------
' CNC: Tool life management data related
'----------------------------------------

' cnc_rdgrpid:read tool life management data(tool group number)
Type ODBTLIFE1
  nDummy As Integer             ' dummy
  nType As Integer              ' data type
  lData As Long                 ' data
End Type

' cnc_rdngrp:read tool life management data(number of tool groups)
Type ODBTLIFE2
  nDummy(0 To 1) As Integer     ' dummy
  lData As Long                 ' data
End Type

' cnc_rdntool:read tool life management data(number of tools)
' cnc_rdlife:read tool life management data(tool life)
' cnc_rdcount:read tool life management data(tool lift counter)
Type ODBTLIFE3
  nDatano As Integer            ' data number
  nDummy As Integer             ' dummy
  lData As Long                 ' data
End Type

' cnc_rd1length:read tool life management data(tool length number-1)
' cnc_rd2length:read tool life management data(tool length number-2)
' cnc_rd1radius:read tool life management data(cutter compensation no.-1)
' cnc_rd2radius:read tool life management data(cutter compensation no.-2)
' cnc_t1info:read tool life management data(tool information-1)
' cnc_t2info:read tool life management data(tool information-2)
' cnc_toolnum:read tool life management data(tool number)
Type ODBTLIFE4
  nDatano As Integer            ' data number
  nType As Integer              ' data type
  lData As Long                 ' data
End Type

' cnc_rdgrpid2:read tool life management data(tool group number) 2
Type ODBTLIFE5
  lDummy As Long                ' dummy
  lType As Long                 ' data type
  lData As Long                 ' data
End Type

' cnc_rdtoolrng:read tool life management data(tool number, tool life,
'                                      tool life counter)(area specified)
Type IODBTRdata
  lNtool As Long                ' tool number
  lLife As Long                 ' tool life
  lCount As Long                ' tool life counter
End Type

Type IODBTR
  nDatano_s As Integer          ' start group number
  nDummy As Integer             ' dummy
  nDatano_e As Integer          ' end group number
  nData(0 To 4) As IODBTRdata
End Type                        ' In case that the number of data is 5

' cnc_rdtoolgrp:read tool life management data(all data within group)
Type ODBTGdata
  lTuse_num As Long             ' tool number
  lTool_num As Long             ' tool life
  lLength_num As Long           ' tool life counter
  lRadius_num As Long           ' tool life counter
  lTinfo As Long                ' tool life counter
End Type

Type ODBTG
  nGrp_num As Integer           ' start group number
  nDummy(0 To 1) As Integer     ' dummy
  lNtool As Long                ' tool number
  lLife As Long                 ' tool life
  lCount As Long                ' tool life counter
  nData(0 To 4) As ODBTGdata
End Type                        ' In case that the number of data is 5

' cnc_wrcountr:write tool life management data(tool life counter)
'                                                         (area specified)
Type IDBWRCdata
  lDummy(0 To 1) As Long        ' dummy
  lCount As Long                ' tool life counter
End Type

Type IDBWRC
  nDatano_s As Integer          ' start group number
  nDummy As Integer             ' dummy
  nDatano_e As Integer          ' end group number
  nData(0 To 4) As IDBWRCdata
End Type                        ' In case that the number of data is 5

' cnc_rdusegrpid:read tool life management data(used tool group number)
Type ODBUSEGR
  nDatano As Integer            ' dummy
  nType As Integer              ' dummy
  lNext As Long                 ' next use group number
  lUse As Long                  ' using group number
  lSlct As Long                 ' selecting group number
End Type

' cnc_rdmaxgrp:read tool life management data(max. number of tool groups)
' cnc_rdmaxtool:read tool life management data
'                                       (maximum number of tool within group)
Type ODBLFNO
  nDatano As Integer            ' dummy
  nType As Integer              ' dummy
  nData As Integer              ' number of data
End Type

' cnc_rdusetlno:read tool life management data(used tool no within group)
Type ODBTLUSE
  nS_grp As Integer             ' start group number
  nDummy As Integer             ' dummy
  nE_grp As Integer             ' end group number
  lData(0 To 4) As Long         ' tool using number
End Type                        ' In case that the number of group is 5

' cnc_rd1tlifedata:read tool life management data(tool data1)
' cnc_rd2tlifedata:read tool life management data(tool data2)
' cnc_wr1tlifedata:write tool life management data(tool data1)
' cnc_wr2tlifedata:write tool life management data(tool data2)
Type IODBTD
  nDatano As Integer            ' tool group number
  nType As Integer              ' tool using number
  lTool_num As Long             ' tool number
  lH_code As Long               ' H code
  lD_code As Long               ' D code
  lTool_inf As Long             ' tool information
End Type

' cnc_rd1tlifedat2:read tool life management data(tool data1) 2
' cnc_wr1tlifedat2:write tool life management data(tool data1) 2
Type IODBTD2
  nDatano As Integer            ' tool group number
  nDummy As Integer             ' dummy
  lType As Long                 ' tool using number
  lTool_num As Long             ' tool number
  lH_code As Long               ' H code
  lD_code As Long               ' D code
  lTool_inf As Long             ' tool information
End Type

' cnc_rdgrpinfo:read tool life management data(tool group information)
' cnc_wrgrpinfo:write tool life management data(tool group information)
Type IODBTGIdata
  lN_tool As Long               ' number of tool
  lCount_value As Long          ' tool life
  lCounter As Long              ' tool life counter
  lCount_type As Long           ' tool life counter type
End Type

Type IODBTGI
  nS_grp As Integer             ' start group number
  nDummy As Integer             ' dummy
  nE_grp As Integer             ' end group number
  nData(0 To 4) As IODBTGIdata
End Type                        ' In case that the number of group is 5

Type IODBTGI2
  nS_grp As Integer             ' start group number
  nDummy As Integer             ' dummy
  nE_grp As Integer             ' end group number
  nOpt_grpno(0 To 4) As Long    ' optional group number of tool
End Type                        ' In case that the number of group is 5

Type IODBTGI3
  nS_grp As Integer             ' start group number
  nDummy As Integer             ' dummy
  nE_grp As Integer             ' end group number
  nLife_rest(0 To 4) As Long    ' tool life rest count
End Type                        ' In case that the number of group is 5

' cnc_instlifedt:insert tool life management data(tool data)
Type IDBITD
  nDatano As Integer            ' tool group number
  nType As Integer              ' tool using number
  lData As Long                 ' tool number
End Type

' cnc_rdgrpinfo4:read tool life management data
Type IODBTGI4
  nGrp_no As Integer            ' tool group number
  lN_tool As Long               ' number of tool
  lCount_value As Long          ' tool life
  lCounter As Long              ' tool life counter
  lCount_type As Long           ' tool life counter type
  lOpt_grpno As Long            ' optional group number of tool
  lLife_rest As Long            ' tool life rest count
End Type

' cnc_rdexchgtgrp:read tool life management data(exchange tool group number)
Type ODBEXGP
  lGrp_no  As Long              ' group number
  lOpt_grpno As Long            ' optional group number
End Type


'-----------------------------------
' CNC: Tool management data related
'-----------------------------------
' cnc_regtool:new registration of tool management data
' cnc_rdtool:lead of tool management data
' cnc_wrtool:write of tool management data
Type IODBTLMNG
    lT_code As Long
    lLife_count As Long
    lMax_Life As Long
    lRest_Life As Long
    sLife_stat As Byte
    sCust_bits As Byte
    nTool_info As Integer
    nH_code As Integer
    nD_code As Integer
    lSpindle_speed As Long
    lFeedrate As Long
    nMagazine As Integer
    nPot As Integer
    nG_code As Integer
    nW_code As Integer
    lReserved(0 To 4) As Long
    lCustom1 As Long
    lCustom2 As Long
    lCustom3 As Long
    lCustom4 As Long
    lCustom5 As Long
    lCustom6 As Long
    lCustom7 As Long
    lCustom8 As Long
    lCustom9 As Long
    lCustom10 As Long
    lCustom11 As Long
    lCustom12 As Long
    lCustom13 As Long
    lCustom14 As Long
    lCustom15 As Long
    lCustom16 As Long
    lCustom17 As Long
    lCustom18 As Long
    lCustom19 As Long
    lCustom20 As Long
End Type

' cnc_wrtool2:write of individual data of tool management data
Type IDBTLM1
    nData_id As Long
    sData1 As Byte
End Type

Type IDBTLM2
    nData_id As Long
    nData2 As Integer
End Type

Type IDBTLM3
    nData_id As Long
    lData3 As Long
End Type

' cnc_regmagazine:new registration of magazine management data
' cnc_rdmagazine:lead of magazine management data
Type IODBTLMAG
    nMagazine As Integer
    nPot As Integer
    nTool_index As Integer
End Type

' cnc_delmagazine:deletion of magazine management data
Type IODBTLMAG2
    nMagazine As Integer
    nPot As Integer
End Type


'-------------------------------------
' CNC: Operation history data related
'-------------------------------------

' cnc_rdophistry:read operation history data
Type RECALM
  nRec_type As Integer          ' record type
  nAlm_grp As Integer           ' alarm group
  nAlm_no As Integer            ' alarm number
  sAxis_no As Byte              ' axis number
  sDummy As Byte
End Type

Type ODBHIS1
  nS_no As Integer              ' start number
  nType As Integer              ' dummy
  nE_no As Integer              ' end number
  nRec_alm(0 To 9) As RECALM
End Type                        ' In case that the number of data is 10

Type RECMDI
  nRec_type As Integer          ' record type
  sKey_code As Byte             ' key code
  sPw_flag As Byte              ' power on flag
  sDummy(0 To 3) As Byte
End Type

Type ODBHIS2
  nS_no As Integer              ' start number
  nType As Integer              ' dummy
  nE_no As Integer              ' end number
  nRec_mdi(0 To 9) As RECMDI
End Type                        ' In case that the number of data is 10

Type RECSGN
  nRec_type As Integer          ' record type
  sSig_name As Byte             ' signal name
  sSig_old As Byte              ' old signal bit pattern
  sSig_new As Byte              ' new signal bit pattern
  sDummy As Byte
  nSig_no As Integer            ' signal number
End Type

Type ODBHIS3
  nS_no As Integer              ' start number
  nType As Integer              ' dummy
  nE_no As Integer              ' end number
  nRec_sgn(0 To 9) As RECSGN
End Type                        ' In case that the number of data is 10

Type RECDATE
  nRec_type As Integer          ' record type
  sYear As Byte                 ' year
  sMonth As Byte                ' month
  sDay As Byte                  ' day
  sPw_flag As Byte              ' power on flag
  sDummy(0 To 1) As Byte
End Type

Type ODBHIS4
  nS_no As Integer              ' start number
  nType As Integer              ' dummy
  nE_no As Integer              ' end number
  nRec_date(0 To 9) As RECDATE
End Type                        ' In case that the number of data is 10

Type RECTIME
  nRec_type As Integer          ' record flag
  sHour As Byte                 ' hour
  sMinute As Byte               ' minute
  sSecond As Byte               ' second
  sPw_flag As Byte              ' power on flag
  sDummy(0 To 1) As Byte
End Type

Type ODBHIS5
  nS_no As Integer              ' start number
  nType As Integer              ' dummy
  nE_no As Integer              ' end number
  nRec_time(0 To 9) As RECTIME
End Type                        ' In case that the number of data is 10

' cnc_rdalmhistry:read alarm history data
Type ODBAHISdata
  nDummy As Integer
  nAlm_grp As Integer           ' alarm group
  nAlm_no As Integer            ' alarm number
  sAxis_no As Byte              ' axis number
  sYear As Byte                 ' year
  sMonth As Byte                ' month
  sDay As Byte                  ' day
  sHour As Byte                 ' hour
  sMinute As Byte               ' minute
  sSecond As Byte               ' second
  sDummy2 As Byte
  nLen_msg As Integer           ' alarm message length
  sAlm_msg As String * 32       ' alarm message
End Type

Type ODBAHIS
  nS_no As Integer              ' start number
  nType As Integer              ' dummy
  nE_no As Integer              ' end number
  nAlm_his(0 To 9) As ODBAHISdata
End Type                        ' In case that the number of data is 10

' cnc_rdalmhistry3:read alarm history data 3
Type ODBAHIS3data
  nDummy As Integer
  nAlm_grp As Integer           ' alarm group
  nAlm_no As Integer            ' alarm number
  sAxis_no As Byte              ' axis number
  sYear As Byte                 ' year
  sMonth As Byte                ' month
  sDay As Byte                  ' day
  sHour As Byte                 ' hour
  sMinute As Byte               ' minute
  sSecond As Byte               ' second
  sDummy2 As Byte
  nLen_msg As Integer           ' alarm message length
  sAlm_msg As String * 36       ' alarm message
End Type

Type ODBAHIS3
  nS_no As Integer              ' start number
  nType As Integer              ' dummy
  nE_no As Integer              ' end number
  nAlm_his(0 To 9) As ODBAHIS3data
End Type                        ' In case that the number of data is 10

' cnc_rdhissgnl:read signals related operation history
' cnc_wrhissgnl:write signals related operation history
Type IODBSIGdata
  nEnt_no As Integer            ' entry number
  nSig_no As Integer            ' signal number
  sSig_name As Byte             ' signal name
  sMask_pat As Byte             ' signal mask pattern
End Type

Type IODBSIG
  nDatano As Integer            ' dummy
  nType As Integer              ' dummy
  nData(0 To 19) As IODBSIGdata
End Type                        ' In case that the number of data is 20


'-------------
' CNC: Others
'-------------

' cnc_sysinfo:read CNC system information
Type ODBSYS
  nAddinfo As Integer           ' additional information
  nMax_axis As Integer          ' maximum axis number
  sCnc_type As String * 2       ' cnc type <ascii char>
  sMt_type As String * 2        ' M/T/TT <ascii char>
  sSeries As String * 4         ' series NO. <ascii char>
  sVersion As String * 4        ' version NO.<ascii char>
  sAxes As String * 2           ' axis number<ascii char>
End Type

' cnc_statinfo:read CNC status information
Type ODBST
  nDummy As Integer             ' dummy
  nTmmode As Integer            ' T/M mode
  nAut As Integer               ' selected automatic mode
  nRun As Integer               ' running status
  nMotion As Integer            ' axis, dwell status
  nMstb As Integer              ' m, s, t, b status
  nEmergency As Integer         ' emergency stop status
  nAlarm As Integer             ' alarm status
  nEdit As Integer              ' editting status
End Type

' cnc_alarm:read alarm status
Type ODBALM
  nDummy(0 To 1) As Integer     ' dummy
  nData As Integer              ' alarm status
End Type

' cnc_rdalminfo:read alarm information
Type ALMINFO1data
  nAxis As Integer
  nAlm_no As Integer
End Type

Type ALMINFO1
  nAlm(0 To 4) As ALMINFO1data
  nData_end As Integer
End Type                        ' In case that the number of alarm is 5

Type ALMINFO2data
  nAxis As Integer
  nAlm_no As Integer
  nMsg_len As Integer
  sAlm_msg As String * 32
End Type

Type ALMINFO2
  nAlm(0 To 4) As ALMINFO2data
  nData_end As Integer
End Type                        ' In case that the number of alarm is 5

' cnc_modal:read modal data
Type ODBMDLdata
  lAux_data As Long
  sFlag1 As Byte
  sFlag2 As Byte
End Type

Type ODBMDL1
  nDatano As Integer
  nType As Integer
  sG_data As Byte
End Type

Type ODBMDL2
  nDatano As Integer
  nType As Integer
  sG_rdata(0 To 34) As Byte
End Type

Type ODBMDL3
  nDatano As Integer
  nType As Integer
  nAux As ODBMDLdata
End Type

Type ODBMDL4
  nDatano As Integer
  nType As Integer
  nRaux1(0 To 26) As ODBMDLdata
End Type

Type ODBMDL5
  nDatano As Integer
  nType As Integer
  nRaux2(0 To 7) As ODBMDLdata
End Type

Type ODBMDL6
  nDatano As Integer
  nType As Integer
  sG_1shot(0 To 3) As Byte
End Type

' cnc_diagnoss:read diagnosis data
' cnc_diagnosr:read diagnosis data(area specified)
Type ODBDGN1
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  sCdata As Byte
End Type

Type ODBDGN2
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  nIdata As Integer
End Type

Type ODBDGN3
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  lLdata As Long
End Type

Type ODBDGN4
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  sCdatas(0 To 7) As Byte
End Type                        ' In case that the number of axes is 8

Type ODBDGN5
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  nIdatas(0 To 7) As Integer
End Type                        ' In case that the number of axes is 8

Type ODBDGN6
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  lLdatas(0 To 7) As Long
End Type                        ' In case that the number of axes is 8

' cnc_adcnv:read A/D conversion data
Type ODBAD
  nDatano As Integer            ' input analog voltage type
  nType As Integer              ' analog voltage type
  nData As Integer              ' digital voltage data
End Type

' cnc_rdopmsg:read operator's message
Type MSG
  nDatano As Integer            ' operator's message number
  nType As Integer              ' operator's message type
  nChar_num As Integer          ' message string length
  sData As String * 256         ' operator's message string
End Type                        ' In case that the data length is 256

' cnc_rdopmsg:read operator's message
Type MSG2
  nDatano As Integer            ' operator's message number
  nType As Integer              ' operator's message type
  nChar_num As Integer          ' message string length
  sData As String * 64          ' operator's message string
End Type                        ' In case that the data length is 64

' cnc_rdopmsg3:read operator's message
Type MSG3
  nDatano As Integer            ' operator's message number
  nType As Integer              ' operator's message type
  nChar_num As Integer          ' message string length
  sData As String * 256         ' operator's message string
End Type                        ' In case that the data length is 256

' cnc_rdopmsgmps:read operator message for MAPPS
Type OPMSGMPS
  nDatano As Integer            ' operator's message number
  nType As Integer              ' operator's message type
  nChar_num As Integer          ' message string length
  sData As String * 256         ' operator's message string
End Type                        ' In case that the data length is 256

' cnc_sysconfig:read CNC configuration information
Type ODBSYSC
  sSlot_no_p(0 To 15) As Byte
  sSlot_no_l(0 To 15) As Byte
  nMod_id(0 To 15) As Integer
  nSoft_id(0 To 15) As Integer
  sS_series(0 To 15) As String * 5
  sS_version(0 To 15) As String * 5
  nDummy(0 To 15) As Byte
  nM_rom As Integer
  nS_rom As Integer
  sSvo_soft As String * 8
  sPmc_soft As String * 6
  sLad_soft As String * 6
  sMcr_soft As String * 8
  sSpl1_soft As String * 6
  sSpl2_soft As String * 6
  nFrmmin As Integer
  nDrmmin As Integer
  nSrmmin As Integer
  nPmcmin As Integer
  nCrtmin As Integer
  nSv1min As Integer
  nSv3min As Integer
  nSicmin As Integer
  nPosmin As Integer
  nDrmmrc As Integer
  nDrmarc As Integer
  nPmcmrc As Integer
  nDmaarc As Integer
  nIopt As Integer
  nHdiio As Integer
  nFrmsub As Integer
  nDrmsub As Integer
  nSrmsub As Integer
  nSv5sub As Integer
  nSv7sub As Integer
  nSicsub As Integer
  nPossub As Integer
  nHamsub As Integer
  nGm2gr1 As Integer
  nCrtgr2 As Integer
  nGm1gr2 As Integer
  nGm2gr2 As Integer
  nCmmrb As Integer
  nSv5axs As Integer
  nSv7axs As Integer
  nSicaxs As Integer
  nPosaxs As Integer
  nHanaxs As Integer
  nRomr64 As Integer
  nSrmr64 As Integer
  nDr1r64 As Integer
  nDr2r64 As Integer
  nIopio2 As Integer
  nHdiio2 As Integer
  nCmmrb2 As Integer
  nRomfap As Integer
  nSrmfap As Integer
  nDrmfap As Integer
End Type

' cnc_rdprstrinfo:read program restart information
Type ODBPRS
  nDatano As Integer            ' dummy
  nType As Integer              ' dummy
  nData_info(0 To 4) As Integer ' data setting information
  lRstr_bc As Long              ' block counter
  lRstr_m(0 To 34) As Long      ' M code value
  lRstr_t(0 To 1) As Long       ' T code value
  lRstr_s As Long               ' S code value
  lRstr_b As Long               ' B code value
  lDest(0 To 7) As Long         ' program re-start position
  lDist(0 To 7) As Long         ' program re-start distance
End Type

' cnc_rdopnlsgnl:read output signal image of software operator's panel
' cnc_wropnlsgnl:write output signal of software operator's panel
Type IODBSGNL
  nDatano As Integer            ' dummy
  nType As Integer              ' data select flag
  nMode As Integer              ' mode signal
  nHndl_ax As Integer           ' Manual handle feed axis selection signal
  nHndl_mv As Integer           ' Manual handle feed travel distance selection signal
  nRpd_ovrd As Integer          ' rapid traverse override signal
  nJog_ovrd As Integer          ' manual feedrate override signal
  nFeed_ovrd As Integer         ' feedrate override signal
  nSpdl_ovrd As Integer         ' (not used)
  nBlck_del As Integer          ' optional block skip signal
  nSngl_blck As Integer         ' single block signal
  nMachn_lock As Integer        ' machine lock signal
  nDry_run As Integer           ' dry run signal
  nMem_prtct As Integer         ' memory protection signal
  nFeed_hold As Integer         ' automatic operation halt signal
End Type

' cnc_rdopnlgnrl:read general signal image of software operator's panel
' cnc_wropnlgnrl:write general signal image of software operator's panel
Type IODBGNRL
  nDatano As Integer            ' dummy
  nType As Integer              ' data select flag
  sSgnal As Byte                ' general signal
End Type

' cnc_rdopnlgsname:read general signal name of software operator's panel
' cnc_wropnlgsname:write general signal name of software operator's panel
Type IODBRDNA
  nDatano As Integer            ' dummy
  nType As Integer              ' data select flag
  sSgnl1_name As String * 9     ' general signal 1 name
  sSgnl2_name As String * 9     ' general signal 2 name
  sSgnl3_name As String * 9     ' general signal 3 name
  sSgnl4_name As String * 9     ' general signal 4 name
  sSgnl5_name As String * 9     ' general signal 5 name
  sSgnl6_name As String * 9     ' general signal 6 name
  sSgnl7_name As String * 9     ' general signal 7 name
  sSgnl8_name As String * 9     ' general signal 8 name
End Type

' cnc_getdtailerr:get detail error
Type ODBERR
  nErr_no As Integer
  nErr_dtno As Integer
End Type


' cnc_rdparainfo:read informations of CNC parameter
Type ODBPARAIFdata
  nPrm_no As Integer
  nPrm_type As Integer
End Type

Type ODBPARAIF
  nInfo_no As Integer
  nPrev_no As Integer
  nNext_no As Integer
  nInfo(0 To 9) As ODBPARAIFdata
End Type                        ' In case that the number of data is 10

' cnc_rdsetinfo:read informations of CNC setting data
Type ODBSETIFdata
  nSet_no As Integer
  nSet_type As Integer
End Type

Type ODBSETIF
  nInfo_no As Integer
  nPrev_no As Integer
  nNext_no As Integer
  nInfo(0 To 9) As ODBSETIFdata
End Type                        ' In case that the number of data is 10

' cnc_rddiaginfo:read informations of CNC diagnose data
Type ODBDIAGIFdata
  nDiag_no As Integer
  nDiag_type As Integer
End Type

Type ODBDIAGIF
  nInfo_no As Integer
  nPrev_no As Integer
  nNext_no As Integer
  nInfo(0 To 9) As ODBDIAGIFdata
End Type                        ' In case that the number of data is 10

' cnc_rdparanum:read maximum, minimum and total number of CNC parameter
Type ODBPARANUM
  nPara_min As Integer
  nPara_max As Integer
  nTotal_no As Integer
End Type

' cnc_rdsetnum:read maximum, minimum and total number of CNC setting data
Type ODBSETNUM
  nSet_min As Integer
  nSet_max As Integer
  nTotal_no As Integer
End Type

' cnc_rddiagnum:read maximum, minimum and total number of CNC diagnose data
Type ODBDIAGNUM
  nDiag_min As Integer
  nDiag_max As Integer
  nTotal_no As Integer
End Type

' cnc_rdfrominfo:read F-ROM information on CNC
Type ODBFINFOdata
  sSys_name As String * 12   ' F-ROM SYSTEM data Name
  lFrom_size As Long         ' F-ROM Size
End Type

Type ODBFINFO
  sSlot_name As String * 12  ' Slot Name
  lFrom_num  As Long         ' Number of F-ROM SYSTEM data
  nInfo(0 To 31) As ODBFINFOdata
End Type

' cnc_getfrominfo:read F-ROM information on CNC
Type ODBFINFORMdata
  sSys_name As String * 12   ' F-ROM SYSTEM data Name
  lFrom_size As Long         ' F-ROM Size
  lFrom_attrib As Long       ' F-ROM data attribute
End Type

Type ODBFINFORM
  lSlot_no As Long           ' Slot Number
  sSlot_name As String * 12  ' Slot Name
  lFrom_num  As Long         ' Number of F-ROM SYSTEM data
  nInfo(0 To 31) As ODBFINFORMdata
End Type

' cnc_rdsraminfo:read S-RAM information on CNC
' cnc_getsraminfo:read S-RAM information on CNC
Type ODBSINFOdata
  sSram_name As String * 12      ' S-RAM data Name
  lSram_size As Long             ' S-RAM data Size
  nDiv_number As Integer         ' Division number of S-RAM file
  sFname(0 To 6) As String * 16  ' S-RAM file names
End Type

Type ODBSINFO
  lSram_num As Long              ' Number of S-RAM data
  nInfo(0 To 7) As ODBSINFOdata
End Type

' cnc_dtsvrdpgdir:read file directory in Data Server
Type ODBDSDIRdata
  sFile_name As String * 16
  sComment As String * 64
  nSize As Long
  sDate As String * 16
End Type

Type ODBDSDIR
  nFile_num As Long
  nRemainder As Long
  nData_num As Integer
  nData(0 To 31) As ODBDSDIRdata
End Type

' cnc_dtsvrdset:read setting data for Data Server
' cnc_dtsvwrset:write setting data for Data Server
Type IODBDSSET
  sHost_ip As String * 16
  sHost_uname As String * 32
  sHost_passwd As String * 32
  sHost_dir As String * 128
  sDtsv_mac As String * 13
  sDtsv_ip As String * 16
  sDtsv_mask As String * 16
End Type

' cnc_dtsvmntinfo:read maintenance information for Data Server
Type ODBDSMNT
  lEmpty_cnt As Long
  lTotal_size As Long
  lRead_ptr As Long
  lWrite_ptr As Long
End Type

' cnc_rdposerrs2:read the position deviation S1 and S2
Type ODBPSER
  nPoserr1 As Long
  nPoserr2 As Long
End Type

' cnc_rdctrldi:read the control input signal
Type ODBSPDIdata
  nSgnl1 As Byte
  nSgnl2 As Byte
  nSgnl3 As Byte
  nSgnl4 As Byte
End Type
Type ODBSPDI
  nData(0 To 2) As ODBSPDIdata
End Type ' In case that the number of data is 3

' cnc_rdctrldo:read the control output signal
Type ODBSPDOdata
  nSgnl1 As Byte
  nSgnl2 As Byte
  nSgnl3 As Byte
  nSgnl4 As Byte
End Type
Type ODBSPDO
  nData(0 To 2) As ODBSPDOdata
End Type ' In case that the number of data is 3

' cnc_rdsvfeedback:Read Servo feedback multiplication data
Type ODBSVFBACK
  nDummy As Integer
  nDtype As Integer
  lFback(0 To 7) As Long
  lAfback(0 To 7) As Long
End Type

' cnc_rdwaveprm:read the parameter of wave diagnosis
' cnc_wrwaveprm:write the parameter of wave diagnosis
Type CHDATA
  nKind As Integer
  nAxis As Integer              ' nKind == 13 --> Low byte  : signal address
                                '                 High byte : bit number
  nIo_no As Integer             ' nKind == 13 --> signal number
End Type

Type IODBWAVE
  nCondition As Integer
  sTrg_adr As Byte
  sTrg_bit As Byte
  nTrg_no As Integer
  nDelay As Integer
  nT_range As Integer
  nCh(0 To 11) As CHDATA
End Type

' cnc_rdwaveprm2:read the parameter of wave diagnosis 2
' cnc_wrwaveprm2:write the parameter of wave diagnosis 2
Type WVPRMdata1                 ' nKind is except 13
  nKind As Integer
  nAxis As Long
  nReserve2 As Long
End Type

Type WVPRMdata2                 ' nKind is 13
  nKind As Integer
  sAdr As Byte
  sBit As Byte
  nNo As Integer
  nReserve2 As Long
End Type

Type IODBWVPRM
  nCondition As Integer
  sTrg_adr As Byte
  sTrg_bit As Byte
  nTrg_no As Integer
  nReserve1 As Integer
  nDelay As Long
  nT_range As Long
  nCh(0 To 11) As WVPRMdata1    ' or WVPRMdata2
End Type

' cnc_rdwavedata:read the data of wave diagnosis
Type ODBWVDT
  nChannel As Integer
  nKind As Integer
  nAxis As Integer              ' nKind == 13 --> Low byte  : signal address
                                '                 High byte : bit number
  nIo_no As Integer             ' nKind == 13 --> signal number
  sYear As Byte
  sMonth As Byte
  sDay As Byte
  sHour As Byte
  sMinute As Byte
  sSecond As Byte
  nT_cycle As Integer
  nData(0 To 8191) As Integer
End Type

' cnc_rdmgrpdata:read M-code group data
Type ODBMGRP
  lM_code As Long
  nGrp_no As Integer
  sM_name As String * 21
  sDummy As Byte
End Type

' cnc_wrmgrpdata:write M-code group data
Type IDBMGRP
  nS_no As Integer
  nDummy As Integer
  nNum As Integer
  nGroup(0 To 499) As Integer
End Type

' cnc_rdproctime:read processing time stamp data
Type ODBPTIMEdata
  lPrg_no As Long
  nHour As Integer
  sMinute As Byte
  sSecond As Byte
End Type

Type ODBPTIME
  nNum As Integer
  nData(0 To 9) As ODBPTIMEdata
End Type

' cnc_rdprgdirtime:read program directory for processing time data
Type PRGDIRTM
  lPrg_no As Long
  sComment As String * 51
  sCuttime As String * 13
End Type

' cnc_rdprogdir2:read program directory 2
#If ONO8D Then

Type PRGDIR2
  lNumber As Long
  lLength As Long
  sComment As String * 51
  sDummy As Byte
End Type

#Else

Type PRGDIR2
  nNumber As Integer
  lLength As Long
  sComment As String * 51
  sDummy As Byte
End Type

#End If

' cnc_rdprogdir3:read program directory 3
Type PRGDIR3mdata
  nYear As Integer
  nMonth As Integer
  nDay As Integer
  nHour As Integer
  nMinute As Integer
  nDummy As Integer
End Type

Type PRGDIR3cdata
  nYear As Integer
  nMonth As Integer
  nDay As Integer
  nHour As Integer
  nMinute As Integer
  nDummy As Integer
End Type

Type PRGDIR3
  lNumber As Long
  lLength As Long
  lPage As Long
  sComment As String * 52
  nmData As PRGDIR3mdata
  ncData As PRGDIR3cdata
End Type

' cnc_rdprogdir4:read program directory 4
Type PRGDIR4mdata
  nYear As Integer
  nMonth As Integer
  nDay As Integer
  nHour As Integer
  nMinute As Integer
  nDummy As Integer
End Type

Type PRGDIR4cdata
  nYear As Integer
  nMonth As Integer
  nDay As Integer
  nHour As Integer
  nMinute As Integer
  nDummy As Integer
End Type

Type PRGDIR4
  lNumber As Long
  lLength As Long
  lPage As Long
  sComment As String * 52
  nmData As PRGDIR4mdata
  ncData As PRGDIR4cdata
End Type

' cnc_rdcomparam:read communication parameter for DNC1, DNC2, OSI-Ethernet
' cnc_wrcomparam:write communication parameter for DNC1, DNC2, OSI-Ethernet
Type IODBCPRM
  sNcApli As String * 65
  sDummy1 As Byte
  sHostApli As String * 65
  sDummy2 As Byte
  lStatPstv As Long
  lStatNgtv As Long
  lStatmask As Long
  lAlarmStat As Long
  lPsclHaddr As Long
  lPsclLaddr As Long
  nSvcMode1 As Integer
  nSvcMode2 As Integer
  lFileTout As Long
  lRemTout As Long
End Type

' cnc_rdintchk:read interrupt check
' cnc_wrintchk:write interrupt check
Type IODBINT
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' kind of position
  nDatano_e As Integer          ' end offset number
  lData(0 To 23) As Long        ' position value of area for not attach
End Type

' cnc_rdwkcdshft:read work coordinate shift
' cnc_wrwkcdshft:write work coordinate shift
' cnc_rdwkcdsfms:read work coordinate shift measure
' cnc_wrwkcdsfms:write work coordinate shift measure
Type IODBWCSF
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  lData(0 To 7) As Long         ' data value
End Type                        ' In case that the number of axes is 8

' cnc_rdomhisinfo:read operator message history information
Type ODBOMIF
  nOm_max As Integer
  nOm_sum As Integer
  nOm_char As Integer
End Type

' cnc_rdomhistry:read operator message history
Type ODBOMHIS
  nOm_no As Integer
  nYear As Integer
  nMonth As Integer
  nDay As Integer
  nHour As Integer
  nMinute As Integer
  nSecond As Integer
  sOm_msg As String * 256
End Type

' cnc_rdbtofsr:read b-axis tool offset value(area specified)
' cnc_wrbtofsr:write b-axis tool offset value(area specified)
Type IODBBTO
  nDatano_s As Integer          ' start offset number
  nType As Integer              ' offset type
  nDatano_e As Integer          ' end offset number
  lOfs(0 To 17) As Long         ' offset
End Type                        ' In case that the number of data is 9 (B type)

' cnc_rdbtofsinfo:read b-axis tool offset information
Type ODBBTLINF
  nOfs_type As Integer
  nUse_no As Integer
  nSub_no As Integer
End Type

' cnc_rdbaxis:read b-axis command
Type ODBBAXIS
  nFlag As Integer
  nCommand As Integer
  nSpeed As Integer
  lSub_data As Long
End Type

' cnc_rdsyssoft:read CNC system soft series and version
Type ODBSYSS
  sSlot_no_p(0 To 15) As Byte
  sSlot_no_l(0 To 15) As Byte
  nModule_id(0 To 15) As Integer
  nSoft_id(0 To 15) As Integer
  sSoft_series(0 To 15) As String * 5
  sSoft_version(0 To 15) As String * 5
  nSoft_inst As Integer
  sBoot_ser As String * 5
  sBoot_ver As String * 5
  sServo_ser As String * 5
  sServo_ver As String * 5
  sPmc_ser As String * 5
  sPmc_ver As String * 5
  sLadder_ser As String * 5
  sLadder_ver As String * 5
  sMcrlib_ser As String * 5
  sMcrlib_ver As String * 5
  sMcrapl_ser As String * 5
  sMcrapl_ver As String * 5
  sSpl1_ser As String * 5
  sSpl1_ver As String * 5
  sSpl2_ser As String * 5
  sSpl2_ver As String * 5
  sSpl3_ser As String * 5
  sSpl3_ver As String * 5
  sC_exelib_ser As String * 5
  sC_exelib_ver As String * 5
  sC_exeapl_ser As String * 5
  sC_exeapl_ver As String * 5
  sInt_vga_ser As String * 5
  sInt_vga_ver As String * 5
  sOut_vga_ser As String * 5
  sOut_vga_ver As String * 5
  sPmm_ser As String * 5
  sPmm_ver As String * 5
  sPmc_mng_ser As String * 5
  sPmc_mng_ver As String * 5
  sPmc_shin_ser As String * 5
  sPmc_shin_ver As String * 5
  sPmc_shout_ser As String * 5
  sPmc_shout_ver As String * 5
  sPmc_c_ser As String * 5
  sPmc_c_ver As String * 5
  sPmc_edit_ser As String * 5
  sPmc_edit_ver As String * 5
  sLddr_mng_ser As String * 5
  sLddr_mng_ver As String * 5
  sLddr_apl_ser As String * 5
  sLddr_apl_ver As String * 5
  sSpl4_ser As String * 5
  sSpl4_ver As String * 5
  sMcr2_ser As String * 5
  sMcr2_ver As String * 5
  sMcr3_ser As String * 5
  sMcr3_ver As String * 5
  sEth_boot_ser As String * 5
  sEth_boot_ver As String * 5
  sReserve(0 To 7) As String * 5
End Type

' cnc_rdsyssoft2:read CNC system soft series and version
Type ODBSYSS2
  sSlot_no_p(0 To 15) As Byte
  sSlot_no_l(0 To 15) As Byte
  nModule_id(0 To 15) As Integer
  nSoft_id(0 To 15) As Integer
  sSoft_series(0 To 15) As String * 5
  sSoft_version(0 To 15) As String * 5
  nSoft_inst As Integer
  sBoot_ser As String * 5
  sBoot_ver As String * 5
  sServo_ser As String * 5
  sServo_ver As String * 5
  sPmc_ser As String * 5
  sPmc_ver As String * 5
  sLadder_ser As String * 5
  sLadder_ver As String * 5
  sMcrlib_ser As String * 5
  sMcrlib_ver As String * 5
  sMcrapl_ser As String * 5
  sMcrapl_ver As String * 5
  sSpl1_ser As String * 5
  sSpl1_ver As String * 5
  sSpl2_ser As String * 5
  sSpl2_ver As String * 5
  sSpl3_ser As String * 5
  sSpl3_ver As String * 5
  sC_exelib_ser As String * 5
  sC_exelib_ver As String * 5
  sC_exeapl_ser As String * 5
  sC_exeapl_ver As String * 5
  sInt_vga_ser As String * 5
  sInt_vga_ver As String * 5
  sOut_vga_ser As String * 5
  sOut_vga_ver As String * 5
  sPmm_ser As String * 5
  sPmm_ver As String * 5
  sPmc_mng_ser As String * 5
  sPmc_mng_ver As String * 5
  sPmc_shin_ser As String * 5
  sPmc_shin_ver As String * 5
  sPmc_shout_ser As String * 5
  sPmc_shout_ver As String * 5
  sPmc_c_ser As String * 5
  sPmc_c_ver As String * 5
  sPmc_edit_ser As String * 5
  sPmc_edit_ver As String * 5
  sLddr_mng_ser As String * 5
  sLddr_mng_ver As String * 5
  sLddr_apl_ser As String * 5
  sLddr_apl_ver As String * 5
  sSpl4_ser As String * 5
  sSpl4_ver As String * 5
  sMcr2_ser As String * 5
  sMcr2_ver As String * 5
  sMcr3_ser As String * 5
  sMcr3_ver As String * 5
  sEth_boot_ser As String * 5
  sEth_boot_ver As String * 5
  sReserve(0 To 7) As String * 5
  sEmbEthe_ser As String * 5
  sEmbEthe_ver As String * 5
  sReserve2(0 To 37) As String * 5
End Type

' cnc_rdmdlconfig:read CNC module configuration information
Type ODBMDLC
  nFrom As Integer
  nDram As Integer
  nSram As Integer
  nPmc As Integer
  nCrtc As Integer
  nServo12 As Integer
  nServo34 As Integer
  nServo56 As Integer
  nServo78 As Integer
  nSic As Integer
  nPos_lsi As Integer
  nHi_aio As Integer
  nReserve(0 To 11) As Integer
  nDrmmrc As Integer
  nDrmarc As Integer
  nPmcmrc As Integer
  nDmaarc As Integer
  nIopt As Integer
  nHdiio As Integer
  nGm2gr1 As Integer
  nCrtgr2 As Integer
  nGm1gr2 As Integer
  nGm2gr2 As Integer
  nCmmrb As Integer
  nSv5axs As Integer
  nSv7axs As Integer
  nSicaxs As Integer
  nPosaxs As Integer
  nHamaxs As Integer
  nRomr64 As Integer
  nSrmr64 As Integer
  nDr1r64 As Integer
  nDr2r64 As Integer
  nIopio2 As Integer
  nHdiio2 As Integer
  nCmmrb2 As Integer
  nRomfap As Integer
  nSrmfap As Integer
  nDrmfap As Integer
  nDrmare As Integer
  nPmcmre As Integer
  nDmaare As Integer
  nFrmbgg As Integer
  nDrmbgg As Integer
  nAsrbgg As Integer
  nEdtpsc As Integer
  nSlcpsc As Integer
  nReserve2(0 To 33) As Integer
End Type

' cnc_rdpscdproc:read processing condition file (processing data)
' cnc_wrpscdproc:write processing condition file (processing data)
Type IODBPSCD
  nSlct As Integer
  lFeed As Long
  nPower As Integer
  nFreq As Integer
  nDuty As Integer
  nG_press As Integer
  nG_kind As Integer
  nG_ready_t As Integer
  nDisplace As Integer
  lSupple As Long
  nEdge_slt As Integer
  nAppr_slt As Integer
  nReserve(0 To 4) As Integer
End Type

' cnc_rdpscdpirc:read processing condition file (piercing data)
' cnc_wrpscdpirc:write processing condition file (piercing data)
Type IODBPIRC
  nSlct As Integer
  nPower As Integer
  nFreq As Integer
  nDuty As Integer
  nI_freq As Integer
  nI_duty As Integer
  nStep_t As Integer
  nStep_sum As Integer
  lPier_t As Long
  nG_press As Integer
  nG_kind As Integer
  nG_time As Integer
  nDef_pos As Integer
  nReserve(0 To 3) As Integer
End Type

' cnc_rdpscdedge:read processing condition file (edging data)
' cnc_wrpscdedge:write processing condition file (edging data)
Type IODBEDGE
  nSlct As Integer
  nAngle As Integer
  nPower As Integer
  nFreq As Integer
  nDuty As Integer
  lPier_t As Long
  nG_press As Integer
  nG_kind As Integer
  lR_len As Long
  nR_feed As Integer
  nR_freq As Integer
  nR_duty As Integer
  nReserve(0 To 4) As Integer
End Type

' cnc_rdpscdslop:read processing condition file (slope data)
' cnc_wrpscdslop:write processing condition file (slope data)
Type IODBSLOP
  lSlct As Long
  lUpleng As Long
  nUpsp(0 To 9) As Integer
  lDwleng As Long
  nDwsp(0 To 9) As Integer
  nReserve(0 To 9) As Integer
End Type

' cnc_rdlpwrdty:read power controll duty data
' cnc_wrlpwrdty:write power controll duty data
Type IODBLPWDT
  nSlct As Integer
  nDty_const As Integer
  nDty_min As Integer
  nReserve(0 To 5) As Integer
End Type

' cnc_rdlpwrdat:read laser power data
Type ODBLOPDT
  nSlct As Integer
  nPwr_mon As Integer
  nPwr_ofs As Integer
  nPwr_act As Integer
  lFeed_act As Long
  nReserve(0 To 3) As Integer
End Type

' cnc_rdlagslt:read laser assist gas selection
' cnc_wrlagslt:write laser assist gas selection
Type IODBLAGSL
  nSlct As Integer
  nAg_slt As Integer
  nAgflow_slt As Integer
  nReserve(0 To 5) As Integer
End Type

' cnc_rdlagst:read laser assist gas flow
' cnc_wrlagst:write laser assist gas flow
Type IODBLAGSTdata
  nSlct As Integer
  nPre_time As Integer
  nPre_press As Integer
  nProc_press As Integer
  nEnd_time As Integer
  nEnd_press As Integer
  nReserve(0 To 2) As Integer
End Type

Type IODBLAGST
  nData(0 To 2) As IODBLAGSTdata
End Type

' cnc_rdledgprc:read laser power for edge processing
' cnc_wrledgprc:write laser power for edge processing
Type IODBLEGPR
  nSlct As Integer
  nPower As Integer
  nFreq As Integer
  nDuty As Integer
  nReserve(0 To 4) As Integer
End Type

' cnc_rdlprcprc:read laser power for piercing
' cnc_wrlprcprc:write laser power for piercing
Type IODBLPCPR
  nSlct As Integer
  nPower As Integer
  nFreq As Integer
  nDuty As Integer
  lTime As Long
  nReserve(0 To 3) As Integer
End Type

' cnc_rdlcmddat:read laser command data
Type ODBLCMDT
  nSlct As Integer
  lFeed As Long
  nPower As Integer
  nFreq As Integer
  nDuty As Integer
  nG_kind As Integer
  nG_ready_t As Integer
  nG_press As Integer
  nError As Integer
  lDsplc As Long
  nReserve(0 To 6) As Integer
End Type

' cnc_rdlactnum:read active number
Type ODBLACTN
  nSlct As Integer
  nAct_proc As Integer
  nAct_pirce As Integer
  nAct_slop As Integer
  nReserve(0 To 4) As Integer
End Type

' cnc_rdlcmmt:read laser comment
Type ODBLCMMT
  sComment As String * 25
End Type

' cnc_rdpwofsthis:read power correction factor history data
Type ODBPWOFST
  lPwratio As Long
  lRfvolt As Long
  nYear As Integer
  nMonth As Integer
  nDay As Integer
  nHour As Integer
  nMinute As Integer
  nSecond As Integer
End Type

' cnc_rdmngtime:read management time
' cnc_wrmngtime:write management time
Type IODBMNGTIME
  lLife As Long
  lTotal As Long
End Type

' cnc_rddischarge:read data related to electrical discharge at power correction ends
Type ODBDISCHRG
  nAps As Integer
  nYear As Integer
  nMonth As Integer
  nDay As Integer
  nHour As Integer
  nMinute As Integer
  nSecond As Integer
  nHpc As Integer
  nHfq As Integer
  nHdt As Integer
  nHpa As Integer
  lHce As Long
  lRfi(0 To 7) As Long
  lRfv(0 To 7) As Long
  lDci(0 To 7) As Long
  lDcv(0 To 7) As Long
  lDcw(0 To 7) As Long
End Type

' cnc_rddischrgalm:read alarm history data related to electrical discharg
Type ODBDISCHRGALM
  nYear As Integer
  nMonth As Integer
  nDay As Integer
  nHour As Integer
  nMinute As Integer
  nSecond As Integer
  lAlmnum As Long
  lPsec As Long
  nHpc As Integer
  nHfq As Integer
  nHdt As Integer
  nHpa As Integer
  lHce As Long
  nAsq As Integer
  nPsu As Integer
  nAps As Integer
  nDummy As Integer
  lRfi(0 To 7) As Long
  lRfv(0 To 7) As Long
  lDci(0 To 7) As Long
  lDcv(0 To 7) As Long
  lDcw(0 To 7) As Long
  nAlmcd(0 To 7) As Integer
End Type

' cnc_gettimer:get date and time from cnc
' cnc_settimer:set date and time for cnc
Type IODBTIMER1
  nType As Integer
  nDummy As Integer
  nYear As Integer
  nMonth As Integer
  nDate As Integer
End Type

Type IODBTIMER2
  nType As Integer
  nDummy As Integer
  nHour As Integer
  nMinute As Integer
  nSecond As Integer
End Type

' cnc_rdtlctldata: read tool controll data
' cnc_wrtlctldata: write tool controll data
Type IODBTLCTL
  nSlct As Integer
  nUsed_tool As Integer
  nTurret_indx As Integer
  lZero_tl_no As Long
  lT_axis_move As Long
  lTotal_punch(0 To 1) As Long
  nReserve(0 To 10) As Integer
End Type

' cnc_rdtoolinfo: read tool information
Type ODBPTLINF
  nTld_max As Integer
  nMlt_max As Integer
  nReserve As Integer
  nTld_size(0 To 15) As Integer
  nMlt_size(0 To 15) As Integer
  nReserves(0 To 15) As Integer
End Type

' cnc_rdtooldata: read tool data
' cnc_wrtooldata: read tool data
Type IODBTLDT
  nSlct As Integer
  lTool_no As Long
  lX_axis_ofs As Long
  lY_axis_ofs As Long
  lTurret_pos As Long
  lChg_tl_no As Long
  lPunch_count As Long
  lTool_life As Long
  lM_tl_radius As Long
  lM_tl_angle As Long
  sTl_shape As Byte
  lTl_size_i As Long
  lTl_size_j As Long
  lTl_angle As Long
  lReserve(0 To 2) As Long
End Type

' cnc_rdmultitldt: read multi tool data
' cnc_wrmultitldt: write multi tool data
Type IODBMLTTL
  nSlct As Integer
  nM_tl_no As Integer
  lM_tl_radius As Long
  lM_tl_angle As Long
  lX_axis_ofs As Long
  lY_axis_ofs As Long
  sTl_shape As Byte
  lTl_size_i As Long
  lTl_size_j As Long
  lTl_angle As Long
  lReserve(0 To 6) As Long
End Type

' cnc_rdmtapdata: read multi tap data
' cnc_wrmtapdata: write multi tap data
Type IODBMTAP
  nSlct As Integer
  lTool_no As Long
  lX_axis_ofs As Long
  lY_axis_ofs As Long
  lPunch_count As Long
  lTool_life As Long
  lReserve(0 To 10) As Long
End Type

' cnc_rdsafetyzone: read safetyzone data
' cnc_wrsafetyzone: write safetyzone data
Type IODBSAFE
  nSlct As Integer
  lData(0 To 2) As Long
End Type

' cnc_rdtoolzone: read toolzone data
' cnc_wrtoolzone: write toolzone data
Type IODBTLZN
  nSlct As Integer
  lData(0 To 1) As Long
End Type

' cnc_rdacttlzone: read active toolzone data
Type ODBACTTLZN
  nAct_no As Integer
  lData(0 To 1) As Long
End Type

' cnc_rdaxisname: read axis name
Type ODBAXISNAME
  sName As String * 1           ' axis name
  sSuff As String * 1           ' suffix
End Type

' cnc_rdspdlname: read spindle name
Type ODBSPDLNAME
  sName As String * 1           ' axis name
  sSuff1 As String * 1          ' suffix
  sSuff2 As String * 1          ' suffix
  sSuff3 As String * 1          ' suffix
End Type


'------------------
' CNC : SERCOS I/F
'------------------

' cnc_srcsrdidinfo:Read ID information of SERCOS I/F
' cnc_srcswridinfo:Write ID information of SERCOS I/F
Type IODBIDINF
  lId_no As Long
  nDrv_no As Integer
  nAcc_element As Integer
  nErr_general As Integer
  nErr_id_no As Integer
  nErr_id_name As Integer
  nErr_attr As Integer
  nErr_unit As Integer
  nErr_min_val As Integer
  nErr_max_val As Integer
  nId_name_len As Integer
  nId_name_max As Integer
  sId_name As String * 60
  lAttr As Long
  nUnit_len As Integer
  nUnit_max As Integer
  sUnit As String * 12
  lMin_val As Long
  lMax_val As Long
End Type

' cnc_srcsrdexstat:Get execution status of reading/writing operation data of SERCOS I/F
Type ODBSRCSST
  nAcc_element As Integer
  nErr_general As Integer
  nErr_id_no As Integer
  nErr_attr As Integer
  nErr_op_data As Integer
End Type

' cnc_srcsrdlayout:Read drive assign of SERCOS I/F
Type ODBSRCSLYT
  nSpndl(0 To 3) As Integer
  nServo(0 To 7) As Integer
  sAxis_name(0 To 7) As String * 1
End Type


'----------------------------
' CNC : Servo Guide
'----------------------------
' cnc_sdsetchnl:Servo Guide (Channel data set)
Type IODBCHAN
  sChno As Byte
  sAxis As Byte
  lDatanum As Long
  nDatainf As Integer
  nDataadr As Integer
End Type

' cnc_sdsetchnl:Servo Guide (read Sampling data)
' cnc_sfbreadsmpl:Servo feedback data (read Sampling data)
Type ODBSD
  nChadata As Integer
  lCount As Long
End Type

' cnc_sfbsetchnl:Servo feedback data (Channel data set)
Type IDBSFBCHAN
  sChno As Byte
  sAxis As Byte
  nShift As Integer
End Type


'-----
' PMC
'-----

' pmc_rdpmcrng:read PMC data(area specified)
' pmc_wrpmcrng:write PMC data(area specified)
Type IODBPMC1
  nType_a As Integer            ' PMC address type
  nType_d As Integer            ' PMC data type
  nDatano_s As Integer          ' start PMC address
  nDatano_e As Integer          ' end PMC address
  sCdata(0 To 4) As Byte
End Type                        ' In case that the number of data is 5

Type IODBPMC2
  nType_a As Integer            ' PMC address type
  nType_d As Integer            ' PMC data type
  nDatano_s As Integer          ' start PMC address
  nDatano_e As Integer          ' end PMC address
  nIdata(0 To 4) As Integer
End Type                        ' In case that the number of data is 5

Type IODBPMC3
  nType_a As Integer            ' PMC address type
  nType_d As Integer            ' PMC data type
  nDatano_s As Integer          ' start PMC address
  nDatano_e As Integer          ' end PMC address
  lLdata(0 To 4) As Long
End Type                        ' In case that the number of data is 5

' pmc_rdpmcinfo:read informations of PMC data
Type ODBPMCINFdata
  sPmc_adr As Byte
  sAdr_attr As Byte
  nTop_num As Integer
  nLast_num As Integer
End Type

Type ODBPMCINF
  nDatano As Integer
  nInfo(0 To 63) As ODBPMCINFdata
End Type

' pmc_rdcntldata:read PMC parameter data table control data
' pmc_wrcntldata:write PMC parameter data table control data
Type IODBPMCCNTLdata
  sTbl_prm As Byte
  sData_type As Byte
  nData_size As Integer
  nData_dsp As Integer
  nDummy As Integer
End Type

Type IODBPMCCNTL
  nDatano_s As Integer
  nDummy As Integer
  nDatano_e As Integer
  nInfo(0 To 99) As IODBPMCCNTLdata
End Type

' pmc_rdalmmsg:read PMC alarm message
Type ODBPMCALM
    sAlmmsg As String * 128
End Type

' pmc_getdtailerr:get detail error for pmc
Type ODBPMCERR
  nErr_no As Integer
  nErr_dtno As Integer
End Type

' pmc_rdpmctitle:read pmc title data
Type ODBPMCTITLE
    sMtb As String * 48
    sMachine As String * 48
    sType As String * 48
    sPrgno As String * 8
    sPrgvers As String * 4
    sPrgdraw As String * 48
    sDate As String * 32
    sDesign As String * 48
    sWritten As String * 48
    sRemarks As String * 48
End Type


'--------------------------
' PMC : PROFIBUS function
'--------------------------

' read PROFIBUS configration data
Type ODBPRFCNF
  sMaster_ser As String * 5
  sMaster_ver As String * 3
  sSlave_ser As String * 5
  sSlave_ver As String * 3
  sCntl_ser As String * 5
  sCntl_ver As String * 3
End Type

' read bus parameter for master function
' write bus parameter for master function
Type IODBBUSPRM
  sFdl_add As Byte
  sBaudrate As Byte
  nTsl As Integer
  nMin_tsdr As Integer
  nMax_tsdr As Integer
  sTqui As Byte
  sTset As Byte
  lTtr As Long
  sGap As Byte
  sHsa As Byte
  sMax_retry As Byte
  sBp_flag As Byte
  nMin_slv_int As Integer
  nPoll_tout As Integer
  nData_cntl As Integer
  sReserve1(0 To 5) As Byte
  sCls2_name As String * 32
  nUser_dlen As Integer
  sUser_data(0 To 61) As Byte
  sReserve2(0 To 95) As Byte
End Type

' read slave parameter for master function
' write slave parameter for master function
Type IODBSLVPRM
  nDis_enb As Integer
  nIdent_no As Integer
  sSlv_flag As Byte
  sSlv_type As Byte
  sReserve1(0 To 11) As Byte
  sSlv_stat As Byte
  sWd_fact1 As Byte
  sWd_fact2 As Byte
  sMin_tsdr As Byte
  sReserve2 As Byte
  sGrp_ident As Byte
  nUser_plen As Integer
  sUser_pdata(0 To 31) As Byte
  nCnfg_dlen As Integer
  sCnfg_data(0 To 125) As Byte
  nSlv_ulen As Integer
  sSlv_udata(0 To 29) As Byte
  sReserve3(0 To 7) As Byte
End Type

Type IODBSLVPRM2
  nDis_enb As Integer
  nIdent_no As Integer
  sSlv_flag As Byte
  sSlv_type As Byte
  sReserve1(0 To 11) As Byte
  sSlv_stat As Byte
  sWd_fact1 As Byte
  sWd_fact2 As Byte
  sMin_tsdr As Byte
  sReserve2 As Byte
  sGrp_ident As Byte
  nUser_plen As Integer
  sUser_pdata(0 To 205) As Byte
  nCnfg_dlen As Integer
  sCnfg_data(0 To 125) As Byte
  nSlv_ulen As Integer
  sSlv_udata(0 To 29) As Byte
  sReserve3(0 To 7) As Byte
End Type

' read allocation address for master function
' set allocation address for master function
Type IODBPRFADR
  sDi_size As Byte
  sDi_type As Byte
  nDi_addr As Integer
  nReserve1 As Integer
  sDo_size As Byte
  sDo_type As Byte
  nDo_addr As Integer
  nReserve2 As Integer
  sDgn_size As Byte
  sDgn_type As Byte
  nDgn_addr As Integer
End Type

' read allocation address for slave function
' set allocation address for slave function
Type IODBSLVADR
 sSlave_no As Byte
 sDi_size As Byte
 sDi_type As Byte
 nDi_addr As Integer
 sDo_size As Byte
 sDo_type As Byte
 nDo_addr As Integer
 sReserve(0 To 6) As Byte
End Type

' read status for slave function
Type ODBSLVST
  sCnfg_stat As Byte
  sPrm_stat As Byte
  sWdg_stat As Byte
  sLive_stat As Byte
  nIdent_no As Integer
End Type


'-----------------------------------
' CB : CUSTOMER'S BOARD function
'-----------------------------------

Type ODBTRANSINFO
    lMas_buff_size As Long
    nTrans_start_reqflag As Integer
    nTrans_end_reqflag As Integer
    nTrans_start_respflag As Integer
    nTrans_end_respflag As Integer
    lAll_transfer_size As Long
    nData_id As Integer
    nReserve As Integer
    lData_write_pt As Long
    lData_read_pt As Long
    lAccumulation_counter As Long
    nForwarding_status As Integer
End Type


'-----------------------------------------------
' DS : Data server & Ethernet board function    
'-----------------------------------------------

' etb_rdparam : read the parameter of the Ethernet board
' etb_wrparam : write the parameter of the Ethernet board
Type TCPPRM
    sOwnIPAddress As String * 16
    sSubNetMask As String * 16
    sRouterIPAddress As String * 16
End Type

Type HOSTPRM
    nDataServerPort As Integer
    sDataServerIPAddress As String * 16
    sDataServerUserName As String * 32
    sDataServerPassword As String * 32
    sDataServerLoginDirectory As String * 128
End Type

Type FTPPRM
    sFTPServerUserName As String * 32
    sFTPServerPassword As String * 32
    sFTPServerLoginDirectory As String * 128
End Type

Type ETBPRM
    sOwnMACAddress As String * 13
    nMaximumChannel As Integer
    nHDDExistence As Integer
    nNumberOfScreens As Integer
End Type

Type IODBETP1
    nParameterType As Integer
    uTcp As TCPPRM
End Type

Type IODBETP2
    nParameterType As Integer
    uHost As HOSTPRM
End Type

Type IODBETP3
    nParameterType As Integer
    uFtp As FTPPRM
End Type

Type IODBETP4
    nParameterType As Integer
    uEtb As ETBPRM
End Type

' etb_rderrmsg : read the error message of the Ethernet board
Type ODBETMSG
    sTitle As String * 33
    sMessage(0 To 9) As String * 39
End Type

' ds_rdhddinfo : read information of the Data Server's HDD
Type ODBHDDINF
    lFile_num As Long
    lRemainder_l As Long
    lRemainder_h As Long
    sCurrent_dir As String * 32
End Type

' ds_rdhdddir : read the file list of the Data Server's HDD
Type ODBHDDDIR
    sFile_name As String * 64
    sComment As String * 80
    nAttribute As Integer
    nReserved As Integer
    lSize As Long
    sDate As String * 16
End Type

' ds_rdhostdir : read the file list of the host
Type ODBHOSTDIR
    sHost_file As String * 128
End Type

' ds_rdmntinfo : read maintenance information
Type DSMNTINFO
    nEmpty_cnt As Integer
    lTotal_size As Long
    nReadPtr As Integer
    nWritePtr As Integer
End Type

'--------------------------
' HSSB multiple connection
'--------------------------

' cnc_rdnodeinfo:read node informations
Type ODBNODE
 nNode_no As Long
 nIo_base As Long
 nStatus As Long
 nCnc_type As Long
 sNode_name As String * 20
End Type


'-------------------------------------
' CNC: Control axis / spindle related
'-------------------------------------

' read actual axis feedrate(F)
Declare Function cnc_actf% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBACT)

' read absolute axis position
Declare Function cnc_absolute% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read machine axis position
Declare Function cnc_machine% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read relative axis position
Declare Function cnc_relative% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read distance to go
Declare Function cnc_distance% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read skip position
Declare Function cnc_skip% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read servo delay value
Declare Function cnc_srvdelay% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read acceleration/deceleration delay value
Declare Function cnc_accdecdly% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read all dynamic data
#If ONO8D Then
Declare Function cnc_rddynamic% Lib "fwlib32.dll" Alias "cnc_rddynamico8" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)
#Else
Declare Function cnc_rddynamic% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)
#End If

' read actual spindle speed(S)
Declare Function cnc_acts% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBACT)

' read actual spindle speed(S) (All or specified)
Declare Function cnc_acts2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBACT2)

' set origin / preset relative axis position
Declare Function cnc_wrrelpos% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IDBWRR)

' preset work coordinate
Declare Function cnc_prstwkcd% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IDBWRA)

' read manual overlapped motion value
Declare Function cnc_rdmovrlap% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As IODBOVL)

' cancel manual overlapped motion value
Declare Function cnc_canmovrlap% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read load information of serial spindle
Declare Function cnc_rdspload% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBSPN)

' read maximum r.p.m. ratio of serial spindle
Declare Function cnc_rdspmaxrpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBSPN)

' read gear ratio of serial spindle
Declare Function cnc_rdspgear% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBSPN)

' read absolute axis position 2
Declare Function cnc_absolute2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read relative axis position 2
Declare Function cnc_relative2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAXIS)

' read tool position
Declare Function cnc_rdposition% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As Any)

' read current speed
Declare Function cnc_rdspeed% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal nType%, Odb As ODBSPEED)

' read spindle load meter
Declare Function cnc_rdspmeter Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal nType As Integer, nLen As Integer, Odb As Any) As Integer
' read the spindle speed
Declare Function cnc_rdspdlspeed Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Integer, ByVal b As Integer, c As Long) As Integer

'----------------------
' CNC: Program related
'----------------------

' start downloading NC program
Declare Function cnc_dwnstart% Lib "fwlib32.dll" (ByVal FlibHndl%)

' download NC program
Declare Function cnc_download% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b%)

' download NC program(conditional)
Declare Function cnc_cdownload% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b%)

' end of downloading NC program
Declare Function cnc_dwnend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' end of downloading NC program
Declare Function cnc_dwnend2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' start downloading NC program 3
Declare Function cnc_dwnstart3% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' download NC program 3
Declare Function cnc_download3% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, ByVal b$)

' end of downloading NC program 3
Declare Function cnc_dwnend3% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start verification of NC program
Declare Function cnc_vrfstart% Lib "fwlib32.dll" (ByVal FlibHndl%)

' verify NC program
Declare Function cnc_verify% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b%)

' verify NC program(conditional)
Declare Function cnc_cverify% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b%)

' end of verification
Declare Function cnc_vrfend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start downloading DNC program
Declare Function cnc_dncstart% Lib "fwlib32.dll" (ByVal FlibHndl%)

' download DNC program
Declare Function cnc_dnc% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b&)

' download DNC program(conditional)
Declare Function cnc_cdnc% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b&)

' end of downloading DNC program
Declare Function cnc_dncend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start downloading DNC program 2
Declare Function cnc_dncstart2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' download DNC program 2
Declare Function cnc_dnc2% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, ByVal b$)

' end of downloading DNC program 2
Declare Function cnc_dncend2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read the diagnosis data of DNC operation
Declare Function cnc_rddncdgndt% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBDNCDGN)

' start uploading NC program
#If ONO8D Then
Declare Function cnc_upstart% Lib "fwlib32.dll" Alias "cnc_upstarto8" (ByVal FlibHndl%, ByVal a&)
#Else
Declare Function cnc_upstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)
#End If

' upload NC program
Declare Function cnc_upload% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBUP, a&)

' upload NC program(conditional)
Declare Function cnc_cupload% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBUP, a&)

' end of uploading NC program
Declare Function cnc_upend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start uploading NC program 3
Declare Function cnc_upstart3% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, ByVal c&)

' upload NC program 3
Declare Function cnc_upload3% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, ByVal b$)

' end of uploading NC program
Declare Function cnc_upend3% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read buffer status for downloading/verification NC program
Declare Function cnc_buff% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBBUF)

' search specified program
#If ONO8D Then
Declare Function cnc_search% Lib "fwlib32.dll" Alias "cnc_searcho8" (ByVal FlibHndl%, ByVal a&)
#Else
Declare Function cnc_search% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)
#End If

' delete all programs
Declare Function cnc_delall% Lib "fwlib32.dll" (ByVal FlibHndl%)

' delete specified program
#If ONO8D Then
Declare Function cnc_delete% Lib "fwlib32.dll" Alias "cnc_deleteo8" (ByVal FlibHndl%, ByVal a&)
#Else
Declare Function cnc_delete% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)
#End If

' read program directory
#If ONO8D Then
Declare Function cnc_rdprogdir% Lib "fwlib32.dll" Alias "cnc_rdprogdiro8" (ByVal FlibHndl%, ByVal a%, ByVal b&, ByVal c&, ByVal d&, Odb As PRGDIR)
#Else
Declare Function cnc_rdprogdir% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d&, Odb As PRGDIR)
#End If

' read program information
Declare Function cnc_rdproginfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' read program number under execution
#If ONO8D Then
Declare Function cnc_rdprgnum% Lib "fwlib32.dll" Alias "cnc_rdprgnumo8" (ByVal FlibHndl%, Odb As ODBPRO)
#Else
Declare Function cnc_rdprgnum% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPRO)
#End If

' read sequence number under execution
Declare Function cnc_rdseqnum% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSEQ)

' search specified sequence number
Declare Function cnc_seqsrch% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&)

' rewind cursor of NC program
Declare Function cnc_rewind% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read block counter
Declare Function cnc_rdblkcount% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read program under execution
Declare Function cnc_rdexecprog% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, b%, ByVal c$)

' write program for MDI operation
Declare Function cnc_wrmdiprog% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$)

' read execution pointer for MDI operation
#If ONO8D Then
Declare Function cnc_rdmdipntr% Lib "fwlib32.dll" Alias "cnc_rdmdipntro8" (ByVal FlibHndl%, Odb As ODBMDIP)
#Else
Declare Function cnc_rdmdipntr% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBMDIP)
#End If

' write execution pointer for MDI operation
Declare Function cnc_wrmdipntr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&)

' read constantly spindle speed
Declare Function cnc_rdspcss% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBCSS)

' read execution program pointer
Declare Function cnc_rdexecpt% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As PRGPNT, Odb As PRGPNT)

' read MDI program status
Declare Function cnc_rdmdiprgstat Lib "fwlib32.dll" (ByVal FlibHndl As Integer, a As Integer ) As Integer

' line edit (read program)
Declare Function cnc_rdprogline Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long, ByVal b As Long, c As Byte, d As Long, e As Long) As Integer

' line edit (write program)
Declare Function cnc_wrprogline Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long, ByVal b As Long, c As Byte, ByVal d As Long) As Integer

' line edit (delete line in program)
Declare Function cnc_delprogline Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long, ByVal b As Long, ByVal c As Long ) As Integer

' line edit (search string)
Declare Function cnc_searchword Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long, ByVal b As Long, ByVal c As Integer, ByVal d As Integer, e As Long, f As Byte) As Integer

' line edit (search string)
Declare Function cnc_searchresult Lib "fwlib32.dll" (ByVal FlibHndl As Integer, a As Long) As Integer

' program lock
Declare Function cnc_setpglock Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long) As Integer

' program unlock
Declare Function cnc_resetpglock Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long ) As Integer

' read the status of the program lock
Declare Function cnc_rdpglockstat Lib "fwlib32.dll" (ByVal FlibHndl As Integer, a As Long, b As Long) As Integer

' NC program copy
Declare Function cnc_copyprog Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long, ByVal b As Long ) As Integer

' condense program
Declare Function cnc_condense Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Integer, ByVal b As Long ) As Integer


'---------------------------
' CNC: NC file data related
'---------------------------

' read tool offset value
Declare Function cnc_rdtofs% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As ODBTOFS)

' write tool offset value
Declare Function cnc_wrtofs% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d&)

' read tool offset value(area specified)
Declare Function cnc_rdtofsr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d%, Odb As Any)

' write tool offset value(area specified)
Declare Function cnc_wrtofsr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read work zero offset value
Declare Function cnc_rdzofs% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBZOFS)

' write work zero offset value
Declare Function cnc_wrzofs% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBZOFS)

' read work zero offset value(area specified)
Declare Function cnc_rdzofsr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d%, Odb As IODBZOR)

' write work zero offset value(area specified)
Declare Function cnc_wrzofsr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBZOR)

' read parameter
Declare Function cnc_rdparam% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As Any)

' write parameter
Declare Function cnc_wrparam% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read parameter(area specified)
Declare Function cnc_rdparar% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b%, c%, d%, Odb As Any)

' write parameter(area specified)
Declare Function cnc_wrparas% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read setting data
Declare Function cnc_rdset% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As Any)

' write setting data
Declare Function cnc_wrset% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read setting data(area specified)
Declare Function cnc_rdsetr% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b%, c%, d%, Odb As Any)

' write setting data(area specified)
Declare Function cnc_wrsets% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read pitch error compensation data(area specified)
Declare Function cnc_rdpitchr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBPI)

' write pitch error compensation data(area specified)
Declare Function cnc_wrpitchr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBPI)

' read custom macro variable
Declare Function cnc_rdmacro% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBM)

' write custom macro variable
Declare Function cnc_wrmacro% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d%)

' read custom macro variables(area specified)
Declare Function cnc_rdmacror% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBMR)

' write custom macro variables(area specified)
Declare Function cnc_wrmacror% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBMR)

' read P code macro variable
Declare Function cnc_rdpmacro% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, Odb As ODBPM)

' write P code macro variable
Declare Function cnc_wrpmacro% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b&, ByVal c%)

' read P code macro variables(area specified)
Declare Function cnc_rdpmacror% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b&, ByVal c&, Odb As IODBPR)

' write P code macro variables(area specified)
Declare Function cnc_wrpmacror% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, Idb As IODBPR)

' read tool offset information
Declare Function cnc_rdtofsinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBTLINF)

' read work zero offset information
Declare Function cnc_rdzofsinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read pitch error compensation data information
Declare Function cnc_rdpitchinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read custom macro variable information
Declare Function cnc_rdmacroinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBMVINF)

' read P code macro variable information
Declare Function cnc_rdpmacroinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPMINF)

' read the information for function cnc_rdhsparam()
Declare Function cnc_rdhsprminfo Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long, Odb As HSPINFO) As Integer

' read parameters at the high speed
Declare Function cnc_rdhsparam Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Long, Odb As Any, Odb As Any) As Integer

' read rotary table dynamic fixture offset
Declare Function cnc_rdfixoffs Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Integer, ByVal b As Integer, Odb As ODBFOFS) As Integer


'----------------------------------------
' CNC: Tool life management data related
'----------------------------------------

' read tool life management data(tool group number)
Declare Function cnc_rdgrpid% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBTLIFE1)

' read tool life management data(number of tool groups)
Declare Function cnc_rdngrp% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBTLIFE2)

' read tool life management data(number of tools)
Declare Function cnc_rdntool% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBTLIFE3)

' read tool life management data(tool life)
Declare Function cnc_rdlife% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBTLIFE3)

' read tool life management data(tool lift counter)
Declare Function cnc_rdcount% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBTLIFE3)

' read tool life management data(tool length number-1)
Declare Function cnc_rd1length% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBTLIFE4)

' read tool life management data(tool length number-2)
Declare Function cnc_rd2length% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBTLIFE4)

' read tool life management data(cutter compensation no.-1)
Declare Function cnc_rd1radius% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBTLIFE4)

' read tool life management data(cutter compensation no.-2)
Declare Function cnc_rd2radius% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBTLIFE4)

' read tool life management data(tool information-1)
Declare Function cnc_t1info% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBTLIFE4)

' read tool life management data(tool information-2)
Declare Function cnc_t2info% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBTLIFE4)

' read tool life management data(tool number)
Declare Function cnc_toolnum% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBTLIFE4)

' read tool life management data(tool number, tool life, tool life counter)(area specified)
Declare Function cnc_rdtoolrng% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBTR)

' read tool life management data(all data within group)
Declare Function cnc_rdtoolgrp% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBTG)

' write tool life management data(tool life counter) (area specified)
Declare Function cnc_wrcountr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IDBWRC)

' read tool life management data(used tool group number)
Declare Function cnc_rdusegrpid% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBUSEGR)

' read tool life management data(max. number of tool groups)
Declare Function cnc_rdmaxgrp% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBLFNO)

' read tool life management data(maximum number of tool within group)
Declare Function cnc_rdmaxtool% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBLFNO)

' read tool life management data(used tool no. within group)
Declare Function cnc_rdusetlno% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As ODBTLUSE)

' read tool life management data(tool data1)
Declare Function cnc_rd1tlifedata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As IODBTD)

' read tool life management data(tool data2)
Declare Function cnc_rd2tlifedata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As IODBTD)

' write tool life management data(tool data1)
Declare Function cnc_wr1tlifedata% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBTD)

' write tool life management data(tool data2)
Declare Function cnc_wr2tlifedata% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBTD)

' read tool life management data(tool group information)
Declare Function cnc_rdgrpinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBTGI)

' read tool life management data(tool group information 2)
Declare Function cnc_rdgrpinfo2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBTGI2)

' read tool life management data(tool group information 3)
Declare Function cnc_rdgrpinfo3% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBTGI3)

' write tool life management data(tool group information)
Declare Function cnc_wrgrpinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBTGI)

' write tool life management data(tool group information 2)
Declare Function cnc_wrgrpinfo2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBTGI2)

' write tool life management data(tool group information 3)
Declare Function cnc_wrgrpinfo3% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBTGI3)

' delete tool life management data(tool group)
Declare Function cnc_deltlifegrp% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' insert tool life management data(tool data)
Declare Function cnc_instlifedt% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IDBITD)

' delete tool life management data(tool data)
Declare Function cnc_deltlifedt% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%)

' clear tool life management data(tool life counter, tool information)(area specified)
Declare Function cnc_clrcntinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%)

' read tool life management data(tool group number) 2
Declare Function cnc_rdgrpid2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, Odb As ODBTLIFE5)

' read tool life management data(tool data1) 2
Declare Function cnc_rd1tlifedat2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, Odb As IODBTD2)

' write tool life management data(tool data1) 2
Declare Function cnc_wr1tlifedat2% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBTD2)

' read tool life management data(exchange tool group number)
Declare Function cnc_rdexchgtgrp Lib "fwlib32.dll" (ByVal FlibHndl As Integer, a As Integer, Odb As Any) As Integer

' read tool life management data(tool group information 4)
Declare Function cnc_rdgrpinfo4 Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Integer, ByVal b As Integer, ByVal c As Integer, d As Integer, Odb As Any) As Integer


'-----------------------------------
' CNC: Tool management data related
'-----------------------------------
' new registration of tool management data
Declare Function cnc_regtool% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBTLMNG)

' deletion of tool management data
Declare Function cnc_deltool% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%)

' lead of tool management data
Declare Function cnc_rdtool% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBTLMNG)

' write of tool management data
Declare Function cnc_wrtool% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As IODBTLMNG)

' write of individual data of tool management data
Declare Function cnc_wrtool2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As Any)

' new registration of magazine management data
Declare Function cnc_regmagazine% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, Odb As IODBTLMAG)

' deletion of magazine management data
Declare Function cnc_delmagazine% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, Odb As IODBTLMAG2)

' lead of magazine management data
Declare Function cnc_rdmagazine% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, Odb As IODBTLMAG)

' Individual write of magazine management data
Declare Function cnc_wrmagazine% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%)

'-------------------------------------
' CNC: Operation history data related
'-------------------------------------

' stop logging operation history data
Declare Function cnc_stopophis% Lib "fwlib32.dll" (ByVal FlibHndl%)

' restart logging operation history data
Declare Function cnc_startophis% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read number of operation history data
Declare Function cnc_rdophisno% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read operation history data
Declare Function cnc_rdophistry% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b&, ByVal c&, Odb As Any)

' read number of alarm history data
Declare Function cnc_rdalmhisno% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read alarm history data
Declare Function cnc_rdalmhistry% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b&, ByVal c&, Odb As ODBAHIS)

' read alarm history data 3
Declare Function cnc_rdalmhistry3 Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Integer, ByVal b As Integer, ByVal c As Integer, Odb As ODBAHIS3) As Integer

' clear operation history data
Declare Function cnc_clearophis% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read signals related operation history
Declare Function cnc_rdhissgnl% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBSIG)

' write signals related operation history
Declare Function cnc_wrhissgnl% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBSIG)

' write key history to operation history data
Declare Function cnc_wrkeyhistry Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Byte) As Integer


'-------------
' CNC: Others
'-------------

' read CNC system information
Declare Function cnc_sysinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSYS)

' read CNC status information
Declare Function cnc_statinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBST)

' read alarm status
Declare Function cnc_alarm% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBALM)

' read alarm information
Declare Function cnc_rdalminfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As Any)

' clear CNC alarm
Declare Function cnc_clralm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal id%)

' read modal data
Declare Function cnc_modal% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' read diagnosis data
Declare Function cnc_diagnoss% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As Any)

' read diagnosis data(area specified)
Declare Function cnc_diagnosr% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b%, c%, d%, Odb As Any)

' read A/D conversion data
Declare Function cnc_adcnv% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAD)

' read operator's message
Declare Function cnc_rdopmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' read operator's message2
Declare Function cnc_rdopmsg2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' read operator's message3
Declare Function cnc_rdopmsg3% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As Any)

' read operator message for MAPPS
Declare Function cnc_rdopmsgmps Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Integer, b As Integer, c As Any) As Integer

' set path number(for 4 axes lathes, multi-path)
Declare Function cnc_setpath% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' get path number(for 4 axes lathes, multi-path)
Declare Function cnc_getpath% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b%)

' allocate library handle
Declare Function cnc_allclibhndl% Lib "fwlib32.dll" (FlibHndl%)

' free library handle
Declare Function cnc_freelibhndl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' get custom macro type
Declare Function cnc_getmactype% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' set custom macro type
Declare Function cnc_setmactype% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' get P code macro type
Declare Function cnc_getpmactype% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' set P code macro type
Declare Function cnc_setpmactype% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' get screen status
Declare Function cnc_getcrntscrn% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' change screen mode
Declare Function cnc_slctscrn% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read CNC configuration information
Declare Function cnc_sysconfig% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSYSC)

' read program restart information
Declare Function cnc_rdprstrinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPRS)

' search sequence number for program restart
Declare Function cnc_rstrseqsrch% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b&, ByVal c%, ByVal d%)

' read output signal image of software operator's panel
Declare Function cnc_rdopnlsgnl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As IODBSGNL)

' write output signal of software operator's panel
Declare Function cnc_wropnlsgnl% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBSGNL)

' read general signal image of software operator's panel
Declare Function cnc_rdopnlgnrl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As IODBGNRL)

' write general signal image of software operator's panel
Declare Function cnc_wropnlgnrl% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBGNRL)

' read general signal name of software operator's panel
Declare Function cnc_rdopnlgsname% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As IODBRDNA)

' write general signal name of software operator's panel
Declare Function cnc_wropnlgsname% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBRDNA)

' get detail error
Declare Function cnc_getdtailerr% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBERR)

' read informations of CNC parameter
Declare Function cnc_rdparainfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, Odb As ODBPARAIF)

' read informations of CNC setting data
Declare Function cnc_rdsetinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, Odb As ODBSETIF)

' read informations of CNC diagnose data
Declare Function cnc_rddiaginfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, Odb As ODBDIAGIF)

' read maximum, minimum and total number of CNC parameter
Declare Function cnc_rdparanum% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPARANUM)

' read maximum, minimum and total number of CNC setting data
Declare Function cnc_rdsetnum% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSETNUM)

' read maximum, minimum and total number of CNC diagnose data
Declare Function cnc_rddiagnum% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBDIAGNUM)

' get maximum valid figures and number of decimal places
Declare Function cnc_getfigure% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, c%, d%)

' read F-ROM information on CNC 
Declare Function cnc_rdfrominfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBFINFO)

' start of reading F-ROM data from CNC
Declare Function cnc_fromsvstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$, ByVal c&)

' read F-ROM data from CNC
Declare Function cnc_fromsave% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b As Byte, c&)

' end of reading F-ROM data from CNC
Declare Function cnc_fromsvend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start of writing F-ROM data to CNC
Declare Function cnc_fromldstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&)

' write F-ROM data to CNC
Declare Function cnc_fromload% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b&)

' end of writing F-ROM data to CNC
Declare Function cnc_fromldend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' delete F-ROM data on CNC
Declare Function cnc_fromdelete% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$, ByVal c&)

' read S-RAM information on CNC
Declare Function cnc_rdsraminfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSINFO)

' start of reading S-RAM data from CNC
Declare Function cnc_srambkstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b&)

' read S-RAM data from CNC
Declare Function cnc_srambackup% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b As Byte, c&)

' end of reading S-RAM data from CNC
Declare Function cnc_srambkend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read F-ROM information on CNC 
Declare Function cnc_getfrominfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As ODBFINFORM)

' start of reading F-ROM data from CNC
Declare Function cnc_fromgetstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$)

' read F-ROM data from CNC
Declare Function cnc_fromget% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b As Byte, c&)

' end of reading F-ROM data from CNC
Declare Function cnc_fromgetend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start of writing F-ROM data to CNC
Declare Function cnc_fromputstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' write F-ROM data to CNC
Declare Function cnc_fromput% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b&)

' end of writing F-ROM data to CNC
Declare Function cnc_fromputend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' delete F-ROM data on CNC
Declare Function cnc_fromremove% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$)

' read S-RAM information on CNC
Declare Function cnc_getsraminfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSINFO)

' start of reading S-RAM data from CNC
Declare Function cnc_sramgetstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read S-RAM data from CNC
Declare Function cnc_sramget% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b As Byte, c&)

' end of reading S-RAM data from CNC
Declare Function cnc_sramgetend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start of reading S-RAM data from CNC (2)
Declare Function cnc_sramgetstart2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read S-RAM data from CNC (2)
Declare Function cnc_sramget2% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b As Byte, c&)

' end of reading S-RAM data from CNC (2)
Declare Function cnc_sramgetend2% Lib "fwlib32.dll" (ByVal FlibHndl%)

' transfer a file from host computer to CNC by FTP
Declare Function cnc_dtsvftpget% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b$)

' transfer a file from CNC to host computer by FTP
Declare Function cnc_dtsvftpput% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b$)

' get transfer status for FTP
Declare Function cnc_dtsvftpstat% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read file directory in Data Server
Declare Function cnc_dtsvrdpgdir% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b%, Odb As ODBDSDIR)

' delete files in Data Server
Declare Function cnc_dtsvdelete% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' down load from CNC (transfer a file from CNC to MMC)
Declare Function cnc_dtsvdownload% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' up load to CNC (transfer a file from MMC to CNC)
Declare Function cnc_dtsvupload% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' close upload/download between Data Server and CNC
Declare Function cnc_dtsvcnclupdn% Lib "fwlib32.dll" (ByVal FlibHndl%)

' get transfer status for up/down load
Declare Function cnc_dtsvupdnstat% Lib "fwlib32.dll" (ByVal FlibHndl%)

' get file name for DNC operation in Data Server
Declare Function cnc_dtsvgetdncpg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' set program number of DNC oparation to CNC
Declare Function cnc_dtsvsetdncpg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read setting data for Data Server
Declare Function cnc_dtsvrdset% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBDSSET)

' write setting data for Data Server
Declare Function cnc_dtsvwrset% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBDSSET)

' check hard disk in Data Server
Declare Function cnc_dtsvchkdsk% Lib "fwlib32.dll" (ByVal FlibHndl%)

' format hard disk in Data Server
Declare Function cnc_dtsvhdformat% Lib "fwlib32.dll" (ByVal FlibHndl%)

' save interface area in Data Server
Declare Function cnc_dtsvsavecram% Lib "fwlib32.dll" (ByVal FlibHndl%)

' get interface area in Data Server
' Declare Function cnc_dtsvrdcram% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b&, ByVal c$)  '980701
Declare Function cnc_dtsvrdcram% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b&, c As Byte)   '980701

' read maintenance information for Data Server
Declare Function cnc_dtsvmntinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBDSMNT)

' get Data Server mode
Declare Function cnc_dtsvgetmode% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' set Data Server mode
Declare Function cnc_dtsvsetmode% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read error message for Data Server
Declare Function cnc_dtsvrderrmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$)

' transfar file from PC to Data Server  1999.8.23
Declare Function cnc_dtsvwrfile% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b$, ByVal c%)

' transfar file from Data Server to PC  1999.8.23
Declare Function cnc_dtsvrdfile% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b$, ByVal c%)

' read the loop gain for each axis
Declare Function cnc_rdloopgain% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the actual current for each axis
Declare Function cnc_rdcurrent% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read the actual speed for each axis
Declare Function cnc_rdsrvspeed% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the operation mode
Declare Function cnc_rdopmode% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read the position deviation S
Declare Function cnc_rdposerrs% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the position deviation S1 and S2
Declare Function cnc_rdposerrs2% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPSER)

' read the position deviation Z in the rigid tap mode
Declare Function cnc_rdposerrz% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the synchronous error in the synchronous control mode
Declare Function cnc_rdsynerrsy% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the synchronous error in the rigid tap mode
Declare Function cnc_rdsynerrrg% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the spindle alarm
' Declare Function cnc_rdspdlalm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$) '980701
Declare Function cnc_rdspdlalm% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)  '980701

' read the control input signal
Declare Function cnc_rdctrldi% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSPDI)

' read the control output signal
Declare Function cnc_rdctrldo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSPDO)

' read the number of controled spindle
Declare Function cnc_rdnspdl% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read Servo feedback multiplication data/
Declare Function cnc_rdsvfeedback% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBSVFBACK)

' read data from FANUC BUS
Declare Function cnc_rdfbusmem% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d&, Odb As Any)

' write data to FANUC BUS
Declare Function cnc_wrfbusmem% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d&, Odb As Any)

' read the parameter of wave diagnosis
Declare Function cnc_rdwaveprm% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As Any)

' write the parameter of wave diagnosis
Declare Function cnc_wrwaveprm% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As Any)

' read the parameter of wave diagnosis 2
Declare Function cnc_rdwaveprm2% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As Any)

' write the parameter of wave diagnosis 2
Declare Function cnc_wrwaveprm2% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As Any)

' start the sampling for wave diagnosis
Declare Function cnc_wavestart% Lib "fwlib32.dll" (ByVal FlibHndl%)

' stop the sampling for wave diagnosis
Declare Function cnc_wavestop% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read the status of wave diagnosis
Declare Function cnc_wavestat% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read the data of wave diagnosis
Declare Function cnc_rdwavedata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, d&, Odb As Any)

' read M-code group data
Declare Function cnc_rdmgrpdata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As ODBMGRP)

' write M-code group data
Declare Function cnc_wrmgrpdata% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IDBMGRP)

' read processing time stamp data
Declare Function cnc_rdproctime% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPTIME)

' read program directory for processing time data
Declare Function cnc_rdprgdirtime% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, b%, Odb As PRGDIRTM)

' read program directory 2
#If ONO8D Then
Declare Function cnc_rdprogdir2% Lib "fwlib32.dll" Alias "cnc_rdprogdir2o8" (ByVal FlibHndl%, ByVal a%, b&, c%, Odb As Any)
#Else
Declare Function cnc_rdprogdir2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, c%, Odb As Any)
#End If

' read program directory 3
Declare Function cnc_rdprogdir3% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b&, c%, Odb As Any)

' read program directory 4
Declare Function cnc_rdprogdir4% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, c%, Odb As Any)

' rename NC program
Declare Function cnc_renameprog% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b& )

' read DNC file name for DNC1, DNC2, OSI-Ethernet
Declare Function cnc_rddncfname% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' write DNC file name for DNC1, DNC2, OSI-Ethernet
Declare Function cnc_wrdncfname% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read communication parameter for DNC1, DNC2, OSI-Ethernet
Declare Function cnc_rdcomparam% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBCPRM)

' write communication parameter for DNC1, DNC2, OSI-Ethernet
Declare Function cnc_wrcomparam% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBCPRM)

' read log message for DNC2
Declare Function cnc_rdcomlogmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read operator message for DNC1, DNC2
Declare Function cnc_rdcomopemsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read recieve message for OSI-Ethernet
Declare Function cnc_rdrcvmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read send message for OSI-Ethernet
Declare Function cnc_rdsndmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' send message for OSI-Ethernet
Declare Function cnc_sendmessage% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' clear message buffer for OSI-Ethernet
Declare Function cnc_clrmsgbuff% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read message recieve status for OSI-Ethernet
Declare Function cnc_rdrcvstat% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read interrupt check
Declare Function cnc_rdintchk% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d%, Odb As IODBINT)

' write interrupt check
Declare Function cnc_wrintchk% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBINT)

' read interrupt check information
Declare Function cnc_rdintinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read work coordinate shift
Declare Function cnc_rdwkcdshft% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As IODBWCSF)

' write work coordinate shift
Declare Function cnc_wrwkcdshft% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBWCSF)

' read work coordinate shift measure
Declare Function cnc_rdwkcdsfms% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As IODBWCSF)

' write work coordinate shift measure
Declare Function cnc_wrwkcdsfms% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBWCSF)

' stop the sampling for operator message history
Declare Function cnc_stopomhis% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start the sampling for operator message history
Declare Function cnc_startomhis% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read operator message history information
Declare Function cnc_rdomhisinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBOMIF)

' read operator message history
Declare Function cnc_rdomhistry% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b&, Odb As Any)

' clear operator message history
Declare Function cnc_clearomhis% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read b-axis tool offset value(area specified)
Declare Function cnc_rdbtofsr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d%, Odb As IODBBTO)

' write b-axis tool offset value(area specified)
Declare Function cnc_wrbtofsr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBBTO)

' read b-axis tool offset information
Declare Function cnc_rdbtofsinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBBTLINF)

' read b-axis command
Declare Function cnc_rdbaxis% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As Any)

' read current program and its pointer
Declare Function cnc_rdactpt% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, b&)

' read current program and its pointer  and UV macro pointer 
Declare Function cnc_rduvactpt% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, b&, c&)

' set current program and its pointer
Declare Function cnc_wractpt% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b%, c&)

' read CNC system soft series and version
Declare Function cnc_rdsyssoft% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSYSS)

' read CNC system soft series and version
Declare Function cnc_rdsyssoft2% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSYSS2)

' read CNC module configuration information
Declare Function cnc_rdmdlconfig% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBMDLC)

' read CNC module configuration information 2
' Declare Function cnc_rdmdlconfig2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)      '980701
Declare Function cnc_rdmdlconfig2% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)       '980701

' read processing condition file (processing data)
Declare Function cnc_rdpscdproc% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBPSCD)

' write processing condition file (processing data)
Declare Function cnc_wrpscdproc% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Idb As IODBPSCD)

' read processing condition file (piercing data)
Declare Function cnc_rdpscdpirc% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBPIRC)

' write processing condition file (piercing data)
Declare Function cnc_wrpscdpirc% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Idb As IODBPIRC)

' read processing condition file (edging data)
Declare Function cnc_rdpscdedge% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBEDGE)

' write processing condition file (edging data)
Declare Function cnc_wrpscdedge% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Idb As IODBEDGE)

' read processing condition file (slope data)
Declare Function cnc_rdpscdslop% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBSLOP)

' write processing condition file (slope data)
Declare Function cnc_wrpscdslop% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Idb As IODBSLOP)

' read power controll duty data
Declare Function cnc_rdlpwrdty% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBLPWDT)

' write power controll duty data
Declare Function cnc_wrlpwrdty% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBLPWDT)

' read laser power data
Declare Function cnc_rdlpwrdat% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBLOPDT)

' read power complement
Declare Function cnc_rdlpwrcpst% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' write power complement
Declare Function cnc_wrlpwrcpst% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read laser assist gas selection
Declare Function cnc_rdlagslt% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBLAGSL)

' write laser assist gas selection
Declare Function cnc_wrlagslt% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBLAGSL)

' read laser assist gas flow
Declare Function cnc_rdlagst% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBLAGST)

' write laser assist gas flow
Declare Function cnc_wrlagst% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBLAGST)

' read laser power for edge processing
Declare Function cnc_rdledgprc% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBLEGPR)

' write laser power for edge processing
Declare Function cnc_wrledgprc% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBLEGPR)

' read laser power for piercing
Declare Function cnc_rdlprcprc% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBLPCPR)

' write laser power for piercing
Declare Function cnc_wrlprcprc% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBLPCPR)

' read laser command data
Declare Function cnc_rdlcmddat% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBLCMDT)

' read displacement
Declare Function cnc_rdldsplc% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' write displacement
Declare Function cnc_wrldsplc% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read error for axis z
Declare Function cnc_rdlerrz% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read active number
Declare Function cnc_rdlactnum% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBLACTN)

' read laser comment
Declare Function cnc_rdlcmmt% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBLCMMT)

' read laser power select
Declare Function cnc_rdlpwrslt% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' write laser power select
Declare Function cnc_wrlpwrslt% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read laser power controll
Declare Function cnc_rdlpwrctrl% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' write laser power controll
Declare Function cnc_wrlpwrctrl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read power correction factor history data
Declare Function cnc_rdpwofsthis% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b&, Odb As Any)

' read management time
Declare Function cnc_rdmngtime% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b&, Odb As Any)

' write management time
Declare Function cnc_wrmngtime% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, Odb As Any)

' read data related to electrical discharge at power correction ends
Declare Function cnc_rddischarge% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As Any)

' read alarm history data related to electrical discharg
Declare Function cnc_rddischrgalm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b&, Odb As Any)

' get date and time from cnc
Declare Function cnc_gettimer% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As Any)

' set date and time for cnc
Declare Function cnc_settimer% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As Any)

' read tool control data
Declare Function cnc_rdtlctldata% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBTLCTL)

' write tool control data
Declare Function cnc_wrtlctldata% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBTLCTL)

' read tool information
Declare Function cnc_rdtoolinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPTLINF)

' read tool data
Declare Function cnc_rdtooldata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBTLDT)

' read tool data
Declare Function cnc_wrtooldata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Idb As IODBTLDT)

' read multi tool data
Declare Function cnc_rdmultitldt% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBMLTTL)

' write multi tool data
Declare Function cnc_wrmultitldt% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Idb As IODBMLTTL)

' read multi tap data
Declare Function cnc_rdmtapdata Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Integer, b As Integer, Odb As IODBMTAP) As Integer

' write multi tap data
Declare Function cnc_wrmtapdata Lib "fwlib32.dll" (ByVal FlibHndl As Integer, ByVal a As Integer, b As Integer, Idb As IODBMTAP) As Integer

' read safetyzone data
Declare Function cnc_rdsafetyzone% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBSAFE)

' write safetyzone data
Declare Function cnc_wrsafetyzone% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Idb As IODBSAFE)

' read toolzone data
Declare Function cnc_rdtoolzone% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As IODBTLZN)

' write toolzone data
Declare Function cnc_wrtoolzone% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Idb As IODBTLZN)

' read active toolzone data
Declare Function cnc_rdacttlzone% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBACTTLZN)

' read setzone number
Declare Function cnc_rdsetzone% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' write setzone number
Declare Function cnc_wrsetzone% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read multi-piece machining number
Declare Function cnc_rdmultipieceno% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read axis name CNC
Declare Function cnc_rdaxisname% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, Odb As ODBAXISNAME)

' read spindle name
Declare Function cnc_rdspdlname% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, Odb As ODBSPDLNAME)

' read SRAM variable area for C language executor
Declare Function cnc_rdcexesram% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b As Byte, c&)

' write SRAM variable area for C language executor
Declare Function cnc_wrcexesram% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b As Byte, c&)

' read maximum size and linear address of SRAM variable area for C language executor
Declare Function cnc_cexesraminfo% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b&, c&)

' read maximum size of SRAM variable area for C language executor
Declare Function cnc_cexesramsize% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read additional workpiece coordinate systems number
Declare Function cnc_rdcoordnum% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' converts from FANUC code to Shift JIS code
Declare Function cnc_ftosjis% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b As Byte)

' cycle start
Declare Function cnc_start% Lib "fwlib32.dll" (ByVal FlibHndl%)

' reset CNC
Declare Function cnc_reset% Lib "fwlib32.dll" (ByVal FlibHndl%)

' reset CNC 2
Declare Function cnc_reset2% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Display of optional message
Declare Function cnc_dispoptmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' Reading of answer for optional message display
Declare Function cnc_optmsgans% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' Get CNC Model
Declare Function cnc_getcncmodel% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read number of repeats
Declare Function cnc_rdrepeatval Lib "fwlib32.dll" (ByVal FlibHndl As Integer, a As Long) As Integer


'------------------
' CNC : SERCOS I/F
'------------------

' Get reservation of service channel for SERCOS I/F
Declare Function cnc_srcsrsvchnl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Read ID information of SERCOS I/F
Declare Function cnc_srcsrdidinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b%, ByVal c%, Odb As IODBIDINF)

' Write ID information of SERCOS I/F
Declare Function cnc_srcswridinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBIDINF)

' Start of reading operation data from drive of SERCOS I/F
Declare Function cnc_srcsstartrd% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b%)

' Start of writing operation data to drive of SERCOS I/F
Declare Function cnc_srcsstartwrt% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b%)

' Stop of reading/writing operation data from/to drive of SERCOS I/F
Declare Function cnc_srcsstopexec% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Get execution status of reading/writing operation data of SERCOS I/F
Declare Function cnc_srcsrdexstat% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSRCSST)

' Read operation data from data buffer for SERCOS I/F
Declare Function cnc_srcsrdopdata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b&, Odb As Any)

' Write operation data to data buffer for SERCOS I/F
Declare Function cnc_srcswropdata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b&, Idb As Any)

' Free reservation of service channel for SERCOS I/F
Declare Function cnc_srcsfreechnl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Read drive assign of SERCOS I/F
Declare Function cnc_srcsrdlayout% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSRCSLYT)

' Read communication phase of drive of SERCOS I/F
Declare Function cnc_srcsrddrvcp% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)


'---------------------------
' CNC : Graphic command data
'---------------------------

' Start drawing position
Declare Function cnc_startdrawpos% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Stop drawing position
Declare Function cnc_stopdrawpos% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Start dynamic graphic
Declare Function cnc_startdyngrph% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Stop dynamic graphic
Declare Function cnc_stopdyngrph% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Read graphic command data
Declare Function cnc_rdgrphcmd% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b%)

' Update graphic command read pointer
Declare Function cnc_wrgrphcmdptr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)
' Read cancel flag
Declare Function cnc_rdgrphcanflg% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' Clear graphic command
Declare Function cnc_clrgrphcmd% Lib "fwlib32.dll" (ByVal FlibHndl%)



'---------------------------
' CNC : Servo learning data
'---------------------------

' Servo learning data read start
Declare Function cnc_svdtstartrd% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' Servo learning data write start
Declare Function cnc_svdtstartwr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' Servo learning data read end
Declare Function cnc_svdtendrd% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo learning data write end
Declare Function cnc_svdtendwr% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo learning data read/write stop
Declare Function cnc_svdtstopexec% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo learning data read from I/F buffer
Declare Function cnc_svdtrddata% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b&, Odb As Any)

' Servo learning data write from I/F buffer
Declare Function cnc_svdtwrdata% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b&, Odb As Any)


'----------------------------
' CNC : Servo Guide
'----------------------------
' Servo Guide (Channel data set)
Declare Function cnc_sdsetchnl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As IODBCHAN)

' Servo Guide (Channel data clear)
Declare Function cnc_sdclrchnl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo Guide (Sampling start)
Declare Function cnc_sdstartsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, c%)

' Servo Guide (Sampling cancel)
Declare Function cnc_sdcancelsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo Guide (read Sampling data)
Declare Function cnc_sdreadsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b&, Odb As Any)

' Servo Guide (Sampling end)
Declare Function cnc_sdendsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo Guide (read 1 shot data)
Declare Function cnc_sdread1shot% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' Servo feedback data (Channel data set)
Declare Function cnc_sfbsetchnl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, Odb As IDBSFBCHAN)

' Servo feedback data (Channel data clear)
Declare Function cnc_sfbclrchnl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo feedback data (Sampling start)
Declare Function cnc_sfbstartsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b& )

' Servo feedback data (Sampling cancel)
Declare Function cnc_sfbcancelsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo feedback data (read Sampling data)
Declare Function cnc_sfbreadsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b&, Odb As Any)

' Servo feedback data (Sampling end)
Declare Function cnc_sfbendsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%)


'---------------------------
' CNC : NC display function
'---------------------------

' Start NC display
Declare Function cnc_startnccmd% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Start NC display (2)
Declare Function cnc_startnccmd2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' Stop NC display
Declare Function cnc_stopnccmd% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Get NC display mode
Declare Function cnc_getdspmode% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' Set current display screen
Declare Function cnc_setcurscrn% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' Change status of SDF
Declare Function cnc_sdfstatchg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d&)

' Set manager application for SDF
Declare Function cnc_sdfmnghwnd% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, ByVal c&)


'-----------------------------------
' CNC : Remote diagnostics function
'-----------------------------------

' Start remote diagnostics function
Declare Function cnc_startrmtdgn% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Stop remote diagnostics function
Declare Function cnc_stoprmtdgn% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Read data from remote diagnostics I/F
Declare Function cnc_rdrmtdgn% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, ByVal b$)

' Write data to remote diagnostics I/F
Declare Function cnc_wrrmtdgn% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, ByVal b$)

' Set CommStatus of remote diagnostics I/F area
Declare Function cnc_wrcommstatus% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' Check remote diagnostics I/F
Declare Function cnc_chkrmtdgn% Lib "fwlib32.dll" (ByVal FlibHndl%)

'-----
' PMC
'-----

' read message from PMC to MMC
Declare Function pmc_rdmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b%)

' write message from MMC to PMC
Declare Function pmc_wrmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%)

' read message from PMC to MMC(conditional)
Declare Function pmc_crdmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b%)

' write message from MMC to PMC(conditional)
Declare Function pmc_cwrmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%)

' read PMC data(area specified)
Declare Function pmc_rdpmcrng% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d&, ByVal e&, Odb As Any)

' write PMC data(area specified)
Declare Function pmc_wrpmcrng% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, Idb As Any)

' read data from extended backup memory
' Declare Function pmc_rdkpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b$, ByVal c&)         '980701
Declare Function pmc_rdkpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b As Byte, ByVal c&)          '980701

' write data to extended backup memory
' Declare Function pmc_wrkpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b$, ByVal c&)         '980701
Declare Function pmc_wrkpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b As Byte, ByVal c&)          '980701

' read data from extended backup memory
Declare Function pmc_rdkpm2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b As Byte, ByVal c&)

' write data to extended backup memory
Declare Function pmc_wrkpm2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b As Byte, ByVal c&)

' read maximum size of extended backup memory
Declare Function pmc_kpmsiz% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read informations of PMC data
Declare Function pmc_rdpmcinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBPMCINF)

' read PMC parameter data table contorol data
Declare Function pmc_rdcntldata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBPMCCNTL)

' write PMC parameter data table contorol data
Declare Function pmc_wrcntldata% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBPMCCNTL)

' read PMC parameter data table contorol data group number
Declare Function pmc_rdcntlgrp% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' write PMC parameter data table contorol data group number
Declare Function pmc_wrcntlgrp% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read PMC alarm message
Declare Function pmc_rdalmmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, c%, Odb As Any)

' get detail error for pmc
Declare Function pmc_getdtailerr% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPMCERR)

' read PMC memory data
Declare Function pmc_rdpmcmem% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, ByVal c&, Odb As Any)

' write PMC memory data
Declare Function pmc_wrpmcmem% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, ByVal c&, Idb As Any)

' read pmc title data
Declare Function pmc_rdpmctitle% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPMCTITLE)

' read PMC parameter start
Declare Function pmc_rdprmstart Lib "fwlib32.dll" (ByVal FlibHndl As Integer) As Integer

' read PMC parameter
Declare Function pmc_rdpmcparam Lib "fwlib32.dll" (ByVal FlibHndl As Integer, a As Long, b As Byte) As Integer

' read PMC parameter end
Declare Function pmc_rdprmend Lib "fwlib32.dll" (ByVal FlibHndl As Integer) As Integer

' write PMC parameter start
Declare Function pmc_wrprmstart Lib "fwlib32.dll" (ByVal FlibHndl As Integer) As Integer

' write PMC parameter
Declare Function pmc_wrpmcparam Lib "fwlib32.dll" (ByVal FlibHndl As Integer, a As Long, b As Byte) As Integer

' write PMC parameter end
Declare Function pmc_wrprmend Lib "fwlib32.dll" (ByVal FlibHndl As Integer) As Integer


'----------------------------
' PMC : PROFIBUS function
'----------------------------

' read PROFIBUS configration data
Declare Function pmc_prfrdconfig% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPRFCNF)

' read bus parameter for master function
Declare Function pmc_prfrdbusprm% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBBUSPRM)

' write bus parameter for master function
Declare Function pmc_prfwrbusprm% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBBUSPRM)

' read slave parameter for master function
Declare Function pmc_prfrdslvprm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As Any)

' write slave parameter for master function
Declare Function pmc_prfwrslvprm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read allocation address for master function
Declare Function pmc_prfrdallcadr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As IODBPRFADR)

' set allocation address for master function
Declare Function pmc_prfwrallcadr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBPRFADR)

' read allocation address for slave function
Declare Function pmc_prfrdslvaddr% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBSLVADR)

' set allocation address for slave function
Declare Function pmc_prfwrslvaddr% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBSLVADR)

' read status for slave function
Declare Function pmc_prfrdslvstat% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSLVST)


'-----------------------------------
' CB : CUSTOMER'S BOARD function    
'-----------------------------------

' start downloading Customer's board data
Declare Function cb_dwnstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&)

' download Customer's board data
Declare Function cb_download% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, b As Byte)

' end of downloading Customer's board data
Declare Function cb_dwnend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start uploading Customer's board data
Declare Function cb_upstart% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b&)

' upload Customer's board data
Declare Function cb_upload% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, b As Byte)

' end of uploading Customer's board data
Declare Function cb_upend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' get transfer informations
Declare Function cb_transinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBTRANSINFO)



'-----------------------------------------------
' DS : Data server & Ethernet board function    
'-----------------------------------------------

' read the parameter of the Ethernet board
Declare Function etb_rdparam% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As Any)

' write the parameter of the Ethernet board
Declare Function etb_wrparam% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As Any)

' read the error message of the Ethernet board
Declare Function etb_rderrmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBETMSG)

' read the mode of the Data Server
Declare Function ds_rdmode% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' write the mode of the Data Server
Declare Function ds_wrmode% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read information of the Data Server's HDD
Declare Function ds_rdhddinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBHDDINF)

' read the file list of the Data Server's HDD
Declare Function ds_rdhdddir% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, ByVal b&, c%, Odb As Any)

' delete the file of the Data Serve's HDD
Declare Function ds_delhddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' copy the file of the Data Server's HDD
Declare Function ds_copyhddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b As Byte)

' change the file name of the Data Server's HDD
Declare Function ds_renhddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b As Byte)

' execute the PUT command of the FTP
Declare Function ds_puthddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b As Byte)

' execute the MPUT command of the FTP
Declare Function ds_mputhddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' read information of the host
Declare Function ds_rdhostinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, ByVal b&)

' read the file list of the host
Declare Function ds_rdhostdir% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, c%, Odb As Any, ByVal e&)

' read the file list of the host 2
Declare Function ds_rdhostdir2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, c%, d&, Odb As Any, ByVal f&)

' delete the file of the host
Declare Function ds_delhostfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, ByVal b&)

' execute the GET command of the FTP
Declare Function ds_gethostfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b As Byte)

' execute the MGET command of the FTP
Declare Function ds_mgethostfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' read the execution result
Declare Function ds_rdresult% Lib "fwlib32.dll" (ByVal FlibHndl%)

' stop the execution of the command
Declare Function ds_cancel% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read the file from the Data Server
Declare Function ds_rdncfile% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b As Byte)

' read the file from the Data Server 2
Declare Function ds_rdncfile2% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' write the file to the Data Server
Declare Function ds_wrncfile% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&)

' read the file name for the DNC operation in the Data Server's HDD
Declare Function ds_rddnchddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' write the file name for the DNC operation in the Data Server's HDD
Declare Function ds_wrdnchddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' read the file name for the DNC operation in the host
Declare Function ds_rddnchostfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b As Byte)

' write the file name for the DNC operation in the host
Declare Function ds_wrdnchostfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' read the connecting host number
Declare Function ds_rdhostno% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read maintenance information
Declare Function ds_rdmntinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As DSMNTINFO)

' check the Data Server's HDD
Declare Function ds_checkhdd% Lib "fwlib32.dll" (ByVal FlibHndl%)

' format the Data Server's HDD
Declare Function ds_formathdd% Lib "fwlib32.dll" (ByVal FlibHndl%)

' create the directory in the Data Server's HDD
Declare Function ds_makehdddir% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' delete directory in the Data Server's HDD
Declare Function ds_delhdddir% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' change the current directory
Declare Function ds_chghdddir% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' execute the PUT command according to the list file
Declare Function ds_lputhddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' delete files according to the list file
Declare Function ds_ldelhddfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' execute the GET command according to the list file
Declare Function ds_lgethostfile% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' read the directory for M198 operation
Declare Function ds_rdm198hdddir% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte)

' write the directory for M198 operation
Declare Function ds_wrm198hdddir% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read the connecting host number for the M198 operation
Declare Function ds_rdm198host% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' write the connecting host number for the M198 operation
Declare Function ds_wrm198host% Lib "fwlib32.dll" (ByVal FlibHndl%)

' write the connecting host number
Declare Function ds_wrhostno% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)


'--------------------------
' HSSB multiple connection
'--------------------------

' read number of node
Declare Function cnc_rdnodenum% Lib "fwlib32.dll" (a&)

' read node informations
Declare Function cnc_rdnodeinfo% Lib "fwlib32.dll" (ByVal a&, Odb As ODBNODE)

' set default node number
Declare Function cnc_setdefnode% Lib "fwlib32.dll" (ByVal a&)

' allocate library handle 2
Declare Function cnc_allclibhndl2% Lib "fwlib32.dll" (ByVal a&, FlibHndl%)

