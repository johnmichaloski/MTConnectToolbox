//
// WIN32MessageMeanings.h
// 
#ifndef  _WIN32MessageMeanings_h
#define  _WIN32MessageMeanings_h

#include "stdafx.h"
#include <map>
#include "StringEx.h"
#include <Commctrl.h>
/* ADD THESE
#define WM_USER                         0x0400
#define OCM__BASE           (WM_USER+0x1c00)
#define OCM_COMMAND         (OCM__BASE + WM_COMMAND)

#ifdef _WIN32
#define OCM_CTLCOLORBTN     (OCM__BASE + WM_CTLCOLORBTN)
#define OCM_CTLCOLOREDIT    (OCM__BASE + WM_CTLCOLOREDIT)
#define OCM_CTLCOLORDLG     (OCM__BASE + WM_CTLCOLORDLG)
#define OCM_CTLCOLORLISTBOX (OCM__BASE + WM_CTLCOLORLISTBOX)
#define OCM_CTLCOLORMSGBOX  (OCM__BASE + WM_CTLCOLORMSGBOX)
#define OCM_CTLCOLORSCROLLBAR   (OCM__BASE + WM_CTLCOLORSCROLLBAR)
#define OCM_CTLCOLORSTATIC  (OCM__BASE + WM_CTLCOLORSTATIC)
#else
#define OCM_CTLCOLOR        (OCM__BASE + WM_CTLCOLOR)
#endif

#define OCM_DRAWITEM        (OCM__BASE + WM_DRAWITEM)
#define OCM_MEASUREITEM     (OCM__BASE + WM_MEASUREITEM)
#define OCM_DELETEITEM      (OCM__BASE + WM_DELETEITEM)
#define OCM_VKEYTOITEM      (OCM__BASE + WM_VKEYTOITEM)
#define OCM_CHARTOITEM      (OCM__BASE + WM_CHARTOITEM)
#define OCM_COMPAREITEM     (OCM__BASE + WM_COMPAREITEM)
#define OCM_HSCROLL         (OCM__BASE + WM_HSCROLL)
#define OCM_VSCROLL         (OCM__BASE + WM_VSCROLL)
#define OCM_PARENTNOTIFY    (OCM__BASE + WM_PARENTNOTIFY)
  */
// <winuser.h>
/*
typedef struct tagNMHDR { 
HWND hwndFrom; 
UINT idFrom; 
UINT code; 
} NMHDR; TVM_INSERTITEM 

*/
#define BEGIN_DEBUG_MSG_MAP(theClass) \
	CMessageMeanings meanings;\
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) \
	{ \
		BOOL bHandled = TRUE; \
		hWnd; \
		uMsg; \
		wParam; \
		lParam; \
		lResult; \
		bHandled; \
		AtlTrace("%s ProcessWindowMessage %S wParam = %x lParam = %x\n", #theClass, (BSTR) meanings.FindMeaning(uMsg), wParam, lParam );\
		if(uMsg==WM_NOTIFY)\
		{\
			NMHDR * pnmh = (LPNMHDR) lParam;\
			AtlTrace("%s WM_NOTIFY Hwnd From= %x ID From=%x Code =%S CodeValue=%x\n", #theClass, pnmh->hwndFrom,pnmh->idFrom, (BSTR) meanings.FindNotificationMeaning(pnmh->code), pnmh->code);\
		}\
		switch(dwMsgMapID) \
		{ \
		case 0:\



typedef std::map<UINT, StringEx> MessageMap;

class CMessageMeanings
{
public:
	MessageMap msgs;
	MessageMap notifications;
	CMessageMeanings()
	{  
		msgs[0x0001]=L"WM_CREATE";
		msgs[0x0002]=L"WM_DESTROY";
		msgs[0x0003]=L"WM_MOVE";
		msgs[0x0005]=L"WM_SIZE";
		msgs[0x0006]=L"WM_ACTIVATE";
		
		msgs[0x0007]=L"WM_SETFOCUS";
		msgs[0x0008]=L"WM_KILLFOCUS";
		msgs[0x000A]=L"WM_ENABLE";
		msgs[0x000B]=L"WM_SETREDRAW";
		msgs[0x000C]=L"WM_SETTEXT";
		msgs[0x000D]=L"WM_GETTEXT";
		msgs[0x000E]=L"WM_GETTEXTLENGTH";
		msgs[0x000F]=L"WM_PAINT";
		msgs[0x0010]=L"WM_CLOSE";
		msgs[0x0011]=L"WM_QUERYENDSESSION";
		msgs[0x0012]=L"WM_QUIT";
		msgs[0x0013]=L"WM_QUERYOPEN";
		msgs[0x0014]=L"WM_ERASEBKGND";
		msgs[0x0015]=L"WM_SYSCOLORCHANGE";
		msgs[0x0016]=L"WM_ENDSESSION";
		msgs[0x0018]=L"WM_SHOWWINDOW";
		msgs[0x001A]=L"WM_WININICHANGE";
		msgs[0x001B]=L"WM_DEVMODECHANGE";
		msgs[0x001C]=L"WM_ACTIVATEAPP";
		msgs[0x001D]=L"WM_FONTCHANGE";
		msgs[0x001E]=L"WM_TIMECHANGE";
		msgs[0x001F]=L"WM_CANCELMODE";
		msgs[0x0020]=L"WM_SETCURSOR";
		msgs[0x0021]=L"WM_MOUSEACTIVATE";
		msgs[0x0022]=L"WM_CHILDACTIVATE";
		msgs[0x0023]=L"WM_QUEUESYNC";
		msgs[0x0024]=L"WM_GETMINMAXINFO";
		
		msgs[0x0026]=L"WM_PAINTICON";                    
		msgs[0x0027]=L"WM_ICONERASEBKGND";               
		msgs[0x0028]=L"WM_NEXTDLGCTL";                   
		msgs[0x002A]=L"WM_SPOOLERSTATUS";                
		msgs[0x002B]=L"WM_DRAWITEM";                     
		msgs[0x002C]=L"WM_MEASUREITEM";                  
		msgs[0x002D]=L"WM_DELETEITEM";                   
		msgs[0x002E]=L"WM_VKEYTOITEM";                   
		msgs[0x002F]=L"WM_CHARTOITEM";                   
		msgs[0x0030]=L"WM_SETFONT";                      
		msgs[0x0031]=L"WM_GETFONT";                      
		msgs[0x0032]=L"WM_SETHOTKEY";                    
		msgs[0x0033]=L"WM_GETHOTKEY";                    
		msgs[0x0037]=L"WM_QUERYDRAGICON";                
		msgs[0x0039]=L"WM_COMPAREITEM";                  
		msgs[0x003D]=L"WM_GETOBJECT";                    
		
		msgs[0x0041]=L"WM_COMPACTING";                   
		msgs[0x0044]=L"WM_COMMNOTIFY";
		msgs[0x0046]=L"WM_WINDOWPOSCHANGING";            
		msgs[0x0047]=L"WM_WINDOWPOSCHANGED";             
		
		msgs[0x0048]=L"WM_POWER";                        		
		msgs[0x004A]=L"WM_COPYDATA";                     
		msgs[0x004B]=L"WM_CANCELJOURNAL";                
		msgs[0x004E]=L"WM_NOTIFY";                       
		msgs[0x0050]=L"WM_INPUTLANGCHANGEREQUEST";       
		msgs[0x0051]=L"WM_INPUTLANGCHANGE";              
		msgs[0x0052]=L"WM_TCARD";                        
		msgs[0x0053]=L"WM_HELP";                         
		msgs[0x0054]=L"WM_USERCHANGED";                  
		msgs[0x0055]=L"WM_NOTIFYFORMAT";                         
		msgs[0x007B]=L"WM_CONTEXTMENU";                  
		msgs[0x007C]=L"WM_STYLECHANGING";                
		msgs[0x007D]=L"WM_STYLECHANGED";                 
		msgs[0x007E]=L"WM_DISPLAYCHANGE";                
		msgs[0x007F]=L"WM_GETICON";                      
		msgs[0x0080]=L"WM_SETICON";                      
		msgs[0x0081]=L"WM_NCCREATE";                     
		msgs[0x0082]=L"WM_NCDESTROY";                    
		msgs[0x0083]=L"WM_NCCALCSIZE";                   
		msgs[0x0084]=L"WM_NCHITTEST";                    
		msgs[0x0085]=L"WM_NCPAINT";                     
		msgs[0x0086]=L"WM_NCACTIVATE";                  
		msgs[0x0087]=L"WM_GETDLGCODE";                   
		msgs[0x0088]=L"WM_SYNCPAINT";                    
		msgs[0x00A0]=L"WM_NCMOUSEMOVE";                  
		msgs[0x00A1]=L"WM_NCLBUTTONDOWN";                
		msgs[0x00A2]=L"WM_NCLBUTTONUP";                  
		msgs[0x00A3]=L"WM_NCLBUTTONDBLCLK";              
		msgs[0x00A4]=L"WM_NCRBUTTONDOWN";                
		msgs[0x00A5]=L"WM_NCRBUTTONUP";                  
		msgs[0x00A6]=L"WM_NCRBUTTONDBLCLK";             
		msgs[0x00A7]=L"WM_NCMBUTTONDOWN";                
		msgs[0x00A8]=L"WM_NCMBUTTONUP";                  
		msgs[0x00A9]=L"WM_NCMBUTTONDBLCLK";              
		
		msgs[0x0100]=L"WM_KEYDOWN";                      
		msgs[0x0101]=L"WM_KEYUP";                        
		msgs[0x0102]=L"WM_CHAR";                         
		msgs[0x0103]=L"WM_DEADCHAR";                     
		msgs[0x0104]=L"WM_SYSKEYDOWN";                   
		msgs[0x0105]=L"WM_SYSKEYUP";                     
		msgs[0x0106]=L"WM_SYSCHAR";                      
		msgs[0x0107]=L"WM_SYSDEADCHAR";                  
		msgs[0x0108]=L"WM_KEYLAST";                      
		
		msgs[0x010D]=L"WM_IME_STARTCOMPOSITION";         
		msgs[0x010E]=L"WM_IME_ENDCOMPOSITION";           
		msgs[0x010F]=L"WM_IME_COMPOSITION";              
		msgs[0x010F]=L"WM_IME_KEYLAST";                  
		
		msgs[0x0110]=L"WM_INITDIALOG";                   
		msgs[0x0111]=L"WM_COMMAND";                      
		msgs[0x0112]=L"WM_SYSCOMMAND";                   
		msgs[0x0113]=L"WM_TIMER";                        
		msgs[0x0114]=L"WM_HSCROLL";                      
		msgs[0x0115]=L"WM_VSCROLL";                      
		msgs[0x0116]=L"WM_INITMENU";                     
		msgs[0x0117]=L"WM_INITMENUPOPUP";                
		msgs[0x011F]=L"WM_MENUSELECT";                   
		msgs[0x0120]=L"WM_MENUCHAR";                     
		msgs[0x0121]=L"WM_ENTERIDLE";                    
		
		msgs[0x0122]=L"WM_MENURBUTTONUP";                
		msgs[0x0123]=L"WM_MENUDRAG";                    
		msgs[0x0124]=L"WM_MENUGETOBJECT";                
		msgs[0x0125]=L"WM_UNINITMENUPOPUP";              
		msgs[0x0126]=L"WM_MENUCOMMAND";                  
		
		msgs[0x0132]=L"WM_CTLCOLORMSGBOX";               
		msgs[0x0133]=L"WM_CTLCOLOREDIT";                 
		msgs[0x0134]=L"WM_CTLCOLORLISTBOX";              
		msgs[0x0135]=L"WM_CTLCOLORBTN";                  
		msgs[0x0136]=L"WM_CTLCOLORDLG";                  
		msgs[0x0137]=L"WM_CTLCOLORSCROLLBAR";            
		msgs[0x0138]=L"WM_CTLCOLORSTATIC";               
		
		
		msgs[0x0200]=L"WM_MOUSEMOVE";                    
		msgs[0x0201]=L"WM_LBUTTONDOWN";                  
		msgs[0x0202]=L"WM_LBUTTONUP";                    
		msgs[0x0203]=L"WM_LBUTTONDBLCLK";                
		msgs[0x0204]=L"WM_RBUTTONDOWN";                  
		msgs[0x0205]=L"WM_RBUTTONUP";                    
		msgs[0x0206]=L"WM_RBUTTONDBLCLK";                
		msgs[0x0207]=L"WM_MBUTTONDOWN";                  
		msgs[0x0208]=L"WM_MBUTTONUP";                    
		msgs[0x0209]=L"WM_MBUTTONDBLCLK";                
		msgs[0x020A]=L"WM_MOUSELAST";      
		msgs[0x020B]=L"WM_MOUSEWHEEL";                    
		
		msgs[0x0210]=L"WM_PARENTNOTIFY";                  
		msgs[0x0211]=L"WM_ENTERMENULOOP";                 
		msgs[0x0212]=L"WM_EXITMENULOOP";                  
		msgs[0x0213]=L"WM_NEXTMENU";                      
		msgs[0x0214]=L"WM_SIZING";                        
		msgs[0x0215]=L"WM_CAPTURECHANGED";                
		msgs[0x0216]=L"WM_MOVING";  
		
		msgs[0x0218]=L"WM_POWERBROADCAST";                  	
		msgs[0x0220]=L"WM_MDICREATE";                     
		msgs[0x0221]=L"WM_MDIDESTROY";                    
		msgs[0x0222]=L"WM_MDIACTIVATE";                   
		msgs[0x0223]=L"WM_MDIRESTORE";                    
		msgs[0x0224]=L"WM_MDINEXT";                       
		msgs[0x0225]=L"WM_MDIMAXIMIZE";                   
		msgs[0x0226]=L"WM_MDITILE";                       
		msgs[0x0227]=L"WM_MDICASCADE";                    
		msgs[0x0228]=L"WM_MDIICONARRANGE";                
		msgs[0x0229]=L"WM_MDIGETACTIVE";                 
		
		
		msgs[0x0230]=L"WM_MDISETMENU";                    
		msgs[0x0231]=L"WM_ENTERSIZEMOVE";                 
		msgs[0x0232]=L"WM_EXITSIZEMOVE";                  
		msgs[0x0233]=L"WM_DROPFILES";                     
		msgs[0x0234]=L"WM_MDIREFRESHMENU";                	
		msgs[0x02A1]=L"WM_MOUSEHOVER";                     
		msgs[0x02A3]=L"WM_MOUSELEAVE";                     
		msgs[0x0300]=L"WM_CUT";                            
		msgs[0x0301]=L"WM_COPY";                           
		msgs[0x0302]=L"WM_PASTE";                          
		msgs[0x0303]=L"WM_CLEAR";                          
		msgs[0x0304]=L"WM_UNDO";                           
		msgs[0x0305]=L"WM_RENDERFORMAT";                   
		msgs[0x0306]=L"WM_RENDERALLFORMATS";               
		msgs[0x0307]=L"WM_DESTROYCLIPBOARD";               
		msgs[0x0308]=L"WM_DRAWCLIPBOARD";                  
		msgs[0x0309]=L"WM_PAINTCLIPBOARD";                 
		msgs[0x030A]=L"WM_VSCROLLCLIPBOARD";               
		msgs[0x030B]=L"WM_SIZECLIPBOARD";                  
		msgs[0x030C]=L"WM_ASKCBFORMATNAME";                
		msgs[0x030D]=L"WM_CHANGECBCHAIN";                  
		msgs[0x030E]=L"WM_HSCROLLCLIPBOARD";               
		msgs[0x030F]=L"WM_QUERYNEWPALETTE";                
		msgs[0x0310]=L"WM_PALETTEISCHANGING";              
		msgs[0x0311]=L"WM_PALETTECHANGED";                
		msgs[0x0312]=L"WM_HOTKEY";                         
		
		msgs[0x0317]=L"WM_PRINT";                          
		msgs[0x0318]=L"WM_PRINTCLIENT";                    
		
		msgs[0x0358]=L"WM_HANDHELDFIRST";                  
		msgs[0x035F]=L"WM_HANDHELDLAST";                   

		msgs[0x0360]=L"WM_AFXFIRST";                       
		msgs[0x037F]=L"WM_AFXLAST";                        
		msgs[0x0380]=L"WM_PENWINFIRST";                    
		msgs[0x038F]=L"WM_PENWINLAST";                     
		msgs[0x00B0]=L"EM_GETSEL";                
		msgs[0x00B1]=L"EM_SETSEL";                
		msgs[0x00B2]=L"EM_GETRECT";               
		msgs[0x00B3]=L"EM_SETRECT";               
		msgs[0x00B4]=L"EM_SETRECTNP";             
		msgs[0x00B5]=L"EM_SCROLL";                
		msgs[0x00B6]=L"EM_LINESCROLL";            
		msgs[0x00B7]=L"EM_SCROLLCARET";           
		msgs[0x00B8]=L"EM_GETMODIFY";             
		msgs[0x00B9]=L"EM_SETMODIFY";             
		msgs[0x00BA]=L"EM_GETLINECOUNT";          
		msgs[0x00BB]=L"EM_LINEINDEX";             
		msgs[0x00BC]=L"EM_SETHANDLE";             
		msgs[0x00BD]=L"EM_GETHANDLE";             
		msgs[0x00BE]=L"EM_GETTHUMB";              
		msgs[0x00C1]=L"EM_LINELENGTH";            
		msgs[0x00C2]=L"EM_REPLACESEL";            
		msgs[0x00C4]=L"EM_GETLINE";               
		msgs[0x00C5]=L"EM_LIMITTEXT";             
		msgs[0x00C6]=L"EM_CANUNDO";               
		msgs[0x00C7]=L"EM_UNDO";                  
		msgs[0x00C8]=L"EM_FMTLINES";              
		msgs[0x00C9]=L"EM_LINEFROMCHAR";          
		msgs[0x00CB]=L"EM_SETTABSTOPS";           
		msgs[0x00CC]=L"EM_SETPASSWORDCHAR";       
		msgs[0x00CD]=L"EM_EMPTYUNDOBUFFER";       
		msgs[0x00CE]=L"EM_GETFIRSTVISIBLELINE";   
		msgs[0x00CF]=L"EM_SETREADONLY";           
		msgs[0x00D0]=L"EM_SETWORDBREAKPROC";      
		msgs[0x00D1]=L"EM_GETWORDBREAKPROC";      
		msgs[0x00D2]=L"EM_GETPASSWORDCHAR";       
		
		msgs[0x00D3]=L"EM_SETMARGINS";            
		msgs[0x00D4]=L"EM_GETMARGINS";            
		msgs[0x00C5]=L"EM_SETLIMITTEXT";  
		msgs[0x00D5]=L"EM_GETLIMITTEXT";          
		msgs[0x00D6]=L"EM_POSFROMCHAR";           
		msgs[0x00D7]=L"EM_CHARFROMPOS";           
		
		msgs[0x00D8]=L"EM_SETIMESTATUS";          
		msgs[0x00D9]=L"EM_GETIMESTATUS";   
		
		/*
		* Listbox messages
		*/
		msgs[0x0180]=L"LB_ADDSTRING";             
		msgs[0x0181]=L"LB_INSERTSTRING";          
		msgs[0x0182]=L"LB_DELETESTRING";          
		msgs[0x0183]=L"LB_SELITEMRANGEEX";        
		msgs[0x0184]=L"LB_RESETCONTENT";          
		msgs[0x0185]=L"LB_SETSEL";                
		msgs[0x0186]=L"LB_SETCURSEL";             
		msgs[0x0187]=L"LB_GETSEL";                
		msgs[0x0188]=L"LB_GETCURSEL";            
		msgs[0x0189]=L"LB_GETTEXT";               
		msgs[0x018A]=L"LB_GETTEXTLEN";            
		msgs[0x018B]=L"LB_GETCOUNT";              
		msgs[0x018C]=L"LB_SELECTSTRING";          
		msgs[0x018D]=L"LB_DIR";                   
		msgs[0x018E]=L"LB_GETTOPINDEX";           
		msgs[0x018F]=L"LB_FINDSTRING";            
		msgs[0x0190]=L"LB_GETSELCOUNT";           
		msgs[0x0191]=L"LB_GETSELITEMS";           
		msgs[0x0192]=L"LB_SETTABSTOPS";           
		msgs[0x0193]=L"LB_GETHORIZONTALEXTENT";   
		msgs[0x0194]=L"LB_SETHORIZONTALEXTENT";   
		msgs[0x0195]=L"LB_SETCOLUMNWIDTH";        
		msgs[0x0196]=L"LB_ADDFILE";               
		msgs[0x0197]=L"LB_SETTOPINDEX";           
		msgs[0x0198]=L"LB_GETITEMRECT";           
		msgs[0x0199]=L"LB_GETITEMDATA";           
		msgs[0x019A]=L"LB_SETITEMDATA";           
		msgs[0x019B]=L"LB_SELITEMRANGE";          
		msgs[0x019C]=L"LB_SETANCHORINDEX";        
		msgs[0x019D]=L"LB_GETANCHORINDEX";        
		msgs[0x019E]=L"LB_SETCARETINDEX";         
		msgs[0x019F]=L"LB_GETCARETINDEX";         
		msgs[0x01A0]=L"LB_SETITEMHEIGHT";         
		msgs[0x01A1]=L"LB_GETITEMHEIGHT";         
		msgs[0x01A2]=L"LB_FINDSTRINGEXACT";       
		msgs[0x01A5]=L"LB_SETLOCALE";            
		msgs[0x01A6]=L"LB_GETLOCALE";             
		msgs[0x01A7]=L"LB_SETCOUNT";              
		msgs[0x01A8]=L"LB_INITSTORAGE";           
		msgs[0x01A9]=L"LB_ITEMFROMPOINT";         
		msgs[0x01B0]=L"LB_MSGMAX";                
		
		/*
		* Combo Box messages
		*/
		msgs[0x0140]=L"CB_GETEDITSEL";                
		msgs[0x0141]=L"CB_LIMITTEXT";                 
		msgs[0x0142]=L"CB_SETEDITSEL";                
		msgs[0x0143]=L"CB_ADDSTRING";                 
		msgs[0x0144]=L"CB_DELETESTRING";              
		msgs[0x0145]=L"CB_DIR";                       
		msgs[0x0146]=L"CB_GETCOUNT";                  
		msgs[0x0147]=L"CB_GETCURSEL";                 
		msgs[0x0148]=L"CB_GETLBTEXT";                
		msgs[0x0149]=L"CB_GETLBTEXTLEN";              
		msgs[0x014A]=L"CB_INSERTSTRING";              
		msgs[0x014B]=L"CB_RESETCONTENT";              
		msgs[0x014C]=L"CB_FINDSTRING";                
		msgs[0x014D]=L"CB_SELECTSTRING";              
		msgs[0x014E]=L"CB_SETCURSEL";                 
		msgs[0x014F]=L"CB_SHOWDROPDOWN";              
		msgs[0x0150]=L"CB_GETITEMDATA";               
		msgs[0x0151]=L"CB_SETITEMDATA";               
		msgs[0x0152]=L"CB_GETDROPPEDCONTROLRECT";     
		msgs[0x0153]=L"CB_SETITEMHEIGHT";             
		msgs[0x0154]=L"CB_GETITEMHEIGHT";             
		msgs[0x0155]=L"CB_SETEXTENDEDUI";             
		msgs[0x0156]=L"CB_GETEXTENDEDUI";             
		msgs[0x0157]=L"CB_GETDROPPEDSTATE";           
		msgs[0x0158]=L"CB_FINDSTRINGEXACT";           
		msgs[0x0159]=L"CB_SETLOCALE";                 
		msgs[0x015A]=L"CB_GETLOCALE";                 
		msgs[0x015b]=L"CB_GETTOPINDEX";               
		msgs[0x015c]=L"CB_SETTOPINDEX";               
		msgs[0x015d]=L"CB_GETHORIZONTALEXTENT";       
		msgs[0x015e]=L"CB_SETHORIZONTALEXTENT";       
		msgs[0x015f]=L"CB_GETDROPPEDWIDTH";           
		msgs[0x0160]=L"CB_SETDROPPEDWIDTH";           
		msgs[0x0161]=L"CB_INITSTORAGE";               
		msgs[0x0162]=L"CB_MSGMAX";                    
		
		/*
		* Scroll bar messages IDC_ARROW SetClassLongA IDC_ARROW GetModuleHandle
		*/
		msgs[0x00E0]=L"SBM_SETPOS";              
		msgs[0x00E1]=L"SBM_GETPOS";               
		msgs[0x00E2]=L"SBM_SETRANGE";            
		msgs[0x00E6]=L"SBM_SETRANGEREDRAW";      
		msgs[0x00E3]=L"SBM_GETRANGE";           
		msgs[0x00E4]=L"SBM_ENABLE_ARROWS";       
		msgs[0x00E9]=L"SBM_SETSCROLLINFO";            
		msgs[0x00EA]=L"SBM_GETSCROLLINFO";  
		notifications[NM_FIRST-1]=L"NM_OUTOFMEMORY";        
		notifications[NM_FIRST-2]=L"NM_CLICK";               
		notifications[NM_FIRST-3]=L"NM_DBLCLK";              
		notifications[NM_FIRST-4]=L"NM_RETURN";              
		notifications[NM_FIRST-5]=L"NM_RCLICK";              
		notifications[NM_FIRST-6]=L"NM_RDBLCLK";             
		notifications[NM_FIRST-7]=L"NM_SETFOCUS";             
		notifications[NM_FIRST-8]=L"NM_KILLFOCUS";           
		
		notifications[NM_FIRST-12]=L"NM_CUSTOMDRAW";         
		notifications[NM_FIRST-13]=L"NM_HOVER";              
		
		notifications[NM_FIRST-14]=L"NM_NCHITTEST";          
		notifications[NM_FIRST-15]=L"NM_KEYDOWN";            
		notifications[NM_FIRST-16]=L"NM_RELEASEDCAPTURE";    
		notifications[NM_FIRST-17]=L"NM_SETCURSOR";  
		notifications[NM_FIRST-1]=L"NM_CHAR";  
		
		notifications[LVN_FIRST-0]=L"LVN_ITEMCHANGING";        
		notifications[LVN_FIRST-1]=L"LVN_ITEMCHANGED";         
		notifications[LVN_FIRST-2]=L"LVN_INSERTITEM";          
		notifications[LVN_FIRST-3]=L"LVN_DELETEITEM";          
		notifications[LVN_FIRST-4]=L"LVN_DELETEALLITEMS";      
		notifications[LVN_FIRST-5]=L"LVN_BEGINLABELEDITA";     
		notifications[LVN_FIRST-75]=L"LVN_BEGINLABELEDITW";    
		notifications[LVN_FIRST-6]=L"LVN_ENDLABELEDITA";    
		notifications[LVN_FIRST-76]=L"LVN_ENDLABELEDITW";   
		notifications[LVN_FIRST-8]=L"LVN_COLUMNCLICK";      
		notifications[LVN_FIRST-9]=L"LVN_BEGINDRAG";        
		notifications[LVN_FIRST-11]=L"LVN_BEGINRDRAG";      
		notifications[LVN_FIRST-13]=L"LVN_ODCACHEHINT";     
		notifications[LVN_FIRST-52]=L"LVN_ODFINDITEMA";     
		notifications[LVN_FIRST-79]=L"LVN_ODFINDITEMW";     
		
		notifications[LVN_FIRST-14]=L"LVN_ITEMACTIVATE";    
		notifications[LVN_FIRST-1]=L"LVN_ODSTATECHANGED";   
		
		
		notifications[LVN_FIRST-55]=L"LVN_KEYDOWN";         
		
		
		notifications[LVN_FIRST-21]=L"LVN_HOTTRACK";        
		
		notifications[LVN_FIRST-50]=L"LVN_GETDISPINFOA";    
		notifications[LVN_FIRST-77]=L"LVN_GETDISPINFOW";    
		notifications[LVN_FIRST-51]=L"LVN_SETDISPINFOA";    
		notifications[LVN_FIRST-78]=L"LVN_SETDISPINFOW";    
		
		notifications[TV_FIRST + 0]=L"TVM_INSERTITEMA";
		notifications[TV_FIRST + 50]=L"TVM_INSERTITEMW";         
		
		notifications[TV_FIRST + 1]=L"TVM_DELETEITEM";          
		notifications[TV_FIRST + 2]=L"TVM_EXPAND";              
		notifications[TV_FIRST+4]=L"TVM_GETITEMRECT";        
		notifications[TV_FIRST +5]=L"TVM_GETCOUNT";            
		notifications[TV_FIRST +6]=L"TVM_GETINDENT";           
		notifications[TV_FIRST +7]=L"TVM_SETINDENT";           
		notifications[TV_FIRST +8]=L"TVM_GETIMAGELIST";        
		notifications[TV_FIRST +9]=L"TVM_SETIMAGELIST";        
		notifications[TV_FIRST +10]=L"TVM_GETNEXTITEM";         
		notifications[TV_FIRST +11]=L"TVM_SELECTITEM";          
		notifications[TV_FIRST +12]=L"TVM_GETITEMA";            
		notifications[TV_FIRST +62]=L"TVM_GETITEMW";            
		notifications[TV_FIRST +13]=L"TVM_SETITEMA";            
		notifications[TV_FIRST +63]=L"TVM_SETITEMW";            
		notifications[TV_FIRST +14]=L"TVM_EDITLABELA";          
		notifications[TV_FIRST +65]=L"TVM_EDITLABELW";          
		notifications[TV_FIRST +15]=L"TVM_GETEDITCONTROL";      
		notifications[TV_FIRST +16]=L"TVM_GETVISIBLECOUNT";     
		notifications[TV_FIRST +17]=L"TVM_HITTEST";             
		notifications[TV_FIRST +18]=L"TVM_CREATEDRAGIMAGE";     
		notifications[TV_FIRST +19]=L"TVM_SORTCHILDREN";        
		notifications[TV_FIRST +20]=L"TVM_ENSUREVISIBLE";       
		notifications[TV_FIRST +21]=L"TVM_SORTCHILDRENCB";      
		notifications[TV_FIRST +22]=L"TVM_ENDEDITLABELNOW";     
		notifications[TV_FIRST +23]=L"TVM_GETISEARCHSTRINGA";   
		notifications[TV_FIRST +64]=L"TVM_GETISEARCHSTRINGW";   
		notifications[TV_FIRST +24]=L"TVM_SETTOOLTIPS";         
		notifications[TV_FIRST +25]=L"TVM_GETTOOLTIPS";         
		notifications[TV_FIRST +27]=L"TVM_SETITEMHEIGHT";       
		notifications[TV_FIRST +28]=L"TVM_GETITEMHEIGHT";       
		notifications[TV_FIRST +29]=L"TVM_SETBKCOLOR";          
		notifications[TV_FIRST +30]=L"TVM_SETTEXTCOLOR";        
		notifications[TV_FIRST +31]=L"TVM_GETBKCOLOR";          
		notifications[TV_FIRST +32]=L"TVM_GETTEXTCOLOR";        
		notifications[TV_FIRST +33]=L"TVM_SETSCROLLTIME";       
		notifications[TV_FIRST +33]=L"TVM_GETSCROLLTIME";       
		notifications[TV_FIRST +37]=L"TVM_SETINSERTMARKCOLOR";  
		notifications[TV_FIRST +38]=L"TVM_GETINSERTMARKCOLOR"; 
		
		
		notifications[HDN_FIRST - 0]=L"HDN_ITEMCHANGINGA";  
		notifications[HDN_FIRST - 20]=L"HDN_ITEMCHANGINGW";    
		notifications[HDN_FIRST - 1]=L"HDN_ITEMCHANGEDA";     
		notifications[HDN_FIRST - 21]=L"HDN_ITEMCHANGEDW";     
		notifications[HDN_FIRST - 2]=L"HDN_ITEMCLICKA";        
		notifications[HDN_FIRST - 22]=L"HDN_ITEMCLICKW";       
		notifications[HDN_FIRST - 3]=L"HDN_ITEMDBLCLICKA";     
		notifications[HDN_FIRST - 23]=L"HDN_ITEMDBLCLICKW";    
		notifications[HDN_FIRST - 5]=L"HDN_DIVIDERDBLCLICKA";  
		notifications[HDN_FIRST - 25]=L"HDN_DIVIDERDBLCLICKW"; 
		notifications[HDN_FIRST - 6]=L"HDN_BEGINTRACKA";       
		notifications[HDN_FIRST - 26]=L"HDN_BEGINTRACKW";      
		notifications[HDN_FIRST - 7]=L"HDN_ENDTRACKA";         
		notifications[HDN_FIRST - 27]=L"HDN_ENDTRACKW";        
		notifications[HDN_FIRST - 8]=L"HDN_TRACKA";            
		notifications[HDN_FIRST - 28]=L"HDN_TRACKW";           
		notifications[HDN_FIRST - 9]=L"HDN_GETDISPINFOA";      
		notifications[HDN_FIRST - 29]=L"HDN_GETDISPINFOW";     
		notifications[HDN_FIRST - 10]=L"HDN_BEGINDRAG";        
		notifications[HDN_FIRST - 11]=L"HDN_ENDDRAG";          
		
	}
	StringEx FindMeaning(UINT i)
	{
		MessageMap::iterator it2 = msgs.find(i);
		if(it2==msgs.end()) 
		{
			return StringEx().Append("0x%x", i);
		}
		return (*it2).second;
	}
	StringEx FindNotificationMeaning(UINT i)
	{
		MessageMap::iterator it2 = notifications.find(i);
		if(it2==notifications.end()) 
		{
			// listview
			if((i< LVN_FIRST) && (i> LVN_LAST)) return L"ListView Notification";
			// header
			if((i< HDN_FIRST) && (i> HDN_LAST)) return L"header  Notification";
			// treeview
			if((i< TVN_FIRST) && (i> TVN_LAST)) return L"treeview Notification";
			// tooltips
			if((i< TTN_FIRST) && (i> TTN_LAST)) return L"tooltips Notification";
			// tab control
			if((i< TCN_FIRST) && (i> TCN_LAST)) return L"tab control Notification";
			return StringEx().Append("0x%x", i);
		}
		return (*it2).second;
	}
};

#endif