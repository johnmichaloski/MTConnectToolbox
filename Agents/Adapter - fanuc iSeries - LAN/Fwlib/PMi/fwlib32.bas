'------------------------------------------------------
'
' CNC/PMC Data Window Library for Power Mate i series
'
' Copyright (C) 1997 by FANUC LTD. All rights reserved.
'
'------------------------------------------------------


'----------------------
'  Axis Define
'----------------------
Global Const MAX_AXIS = 8		' Power Mate i-H TYPE
'Global Const MAX_AXIS = 2		' Power Mate i-D TYPE
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
  nData(0 To 1) As Integer      ' spindle data
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


'----------------------
' CNC: Program related
'----------------------

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

' cnc_rdctrldi:read the control input signal
Type ODBSPDI
  nSgnl1 As Byte
  nSgnl2 As Byte
  nSgnl3 As Byte
  nSgnl4 As Byte
End Type

' cnc_rdctrldo:read the control output signal
Type ODBSPDO
  nSgnl1 As Byte
  nSgnl2 As Byte
  nSgnl3 As Byte
  nSgnl4 As Byte
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


'----------------------------
' CNC : Servo tuning
'----------------------------
' cnc_sdsetchnl:Servo tuning (Channel data set)
Type IODBCHAN
  sChno As Byte
  sAxis As Byte
  lDatanum As Long
  nDatainf As Integer
  nDataadr As Integer
End Type

' cnc_sdsetchnl:Servo tuning (read Sampling data)
Type ODBSD
  nChadata As Integer
  lCount As Long
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

' set origin / preset relative axis position
Declare Function cnc_wrrelpos% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Idb As IDBWRR)

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

' read pitch error compensation data information
Declare Function cnc_rdpitchinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read custom macro variable information
Declare Function cnc_rdmacroinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBMVINF)

' read P code macro variable information
Declare Function cnc_rdpmacroinfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPMINF)


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

' read A/D conversion data
Declare Function cnc_adcnv% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As ODBAD)

' read operator's message
Declare Function cnc_rdopmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, Odb As Any)

' read operator's message3
Declare Function cnc_rdopmsg3% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As Any)

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

' read F-ROM information on CNC  */
Declare Function cnc_getfrominfo% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, Odb As ODBFINFORM)

' start of reading F-ROM data from CNC */
Declare Function cnc_fromgetstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$)

' read F-ROM data from CNC */
Declare Function cnc_fromget% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b As Byte, c&)

' end of reading F-ROM data from CNC */
Declare Function cnc_fromgetend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start of writing F-ROM data to CNC */
Declare Function cnc_fromputstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%)

' write F-ROM data to CNC */
Declare Function cnc_fromput% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b&)

' end of writing F-ROM data to CNC */
Declare Function cnc_fromputend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' delete F-ROM data on CNC */
Declare Function cnc_fromremove% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b$)

' read S-RAM information on CNC */
Declare Function cnc_getsraminfo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSINFO)

' start of reading S-RAM data from CNC */
Declare Function cnc_sramgetstart% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read S-RAM data from CNC */
Declare Function cnc_sramget% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b As Byte, c&)

' end of reading S-RAM data from CNC */
Declare Function cnc_sramgetend% Lib "fwlib32.dll" (ByVal FlibHndl%)

' start of reading S-RAM data from CNC (2) */
Declare Function cnc_sramgetstart2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)

' read S-RAM data from CNC (2) */
Declare Function cnc_sramget2% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, b As Byte, c&)

' end of reading S-RAM data from CNC (2) */
Declare Function cnc_sramgetend2% Lib "fwlib32.dll" (ByVal FlibHndl%)

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

' read the position deviation Z in the rigid tap mode
Declare Function cnc_rdposerrz% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the synchronous error in the rigid tap mode
Declare Function cnc_rdsynerrrg% Lib "fwlib32.dll" (ByVal FlibHndl%, a&)

' read the spindle alarm
'Declare Function cnc_rdspdlalm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)			'980713
Declare Function cnc_rdspdlalm% Lib "fwlib32.dll" (ByVal FlibHndl%, a as Byte)			'980713

' read the control input signal
Declare Function cnc_rdctrldi% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSPDI)

' read the control output signal
Declare Function cnc_rdctrldo% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSPDO)

' read the number of controled spindle
Declare Function cnc_rdnspdl% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)

' read data from FANUC BUS
Declare Function cnc_rdfbusmem% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d&, Odb As Any)

' write data to FANUC BUS
Declare Function cnc_wrfbusmem% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b%, ByVal c&, ByVal d&, Odb As Any)

' read program directory 2
#If ONO8D Then
Declare Function cnc_rdprogdir2% Lib "fwlib32.dll" Alias "cnc_rdprogdir2o8" (ByVal FlibHndl%, ByVal a%, b&, c%, Odb As PRGDIR2)
#Else
Declare Function cnc_rdprogdir2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, c%, Odb As PRGDIR2)
#End If

' read program directory 3
Declare Function cnc_rdprogdir3% Lib "fwlib32.dll" (ByVal FlibHndl, ByVal a%, b&, c%, Odb As Any)

' read CNC system soft series and version
Declare Function cnc_rdsyssoft% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSYSS)

' read CNC system soft series and version
Declare Function cnc_rdsyssoft2% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBSYSS2)

' read CNC module configuration information
Declare Function cnc_rdmdlconfig% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBMDLC)

' read CNC module configuration information 2
'Declare Function cnc_rdmdlconfig2% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a$)		'980713
Declare Function cnc_rdmdlconfig2% Lib "fwlib32.dll" (ByVal FlibHndl%, a as Byte)		'980713

' get date and time from cnc
Declare Function cnc_gettimer% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As Any)

' set date and time for cnc
Declare Function cnc_settimer% Lib "fwlib32.dll" (ByVal FlibHndl%, Idb As Any)

' read axis name CNC
Declare Function cnc_rdaxisname% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, Odb As ODBAXISNAME)

' read spindle name
Declare Function cnc_rdspdlname% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, Odb As ODBSPDLNAME)

' converts from FANUC code to Shift JIS code
Declare Function cnc_ftosjis% Lib "fwlib32.dll" (ByVal FlibHndl%, a As Byte, b As Byte)

' cycle start
Declare Function cnc_start% Lib "fwlib32.dll" (ByVal FlibHndl%)

' reset CNC
Declare Function cnc_reset% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Get CNC Model
Declare Function cnc_getcncmodel% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)


'----------------------------
' CNC : Servo tuning
'----------------------------
' Servo tuning (Channel data set)
Declare Function cnc_sdsetchnl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, Odb As IODBCHAN)

' Servo tuning (Channel data clear)
Declare Function cnc_sdclrchnl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo tuning (Sampling start)
Declare Function cnc_sdstartsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, c%)

' Servo tuning (Sampling cancel)
Declare Function cnc_sdcancelsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo tuning (read Sampling data)
Declare Function cnc_sdreadsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%, a%, ByVal b&, Odb As ODBSD)

' Servo tuning (Sampling end)
Declare Function cnc_sdendsmpl% Lib "fwlib32.dll" (ByVal FlibHndl%)

' Servo tuning (read 1 shot data)
Declare Function cnc_sdread1shot% Lib "fwlib32.dll" (ByVal FlibHndl%, a%)


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
'Declare Function pmc_rdkpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b$, ByVal c&)	'980713
Declare Function pmc_rdkpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b as Byte, ByVal c&)	'980713

' write data to extended backup memory
'Declare Function pmc_wrkpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, ByVal b$, ByVal c&)	'980713
Declare Function pmc_wrkpm% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a&, b as Byte, ByVal c&)	'980713

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

' read PMC alarm message */
Declare Function pmc_rdalmmsg% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, b%, c%, Odb As Any)

' get detail error for pmc
Declare Function pmc_getdtailerr% Lib "fwlib32.dll" (ByVal FlibHndl%, Odb As ODBPMCERR)

' read PMC memory data
Declare Function pmc_rdpmcmem% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, ByVal c&, Odb As Any)

' write PMC memory data
Declare Function pmc_wrpmcmem% Lib "fwlib32.dll" (ByVal FlibHndl%, ByVal a%, ByVal b&, ByVal c&, Idb As Any)


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
