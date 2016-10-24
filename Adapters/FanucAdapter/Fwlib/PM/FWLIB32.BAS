Attribute VB_Name = "Module1"
'------------------------------------------------------
'
' CNC/PMC Data Window Library for Power Mate
'
' Copyright (C) 1996 by FANUC LTD. All rights reserved.
'
'------------------------------------------------------


'----------------------
'  Axis Define
'----------------------
'Global Const MAX_AXIS = 2              ' Power Mate-D TYPE
Global Const MAX_AXIS = 6               ' Power Mate-H TYPE
Global Const ALL_AXES = -1

'----------------------
'  Error Codes
'----------------------
Global Const EW_NODLL = -15             ' DLL not exist error
Global Const EW_INIERR = -14            ' error in APi library inital valiefile
Global Const EW_ITLOW = -13             ' low temperature alarm of intelligent terminal
Global Const EW_ITHIGHT = -12           ' hight temperature alarm of intelligent terminal
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

'----------------------
'
'  Structure Template
'
'----------------------
'------------------
' CNC: Control axis
'------------------

' cnc_actf:read actual axis feedrate(F)
Type ODBACT
  nDummy(0 To 1) As Integer     ' dummy
  lData As Long                 ' actual feed / actual spindle
End Type

' cnc_absolute:read absolute axis position
' cnc_machine:read machine axis position
' cnc_relative:read relative axis position
' cnc_distance:read distance to go
' cnc_skip:read skip position
' cnc_srvdelay:read servo delay value
' cnc_accdecdly:read acceleration/deceleration delay value
Type ODBAXIS
  nDummy As Integer             ' dummy
  nType As Integer              ' axis number
  lData(0 To 5) As Long         ' data value
End Type                        ' In case that the number of axes is 6

' cnc_rddynamic:read all dynamic data
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
  lAbsolute(0 To 5) As Long     ' absolute position
  lMachine(0 To 5) As Long      ' machine position
  lRelative(0 To 5) As Long     ' relative position
  lDistance(0 To 5) As Long     ' distance to go
End Type                        ' In case that the number of axes is 6

' cnc_wrrelpos:set origin / preset relative axis position
Type IDBWRR
  nDatano As Integer            ' dummy
  nType As Integer              ' axis number
  lData(0 To 5) As Long         ' preset data
End Type                        ' In case that the number of axes is 6


'----------------------
' CNC: Program related
'----------------------

' cnc_upload:upload NC program
Type ODBUP
  nDummy(0 To 1) As Integer     ' dummy
  sData As String * 256         ' data
End Type                        ' In case that the number of data is 256

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
Type ODBPRO
  nDummy(0 To 1) As Integer     ' dummy
  nData As Integer              ' running program number
  nMdata As Integer             ' main program number
End Type

' cnc_rdseqnum:read sequence number under execution
Type ODBSEQ
  nDummy(0 To 1) As Integer     ' dummy
  lData As Long                 ' sequence number
End Type

Type ODBMDIP
  nMdiprog As Integer           ' exec. program number
  lMdipntr As Long              ' exec. pointer
  nCrntprog As Integer          ' prepare program number
  lCrntpntr As Long             ' prepare pointer
End Type


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

' cnc_rdparam:read parameter
' cnc_wrparam:write parameter
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
  sCdatas(0 To 5) As Byte
End Type                        ' In case that the number of axes is 6

Type IODBPSD5
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  nIdatas(0 To 5) As Integer
End Type                        ' In case that the number of axes is 6

Type IODBPSD6
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  lLdatas(0 To 5) As Long
End Type                        ' In case that the number of axes is 6

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

' cnc_rdtofsinfo:read tool offset information
Type ODBTLINF
  nOfs_type As Integer
  nUse_no As Integer
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

' cnc_rdmacroinfo:read custom macro variable information
Type ODBMVINF
  nUse_no1 As Integer
  nUse_no2 As Integer
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
  nDummy(0 To 1) As Integer     ' dummy
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
  sG_rdata(0 To 20) As Byte
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
  sCdatas(0 To 5) As Byte
End Type                        ' In case that the number of axes is 6

Type ODBDGN5
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  nIdatas(0 To 5) As Integer
End Type                        ' In case that the number of axes is 6

Type ODBDGN6
  nDatano As Integer            ' data number
  nType As Integer              ' axis number
  lLdatas(0 To 5) As Long
End Type                        ' In case that the number of axes is 6

' cnc_rdopmsg:read operator's message
Type MSG
  nDatano As Integer            ' operator's message number
  nType As Integer              ' operator's message type
  nChar_num As Integer          ' message string length
  sData As String * 256         ' operator's message string
End Type                        ' In case that the data length is 256

' cnc_rdopmsg3:read operator's message
Type MSG3
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
  nSv1min As Integer
  nSv3min As Integer
  nSv5min As Integer
  nSicmin As Integer
  nPosmin As Integer
  nSubmin As Integer
  nHdiio As Integer
  nDummy2(0 To 31) As Integer
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


'------------------------------
' MAXIS: Axis Movement Control
'------------------------------

' cnc_opdi:signal operation command
Type ODBOPDI1
  nAxis As Integer      ' axis number
  cData As Byte         ' signal data
End Type

Type ODBOPDI2
  nAxis As Integer      ' axis number
  nData As Integer      ' signal data
End Type

Type ODBOPDI3
  nAxis As Integer      ' axis number
  lData As Long         ' signal data
End Type

' cnc_abspoint:absolute movement
' cnc_incpoint:incremental movement
' cnc_dwell:dwell
' cnc_coordre:coordinate establihment
Type ODBPOS
  nData As Integer      ' axis number
  lData As Long         ' data value
End Type

' cnc_refpoint:reference point return
' cnc_abspoint:absolute movement
' cnc_incpoint:incremental movement
' cnc_dwell:dwell
' cnc_coordre:coordinate establihment
' cnc_exebufstat:reading of the executive buffer condition
Type ODBEXEC
  nDummy As Integer
  sData(0 To 1) As String * 8   ' the infomation of the executive condition of
End Type

' cnc_finstate:Reading of the execution completion condition
' cnc_setfin:Release of the reading mode of the execution completion condition
Type ODBFIN
  nDummy As Integer
  sData As String * 8   ' the infomation of the complete notice flag
End Type                ' condition of each PATH


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


'-------------------
' CNC: Control axis
'-------------------

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
Declare Function cnc_rddynamic% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' set origin / preset relative axis position
Declare Function cnc_wrrelpos% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IDBWRR)


'----------------------
' CNC: Program related
'----------------------

' start downloading NC program
Declare Function cnc_dwnstart% Lib "fwlib32.dll" (ByVal FlibHndl%)

' download NC program
Declare Function cnc_download% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$, ByVal b%)

' end of downloading NC program
Declare Function cnc_dwnend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start uploading NC program
Declare Function cnc_upstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' upload NC program
Declare Function cnc_upload% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBUP, a&)

' end of uploading NC program
Declare Function cnc_upend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' search specified program
Declare Function cnc_search% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' delete all programs
Declare Function cnc_delall% Lib "fwlib32.dll" (ByVal FlibHndl%)

' delete specified program
Declare Function cnc_delete% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read program directory
Declare Function cnc_rdprogdir% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d&, Odb As PRGDIR)

' read program information
Declare Function cnc_rdproginfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' read program number under execution
Declare Function cnc_rdprgnum% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPRO)

' read sequence number under execution
Declare Function cnc_rdseqnum% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSEQ)

' search specified sequence number
Declare Function cnc_seqsrch% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&)

' rewind cursor of NC program
Declare Function cnc_rewind% Lib "fwlib32.dll" (ByVal FlibHndl%)

' read program under execution
Declare Function cnc_rdexecprog% Lib "fwlib32.dll" (ByVal FlibHndl%, a&, b%, ByVal c$)

' write program for MDI operation
Declare Function cnc_wrmdiprog% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$)

' read execution pointer for MDI operation
Declare Function cnc_rdmdipntr% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBMDIP)

' write execution pointer for MDI operation
Declare Function cnc_wrmdipntr% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&)


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

' read parameter
Declare Function cnc_rdparam% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As Any)

' write parameter
Declare Function cnc_wrparam% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read parameter(area specified)
Declare Function cnc_rdparar% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b%, c%, d%, Odb As Any)

' write parameter(area specified)
Declare Function cnc_wrparas% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read setting data(area specified)
Declare Function cnc_rdsetr% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b%, c%, d%, Odb As Any)

' write setting data(area specified)
Declare Function cnc_wrsets% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As Any)

' read custom macro variable
Declare Function cnc_rdmacro% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBM)

' write custom macro variable
Declare Function cnc_wrmacro% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d%)

' read custom macro variables(area specified)
Declare Function cnc_rdmacror% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As IODBMR)

' write custom macro variables(area specified)
Declare Function cnc_wrmacror% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IODBMR)

' read tool offset information
Declare Function cnc_rdtofsinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBTLINF)

' read informations of CNC parameter
Declare Function cnc_rdparainfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, Odb As ODBPARAIF)

' read custom macro variable information
Declare Function cnc_rdmacroinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBMVINF)


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

' clear operation history data
Declare Function cnc_clearophis% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read signals related operation history
Declare Function cnc_rdhissgnl% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As IODBSIG)

' write signals related operation history
Declare Function cnc_wrhissgnl% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As IODBSIG)


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

' read modal data
Declare Function cnc_modal% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' read diagnosis data
Declare Function cnc_diagnoss% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As Any)

' read diagnosis data(area specified)
Declare Function cnc_diagnosr% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b%, c%, d%, Odb As Any)

' read operator's message
Declare Function cnc_rdopmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As MSG)

' read operator's message3
Declare Function cnc_rdopmsg3% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As MSG3)

' allocate library handle
Declare Function cnc_allclibhndl% Lib "fwlib32.dll" (FlibHndl%)

' free library handle
Declare Function cnc_freelibhndl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' get custom macro type
Declare Function cnc_getmactype% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' set custom macro type
Declare Function cnc_setmactype% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' read CNC configuration information
Declare Function cnc_sysconfig% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSYSC)

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

' get maximum valid figures and number of decimal places
Declare Function cnc_getfigure% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, c%, d%)

' read the loop gain for each axis
Declare Function cnc_rdloopgain% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the actual current for each axis
Declare Function cnc_rdcurrent% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read the actual speed for each axis
Declare Function cnc_rdsrvspeed% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)


'------------------------------
' MAXIS: Axis Movement Control
'------------------------------

' cnc_opdi:signal operation command
Declare Function cnc_opdi% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' cnc_refpoint:reference point return
Declare Function cnc_refpoint% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb As ODBEXEC)

' cnc_abspoint:absolute movement
Declare Function cnc_abspoint% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d&, Odb1 As ODBPOS, Odb2 As ODBEXEC)

' cnc_incpoint:incremental movement
Declare Function cnc_incpoint% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, ByVal d&, Odb1 As ODBPOS, Odb2 As ODBEXEC)

' cnc_dwell:dwell
Declare Function cnc_dwell% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb1 As ODBPOS, Odb2 As ODBEXEC)

' cnc_coordre:coordinate establihment
Declare Function cnc_coordre% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c%, Odb1 As ODBPOS, Odb2 As ODBEXEC)

' cnc_exebufstat:reading of the executive buffer condition
Declare Function cnc_exebufstat% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBEXEC)

' cnc_finstate:Reading of the execution completion condition
Declare Function cnc_finstate% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBFIN)

' cnc_setfin:Release of the reading mode of the execution completion condition
Declare Function cnc_setfin% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBFIN)


'-----
' PMC
'-----

' read PMC data(area specified)
Declare Function pmc_rdpmcrng% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d&, ByVal e&, Odb As Any)

' write PMC data(area specified)
Declare Function pmc_wrpmcrng% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, Idb As Any)

' read informations of PMC data
Declare Function pmc_rdpmcinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As ODBPMCINF)


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
