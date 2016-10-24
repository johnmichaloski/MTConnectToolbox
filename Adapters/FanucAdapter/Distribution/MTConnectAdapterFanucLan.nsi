; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; installer properties
;XPStyle on
ShowInstDetails show

; The name of the installer
!define VERSION 1.4
Name "MTConnectAdapterFanucLan${VERSION}"
OutFile "MTConnectAdapterFanucLan${VERSION}.exe"

!include Sections.nsh
!include MUI2.nsh
!insertmacro MUI_LANGUAGE English
!include "logiclib.nsh"

!include "MTCAdapterQuery.nsdinc"

; The file to write

; The default installation directory
InstallDir $PROGRAMFILES\MTConnect\MTConnectAdapterFanucLan


Var install_button_text
Var welcome_title
Var welcome_text
var install_mode

;; CONFIGURATION variables
var ServiceName
var DeviceName
var AdapterPort
Var AdapterIP
Var AdapterDelay

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
;InstallDirRegKey HKLM "Software\NSIS_Example2" "Install_Dir"
;AddBrandingImage left 100
brandingtext "MTConnect"

; Request application privileges for Windows Vista
RequestExecutionLevel admin


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LicenseText "MTConnect Liscening Agreement" 
LicenseData "license.txt" 
 
;--------------------------------

; Pages
!define MUI_PAGE_CUSTOMFUNCTION_PRE WelcomePagePre
!define MUI_WELCOMEPAGE_TITLE '$welcome_title'
!define MUI_WELCOMEPAGE_TEXT '$welcome_text'
 !insertmacro MUI_PAGE_WELCOME

Page license 
;Page components
Page directory
;Page custom customPage "" ": custom page"
Page instfiles
Page custom fnc_MTCAdapterQuery_Show ConfigPageLeave


; Finish Page
;!define MUI_FINISHPAGE_RUN "$INSTDIR\FanucLanMTConnectAdapter${VERSION}.exe debug"
;!define MUI_FINISHPAGE_RUN_NOTCHECKED
;!insertmacro MUI_PAGE_FINISH

UninstPage uninstConfirm
UninstPage instfiles


;--------------------------------

; The stuff to install
Section "MTConnectAdapterFanucLan (required)"

  SectionIn RO ;Section Index - If you specify RO as a parameter, then the section will be Read-Only, meaning it will always be set to install. 

  ;SetBrandingImage  /RESIZETOFIT "mtconnect.bmp"
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
	; Put files there
	File "debug.txt"
	File "InstallAdapter.bat"
	File "KillAdapter.bat"
	File "MTCFanucAdapter.ini"
	File "MTCFanucAdapter.exe"
	File "RunAdapter.bat"
	File "SuperUser.bat"
 	File "UninstallService.vbs"
 	File "license.txt"  
 	File "MTCAdapterQuery.nsddef"
	File "MTCAdapterQuery.nsdinc"  
	File "vc2008redist_x86.exe"

  ; Write the installation path into the registry
  ;WriteRegStr HKLM SOFTWARE\NSIS_Example2 "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MTConnectAdapterFanucLan${VERSION}" "DisplayName" "MTConnectAdapterFanucLan${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MTConnectAdapterFanucLan${VERSION}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MTConnectAdapterFanucLan${VERSION}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MTConnectAdapterFanucLan${VERSION}" "NoRepair" 1

  ;ReadINIStr $0 $INSTDIR\winamp.ini winamp outname

	
  WriteUninstaller "uninstall.exe"

SectionEnd




;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MTConnectAdapterFanucLan${VERSION}"
  DeleteRegKey HKLM SOFTWARE\MTConnectAdapterFanucLan${VERSION}

  ; Remove files and uninstaller
  Delete $INSTDIR\uninstall.exe
 Delete "$INSTDIR\debug.txt"
  Delete "$INSTDIR\InstallAdapter.bat"
  Delete "$INSTDIR\KillAdapter.bat"
   Delete "$INSTDIR\MTCFanucAdapter.ini"
   Delete "$INSTDIR\MTCFanucAdapter.exe"
   Delete "$INSTDIR\RunAdapter.bat"
   Delete "$INSTDIR\SuperUser.bat"
    Delete "$INSTDIR\UninstallService.vbs"
   Delete "MTCAdapterQuery.nsddef"
   Delete "MTCAdapterQuery.nsdinc"  

  ; Remove shortcuts, if any
  ;Delete "$SMPROGRAMS\Example2\*.*"

  ; Remove directories used
  ;RMDir "$SMPROGRAMS\Example2"
  RMDir "$INSTDIR"

SectionEnd



Function ConfigPageLeave

;Read Textboxes values
${NSD_GetText} $hCtl_MTCAdapterQuery_TextBox1 $DeviceName  ; name
${NSD_GetText} $hCtl_MTCAdapterQuery_TextBox2  $AdapterPort ; port
${NSD_GetText} $hCtl_MTCAdapterQuery_TextBox3  $AdapterIP ; ip
${NSD_GetText} $hCtl_MTCAdapterQuery_TextBox4  $ServiceName ; service name
${NSD_GetText} $hCtl_MTCAdapterQuery_TextBox5  $AdapterDelay ; delay in seconds

;; Read Check box
${NSD_GetState} $hCtl_MTCAdapterQuery_CheckBox2 $5


;; Sample string concatenation
strcpy $9 "000"
strcpy $4 "$4$9"

;; Sample debugging message box
;MessageBox mb_ok $0
;MessageBox mb_ok $4

;; Write section parameter value to ini file
WriteIniStr '$INSTDIR\MTCFanucAdapter.ini' 'CONFIG' 'DeviceName'   $DeviceName
WriteIniStr '$INSTDIR\MTCFanucAdapter.ini' 'CONFIG' 'FanucPort'   $AdapterPort
WriteIniStr '$INSTDIR\MTCFanucAdapter.ini' 'CONFIG' 'FanucIpAddress'   $AdapterIP
WriteIniStr '$INSTDIR\MTCFanucAdapter.ini' 'CONFIG' 'ServiceName'   $ServiceName
WriteIniStr '$INSTDIR\MTCFanucAdapter.ini' 'CONFIG' 'FocasDelay'   $AdapterDelay

ExecWait '$INSTDIR\vc2008redist_x86.exe' $0
MessageBox mb_ok "Visaul 2008 C++ Install Returned $0"


 
  ; ExecWait '"sc.exe  create $ServiceName start= auto binpath= "$INSTDIR/MTCFanucAdapter.exe""'
 ; DetailPrint "sc.exe  start MTCFanucAdapter returned   $0"

${If} $5 <> 0
MessageBox mb_ok "sc.exe  start $ServiceName"
;ExecWait '"sc.exe  start $ServiceName"' $0 

${EndIf}

FunctionEnd

Function WelcomePagePre

strcpy $install_mode '0' ; test value

${if} $install_mode == '0'
strcpy $install_button_text "Actualize"
strcpy $welcome_title "Welcome title for actualization"
strcpy $welcome_text "Welcome text for actualization"
${else}
strcpy $install_button_text "Install"
strcpy $welcome_title "Welcome title for installation"
strcpy $welcome_text "Welcome text for installation"
${endif}

;MessageBox MB_OK "$welcome_title$\n$welcome_text"

FunctionEnd
 
