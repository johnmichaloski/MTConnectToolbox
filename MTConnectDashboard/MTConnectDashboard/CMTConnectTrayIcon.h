// CMTConnectTrayIcon.h : Declaration of the CCMTConnectTrayIcon

#pragma once
#include "resource.h"       // main symbols

#include "MtConnectDashboard.h"
#include "_IDboardNotificationEvents_CP.h"

#include "MainFrm.h"
#include "WtlHtmlView.h"
#include "StdStringFcn.h"
#include <boost/timer.hpp>
#include "DataDictionary.h"
#include "ElTime.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

extern CMainFrame wndMain;

// CCMTConnectTrayIcon

class CCMTConnectTrayIcon 
{
public:
	CCMTConnectTrayIcon()
	{
		wpx = 300.0;
		dTraceWidth=600.0;
		nSampleTimeSecs = 30;
		_numberfields="Srpm,Feed,Spindle,OEE,RpmTotal"; 
		_formats["Srpm"]="%6.0f";
		_formats["Feed"]="%6.2f";
		_formats["SpindleLoad"]="3.0f%%";
		_formats["OEE"]="%5.2f%%";
		_formats["RpmTotal"]="%6.0f";
		 lastlogtime=COleDateTime::GetCurrentTime();
		 _elapsedSeconds=0;
	}
	static std::string CreateHtmlDocument();
	static std::string HtmlStyle();
	static std::string GetFaultSummaryHtml() ;

	void			AddRow(std::vector<std::string> fields, DataDictionary data, COleDateTime loggingtime = COleDateTime::GetCurrentTime());
	std::string		CreateHistory() ;
	std::string		CreateHtmlRow(std::vector<std::string> fields,DataDictionary data);
	std::string		CreateHtmlTable(std::vector<std::string> fields);
	std::string		GetCurrentValue(std::string field);
	std::string		GetHeaderRow(std::vector<std::string> fields);
	std::string		GetHtmlStatus(std::vector<std::string> fields);
	int				HistoryUpdate(COleDateTime logtime, int seconds);
	void			ReplaceLastRowValues(std::vector<std::string> fields, DataDictionary data);
	void			ReplaceLastRowValues(DataDictionary data);
	void			SetHtmlView(CWtlHtmlView  *pView) { _htmlpage=pView; }
	void			SetTimestamp(COleDateTime logtime);
	void			UpdateHtml(std::vector<std::string> fields);

	// GLobal information
	STDMETHOD(Formats)(BSTR csvFields, BSTR csvFormats);
	STDMETHOD(Types)(BSTR csvFields, BSTR csvTypes);

	// Machine summary tab
	STDMETHOD(CreateTabWindow)(BSTR name);
	//STDMETHOD(SetHeaderColumns)(BSTR csv); // adds history to end of tab
	STDMETHOD(AddRow)(BSTR csvFields, BSTR values);
	STDMETHOD(SetIpAddr)(BSTR ipaddr);

	STDMETHOD(SetTrayIconColor)(BSTR traycolor,VARIANT_BOOL flashing);
	STDMETHOD(SetTooltip)(BSTR bstr);

	// Alarm tab
	STDMETHOD(CreateAlarmTab)(BSTR name);
	STDMETHOD(Clear)(void);
	STDMETHOD(AddFaultRow)(BSTR csvFields, BSTR csvValues);
	STDMETHOD(SetFaultColumns)(BSTR csv); // csv representation of fault column fields

	////////////////////////////////////////////////////////////////////////////////////////////
	CWtlHtmlView * _htmlpage;
	int _elapsedSeconds;
	COleDateTime lastlogtime;
	std::vector<DataDictionary>			_datum;
	std::vector<int>					_historian;
	std::vector<std::string>			_historiantimestamp;
	//std::vector<std::string>			_fields;
	//std::vector<std::string>			_values;
	static std::vector<DataDictionary>	_faultdatum;
	static std::vector<std::string>		_faultfields;
	std::string						_numberfields;
	DataDictionary _formats;
	DataDictionary						_typedatum;
	boost::timer						statetimer;
	static std::string					_faultitems; ;
	int  _laststate;
	int  _state;
	int GetState();
	double wpx;
	double dTraceWidth;
	int nSampleTimeSecs;
	std::string _ipaddr;

};

