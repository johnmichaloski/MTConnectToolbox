//
// OPCItem.h
//
// Description:
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
// **************************************************************************
#pragma once
#include <vector>
#include "atlcomtime.h"
#include "opcda_h.h"
#include "comutil.h"

#include "StdStringFcn.h"

class COPCItem
{
public:
	std::string _alias;
	std::string _tagname;
	std::string _type;
	std::string _subtype;
	_variant_t _value;
	_variant_t _lastvalue;
	FILETIME _time;
	OPCHANDLE _hServerHandleItem;
	OPCHANDLE _hClientItem;
	VARTYPE vt;
	FILETIME _starttime;
	std::string _sStartTime;
	bool _bCompleted;
	bool _bEnabled;

	COPCItem(std::string tag="")
	{
		_tagname=tag;
		_hServerHandleItem=NULL;
		_lastvalue=(long) 99999999999;
		_bEnabled=false;

	}
	template<typename C>
	C GetValue()
	{
		try {
			_bstr_t bstr= (_bstr_t) this->_value;
			C val;
			std::stringstream str;
			str<< (LPCSTR) bstr;
			str >> val;
			return val;
		}

		catch(...) {}

		return C();
	}
	std::string GetValue()
	{
		try {
			_bstr_t bstr= (_bstr_t) this->_value;
			return (LPCSTR) bstr;
		}

		catch(...) {}

		return "";
	}
};

template <class T>
class COPCItems : public std::vector<T *>
{
public:

	typedef std::vector<T *>::iterator COPCItemsIterator;
	COleDateTime _ftTimeStamp;
	long _nValidItems;

	//	~COPCItems();

	T *  FindItem(std::string tag)
	{
		COPCItemsIterator it;
		for(it = begin(); it<end(); it++)
		{
			if((*it)->_alias==tag)
				break;
		}

		if(it==end())
			return NULL;
		return *it;

	}
	T *  FindHandleItem(OPCHANDLE h)
	{
		COPCItemsIterator it;
		for(it = begin(); it<end(); it++)
		{
			if((*it)->_hServerHandleItem==h)
				break;
		}

		if(it==end())
			return NULL;
		return *it;

	}

	bool  ValidHandle(OPCHANDLE clienthandle)
	{
		COPCItemsIterator it;
		for(it = begin(); it<end(); it++)
		{
			if(clienthandle==(OPCHANDLE) (*it))
				return true;
		}
		return false;
	}

	void  Reset()
	{
		_nValidItems=0;
		for(COPCItemsIterator it = begin(); it<end(); it++)
		{
			(*it)->_hServerHandleItem=NULL;
			(*it)->_hClientItem=NULL;
			(*it)->_bEnabled=false;

		}
	}

	std::string  CreateItemsLogEntry()
	{
		USES_CONVERSION;
		// read and log each item
		std::string tmp;		
		tmp+=StdStringFormat(_T("%30s,"),  COleDateTime(_ftTimeStamp).Format(_T("%Y-%B-%d  %H:%M:%S"))); 

		for(int i=0; i<	size(); i++)
		{
			std::string value;
			try {
				if(!at(i)->_bEnabled) 
					value=_T("<INVALID TAG>");
				else
				{
					value= (LPCTSTR) _bstr_t(this->at(i)->_value);
				}
			}
			catch(...)
			{
				_variant_t tmp = this->at(i)->_value;
			}
			tmp+=StdStringFormat(_T("%30s,"), value.c_str() );
		}
		tmp+=_T("\n");
		return tmp;
	}

	std::string  CreateItemsHeader()
	{
		// read and log each item
		std::string tmp;		

		tmp+=StdStringFormat(_T("%30s,"), _T("TIME"));
		for(int i=0; i<size(); i++)
		{
			int n=this->at(i)->_alias.size()-26;
			if(n<0) 
				n=0; 
			tmp+=StdStringFormat(_T("...%27.27s,"),  this->at(i)->_alias.substr(n, 27).c_str());

		}
		tmp+=(_T("\n"));
		return tmp;
	}

	HRESULT SetItemValue(std::string tag,_variant_t val)
	{
		T * item = FindItem(tag);
		if(item==NULL)
			return E_FAIL;
		item->_lastvalue=item->_value;
		item->_value=val;
		return S_OK;
	}
	_variant_t GetItemValue(std::string tag)
	{
		T * item ;
		
		try {
			item = FindItem(tag);
			if(item==NULL)
				return _variant_t();
		}
		catch(...)
		{

			OutputDebugString((tag+" GetItemValue Failed\n").c_str());
			return _variant_t();
		}
		return item->_value;
	
	}
	template<typename C>
	C GetValue(std::string tag, C defaultVal=C())
	{
		T * item ;
		try {
			item = FindItem(tag);
			if(item==NULL)
				return C();
			else
			{
				_bstr_t bstr= (_bstr_t) item->_value;
				C val;
				std::stringstream str;
				str<< (LPCSTR) bstr;
				str >> val;
				return val;
			}
		}
		catch(...) {}

		return C();
	}
	std::string GetValue(std::string tag, const char * defaultVal)
	{
		return GetValue<std::string>(tag, std::string(defaultVal));
	}
	std::string DumpItems()
	{
		// read and log each item
		std::string tmp;		
		VarTypes vt;
	
		for(int i=0; i<size(); i++)
		{
			try{
			tmp+=StdStringFormat(_T("%-30s "),  this->at(i)->_alias.c_str());
			tmp+=StdStringFormat(_T("Enabled=%-10s"), ( this->at(i)->_bEnabled)? _T("True"): _T("False"));
			tmp+=StdStringFormat(_T("Type=%-10s"),  vt.Lookup( at(i)->_value.vt));
			tmp+=StdStringFormat(_T("Value=%-40s"),  (_bstr_t) at(i)->_value);
			tmp+=StdStringFormat(_T("Tag=%-40s "),  this->at(i)->_tagname.c_str());
			}catch(...) {}
			tmp+=_T("\n");

		}
		return tmp;
	}
};
