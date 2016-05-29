#if !defined( WTL_LISTCTRL_H__)
#define WTL_LISTCTRL_H__

//#include "md5.h"
#include <string>
#include <algorithm>
#include <vector>
#include <hash_map>

#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include "resource.h"


// longueur de la clé
#define KEY_BUFF_SZ 11

// taken from Andre Bremer abpublic@hotmail.com
namespace std
{
template<> class hash_compare<string>
{
public:
enum
{
bucket_size = 4,
min_buckets = 8
};

// hash algorithm taken from SGI's STL
size_t operator()(const string& _Keyval) const
{
const char* __s=_Keyval.c_str();
unsigned long __h = 0;
for ( ; *__s; ++__s)
__h = 5*__h + *__s;
return size_t(__h);
}

bool operator()(const string& _Keyval1, const string& _Keyval2) const
{
return (_Keyval1 < _Keyval2);
}
};
}




class CProgressEntry {
public:
int m_Index;
int m_SubIndex;
HWND m_progHandle;

bool created;
CProgressEntry(){m_Prog=NULL; created=false;};
~CProgressEntry(){if (m_Prog) delete m_Prog ;};


CProgressEntry(int Index, int SubIndex)
{
m_Index=Index;
m_SubIndex=SubIndex ;
created=true;
//m_Prog=new CProgressBarCtrl();
m_Prog=new CMacProgressCtrl();
}

CProgressBarCtrl * m_Prog;
//CMacProgressCtrl * m_Prog;
};



class CTestListCtrl: public CWindowImpl< CTestListCtrl, CListViewCtrl>
//,public CCustomDraw<CTestListCtrl>
{
public:
DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

std::hash_map <std::string, CProgressEntry * > m_entries;
std::hash_map <std::string, CProgressEntry * > :: iterator pEntryIter;
std::pair <std::string, CProgressEntry *> entry_pair ;

CHeaderCtrl m_header;

BEGIN_MSG_MAP(CTestListCtrl)
MESSAGE_HANDLER(WM_CREATE,OnCreate)

MESSAGE_HANDLER(WM_PAINT, OnPaint)
MESSAGE_HANDLER(WM_SIZE, OnSize)
MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)

//CHAIN_MSG_MAP(CCustomDraw<CTestListCtrl>)
DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()

CTestListCtrl(void){ };
~CTestListCtrl(void){DeleteAllProgress();};

BOOL SubclassWindow(HWND hWnd)
{
BOOL bRet = CWindowImpl< CTestListCtrl,
CListViewCtrl>::SubclassWindow(hWnd);
return bRet;
}

LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
LRESULT lRes = DefWindowProc();
return lRes;
}


LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
ResizeAllProgress();
Invalidate();
bHandledúLSE;
return 0;
}




LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

// TODO: Add your control notification handler code here
//::MessageBox(NULL,"track",NULL,MB_OK);
ResizeAllProgress();
bHandledúLSE;
return 0;
}

LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
// CListCtrl::OnSize(nType, cx, cy);
// TODO: Add your message handler code here
ResizeAllProgress();
bHandledúLSE;
return 0;
}



// progress bar --

void AddItemProgress(int nItem, int nSubItem, LPCTSTR strItem)
{
// can only create progress for an existing item
if (nItem >= GetItemCount())
return;

// already created ?

// create key
char * k = new char[KEY_BUFF_SZ+1];
if (MakeKeyIndex(nItem,nSubItem,k))
{

CProgressEntry* ProgEntry ;
m_entries.insert(std::make_pair(k,new CProgressEntry(nItem, nSubItem)));
pEntryIter = m_entries.end();
pEntryIter--;
ProgEntry=(CProgressEntry*) pEntryIter->second;

CRect ItemRect;
GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, ItemRect);

int left = ItemRect.left;
int top = ItemRect.top;
int right = ItemRect.right;
int bottom = ItemRect.bottom;

// *********************
// ERROR : below is the error after calling the function more than 4
times
// ****************
HWND hnd=(ProgEntry->m_Prog)->Create(m_hWnd,CRect(left, top, right,
bottom),NULL,PBS_SMOOTH , 1);//PBS_SMOOTH | WS_CHILD
if (hnd)
{
SetItemText(nItem, nSubItem,strItem);
ProgEntry->m_progHandle=hnd;
(ProgEntry->m_Prog)->SetStep(1);
(ProgEntry->m_Prog)->SetRange(0, 100);
(ProgEntry->m_Prog)->SetPos(0);

}
}
delete k;

}



void SetItemProgress(int nItem,int nSubItem, int prog)
{
CProgressEntry * entry=NULL;
if (FindProgressEntryByIndex(nItem, nSubItem,&entry))
{
EnableProgress(entry);
(entry->m_Prog)->SetPos(prog);
SetItemText(nItem, nSubItem,NULL);
}

}

int GetProgressPos(int nItem,int nSubItem)
{
CProgressEntry * entry=NULL;
if (FindProgressEntryByIndex(nItem, nSubItem,&entry))
{
EnableProgress(entry);
return (entry->m_Prog)->GetPos();
}

return 0;
}

int SetProgressPos(int nItem,int nSubItem,int pos)
{
CProgressEntry * entry=NULL;
if (FindProgressEntryByIndex(nItem, nSubItem,&entry))
{
EnableProgress(entry);
SetItemText(nItem, nSubItem,NULL);
return (entry->m_Prog)->SetPos(pos);
}
return 0;
}

int StepProgress(int nItem,int nSubItem)
{
CProgressEntry * entry=NULL;
if (FindProgressEntryByIndex(nItem, nSubItem,&entry))
{
EnableProgress(entry);
SetItemText(nItem, nSubItem,NULL);
//(entry->m_Prog)->Invalidate();
return (entry->m_Prog)->StepIt();
}
return 0;
}

private:

bool MakeKeyIndex(int nItem,int nSubItem, char * k)
{
sprintf(k, "%d_%d", nItem,nSubItem);
return true;
}



void EnableProgress(CProgressEntry * entry)
{
if (entry==NULL)
return;
entry->created=false;
(entry->m_Prog)->ShowWindow(SW_SHOW);
}

bool FindProgressEntryByIndex(int nItem, int nSubItem,CProgressEntry
**entry)
{
bool ret=false;
char * k = new char[KEY_BUFF_SZ+1];
if (MakeKeyIndex(nItem,nSubItem,k))
{
pEntryIter = m_entries.find(k);
if ( pEntryIter != m_entries.end( ) )
{
*entry=(CProgressEntry*) pEntryIter->second;
ret=true;
}
}
delete k;
return ret;

}

void DeleteAllProgress()
{
CProgressEntry * ProgEntry=NULL;
for ( pEntryIter = m_entries.begin( ) ; pEntryIter != m_entries.end( ) ;
pEntryIter++ )
{
ProgEntry = (CProgressEntry*) pEntryIter->second;
::DestroyWindow(ProgEntry->m_progHandle);
delete ProgEntry;
}
m_entries.clear();

}

void ResizeOneProgress(int nItem,int nSubItem=0)
{
CProgressEntry * ProgEntry=NULL;

bool found=FindProgressEntryByIndex(nItem, nSubItem,&ProgEntry);
int n = GetTopIndex();
int nLast = n + GetCountPerPage();
if (nItem>=n && nItem<=nLast)
{
if (found)
ResizeOneProgressByEntry(ProgEntry,SW_SHOW);
}
else if (found)
ResizeOneProgressByEntry(ProgEntry,SW_HIDE);
}

void ResizeOneProgressByEntry(CProgressEntry * ProgEntry,DWORD
style=SW_SHOW)
{
if (ProgEntry)
{
CRect ItemRect;
int n = GetTopIndex();
int nLast = n + GetCountPerPage();
//if (ProgEntry->m_Index=n || ProgEntry->m_Index>nLast)
// style=SW_HIDE;

if (style==SW_SHOW)
{
GetSubItemRect(ProgEntry->m_Index, ProgEntry->m_SubIndex, LVIR_BOUNDS,
ItemRect);
int left = ItemRect.left;
int top = ItemRect.top;
int right = ItemRect.right;
int bottom = ItemRect.bottom;
if ((ProgEntry->m_Prog)->m_hWnd)
{
(ProgEntry->m_Prog)->MoveWindow(ItemRect,FALSE);

if (!ProgEntry->created)
{
(ProgEntry->m_Prog)->ShowWindow(style);
(ProgEntry->m_Prog)->Invalidate();
}
}
}
else
(ProgEntry->m_Prog)->ShowWindow(style);
}
}



void ResizeAllProgress()
{
CProgressEntry* ProgEntry=0;

for ( pEntryIter = m_entries.begin( ) ; pEntryIter != m_entries.end( ) ;
pEntryIter++ )
ResizeOneProgressByEntry(pEntryIter->second);

}

};


#endif