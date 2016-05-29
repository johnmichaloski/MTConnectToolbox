//
// ParameterTable.h
//
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once

#include "StringEx.h"
#include <map>
#include <set>

// TODO: add check if values change, after "freeze".
// TODO: add categories like property pages of VB
// TODO: tablize into vector<string vector> based on type info fields e.g, name, value, description, default, min, max,
// TODO: tablize into variant table.
// TODO: xml
// TODO: entire ini file read based on categories...

class CEnumWrapper 
{
	std::map<int,StringEx> enumerations;
	typedef std::map<int,StringEx>::iterator EnumIterator;
public:
	CEnumWrapper (StringEx description =L"")
	{
		ParseEnumerations(description);
	}
	int ParseEnumerations(StringEx description)
	{
		int n=0;
		if(description.Size() < 1) 
			return n;
		StringVector enums = description.Tokenize(L",");
		if(enums.size() <1) return n;
		for(int i=0; i< enums.size(); i++)
		{
			StringVector fields = enums[i].Tokenize("=");
			if(fields.size() < 2) continue;
			StringEx sNum = fields[0].Trim();
			StringEx sValue = fields[1].Trim();
			int num;
			if(FAILED( sNum.GetInt(&num))) continue;
			enumerations[num]=sValue;

		}
		return n;
	
	}
	int FindEnumeration(StringEx description)
	{
		if(enumerations.size() < 0) return -1;
		EnumIterator it = enumerations.begin();
		for(it = enumerations.begin(); it != enumerations.end(); it++)
		{
			if((*it).second.equals(description))
				return (*it).first;
		}
		return -1;
	}
	StringEx FindEnumeration(int num)
	{
		if(enumerations.size() < 0) return L"";
		EnumIterator it = enumerations.begin();
		for(it = enumerations.begin(); it != enumerations.end(); it++)
		{
			if((*it).first == num)
				return (*it).second;
		}
		return L"";
	}
	StringVector GetEnumerations()
	{
		StringVector enums;
		EnumIterator it = enumerations.begin();
		for(it = enumerations.begin(); it != enumerations.end(); it++)
		{
			enums.push_back((*it).second);
		}
		return enums;
	}
};
/**
* TypeInfo is the class definition for ParameterTable items/records.
* Each class contains:
* <UL>
* <LI> LPSTR type name, </LI>
* <LI> LPVOID address (for storing Sand data),  </LI>
* <LI> bool isset,  </LI>
* <LI> bool flag for readonly,  </LI>
* <LI> StringEx default value,  </LI>
* <LI> StringEx providing a description. </LI> 
* </UL> 
* The default constructor signature contains parameters to define
* each of these values. Store/Retrieve are the primary methods to manipulate
* the data. IsSet is a method to determine if the parameter has been 
* initialized. 
*
*/
class TypeInfo
{
public:
	StringEx m_type;  // const char * so no need to delete or strdup
	void * m_addr;
	bool m_bIsSet;
	bool m_bReadOnly;
	StringEx m_sDefaultValue;
	StringEx m_sDescription;
	StringEx m_name;
	StringEx m_category;
	StringEx m_sComments;

	TypeInfo & operator = (const TypeInfo &t)
	{
		this->m_category=t.m_category;
		m_name=t.m_name;
		m_addr=t.m_addr;
		m_type=t.m_type;
		m_bIsSet=t.m_bIsSet;
		m_bReadOnly=t.m_bReadOnly;
		m_sComments=t.m_sComments;
		m_sDescription=t.m_sDescription;
		m_sDefaultValue=t.m_sDefaultValue;
		return *this;
	}

	bool IsSet() 
	{ 
		return m_bIsSet; 
	}

	TypeInfo()
	{
		m_name = L"None";
		m_addr=NULL;
		m_type= "None";
		m_bIsSet=false;
		m_bReadOnly=false;
		m_sDescription=L"";
		m_sDefaultValue=L"None";
		m_sComments=L"";
		m_category=L"";
	}
	
	TypeInfo(StringEx name, const char * type="", void * addr=NULL, StringEx sComments=L"", StringEx sDescription=L"" ,StringEx sDefaultvalue=L"", StringEx sMinVal=L"" , StringEx sMaxVal=L"")  
	{
		m_name = name;
		m_addr=addr;
		m_type = (char *) type;
		m_sComments=sComments;
		
		if(!sDefaultvalue.IsEmpty())
		{
			m_sDefaultValue=sDefaultvalue;
			Store(sDefaultvalue);
		}
		if(!sDescription.IsEmpty())
		{
			m_sDescription=sDescription;
		}
		m_bIsSet=false;
		m_bReadOnly=false;
	}
	/**
	* Store sets a records value using a character StringEx as data. Using the type,
	* it determines how to store the value. Currently, the following types are supported:
	* double, int, long, unsigned long, class StringEx, class CString.
	* Can be optimized to keep wide chars or just chars. UINT
	* Note documentation claims the use of %D  allows an integer in decimal, or (if it starts with 0) octal, or (if it starts with 0x) hexadecimal notation. Hence, sscanf("12", "%D", i), sscanf("014", "%D", i) and sscanf("0xC", "%D", i) all yield the value 12 in i.
	*
	*/
	void Store(StringEx str)
	{
		if(str.IsNull()) return;  // could be assert
		if(m_bReadOnly) return;

		if(m_addr==NULL) return;
		else m_bIsSet=true;

		int n=0; 
		if(m_type.equals(L"double"))  n=sscanf((char *) str,"%lf", m_addr);
		else if(m_type.equals(L"int") || m_type.equals(L"long") || m_type.equals(L"unsigned long")  || m_type.equals(L"OLE_COLOR"))
		{
			StringEx format;
			bool bHex=false;
			if(	str.Find("0x", ffIgnoreCase))
			{
				bHex=true;
				str.Uppercase();
			}
			if(m_type.equals(L"int")) 
			{
				format = (bHex) ? L"%X" : L"%d";
			}
			else if(m_type.equals(L"long") || m_type.equals(L"OLE_COLOR") )
			{
				format = (bHex) ? L"%lX" : L"%D";
			}
			else if(m_type.equals(L"unsigned long"))
			{
				format = (bHex) ? L"%lX" : L"%lu";
			}
			n=sscanf(str,(char *) format, m_addr);
		}
		else if(m_type.equals(L"short")) n=sscanf((char *)str,"%hd", m_addr);
		else if(m_type.equals(L"char")) n=sscanf((char *)str,"%c", m_addr);
		else if(m_type.equals(L"unsigned char")) n=sscanf((char *)str,"%hhu", m_addr);
		else if(m_type.equals(L"lhex")) n=sscanf((char *)str,"%lX", m_addr); //  non-standard
//		else if(!strcmp(m_type,"unsigned long")) n=sscanf((char *)str,"%lu", m_addr);
		else if(m_type.equals(L"unsigned int" )) n=sscanf((char *)str,"%u", m_addr);
		else if(m_type.equals(L"bool" )) 
		{
			n=sscanf((char *)str,"%hhu", m_addr);
			if(n==0)
			{
				bool * bValue =(bool *) m_addr;
				(*bValue)=(bool) -1;
				if(str.equals(L"true",ffIgnoreCase))*bValue=1;
				if(str.equals(L"false",ffIgnoreCase))*bValue=0;
				if(*bValue>=0) n=1;
			}
		}
		else if(m_type.equals(L"BSTR" ))
		{
			SysFreeString((*(BSTR *)m_addr));
			(*(BSTR *)m_addr)= (BSTR) str.Detach();
		}
		//this is not safe, need array size really
		else if(m_type.equals(L"char *" ))
		{
			strcpy(((char *)m_addr), str.Get());
			n=1;
		}
		else if(m_type.equals(L"class StringEx" )  || m_type.equals(L"Filename" )  || m_type.equals(L"Directory" ))
		{
			StringEx * ptr = (StringEx *) m_addr;
			ptr->Load((BSTR) str);
			n=1;
		}
#if 0
		else if(m_type.equals(L"COleDateTime" ))	
		{
			COleDateTime *v =(COleDateTime *) m_addr;
			v->ParseDateTime((LPTSTR) str);
		}
#endif
		else if(m_type.equals(L"class _variant_t" ))
		{
			try{
				_variant_t varSrc((BSTR) str);
				_variant_t * var = (_variant_t *) m_addr;
				var->ChangeType(var->vt, &varSrc);
				n=1;

			} catch(...)
			{
				OutputDebugString(StringEx("ParameterTable Store \"%S\" of type _variant_t Exception: %S\n", (BSTR) m_name, (BSTR) str));
	 			ATLTRACE2(atlTraceGeneral,2,_T("ParameterTable Store \"%S\" of type _variant_t Exception: %S\n"), (BSTR) m_name, (BSTR) str);
				
			}

		}
		else if(m_type.equals(L"Enumeration" ) )
		{
			int  *pnValue = (int *) m_addr;
			CEnumWrapper enums;
			enums.ParseEnumerations(m_sComments);
			*pnValue= enums.FindEnumeration(str);
			n=1;
		}
#ifdef CSTRING
		else if(m_type.equals(L"class CString" ) )
		{
			CString * ptr = (CString *) m_addr;
			(*ptr) = (char *) str;
			n=1;
		}
#endif
		if(n!=1)
		{
			// Drastic measures, but for now ok
			OutputDebugString(StringEx("ParameterTable Store \"%S\" of type %s Failed: %S\n", (BSTR) m_name, m_type, (BSTR) str));
		}
	}

	/**
	* Produces a StringEx describing the current value of the parameter item.
	* The returned StringEx is "trimmed" of any leading or trailing white spaces.
	* Fixme: add format dimension for printing numeric items.
	*/
	StringEx Retrieve()
	{
		char tmp[128] = {0};
		if(m_addr==NULL) return L"";

		if(m_type.equals(L"double" ))  sprintf(tmp,"%lf", *((double *) m_addr));
		else if(m_type.equals(L"int" ))	sprintf(tmp,"%d", *((int *) m_addr));
		else if(m_type.equals(L"long" ) )	sprintf(tmp,"%d", *((int *) m_addr));
		else if(m_type.equals(L"OLE_COLOR" ))	sprintf(tmp,"0x%x", *((int *) m_addr));
		else if(m_type.equals(L"bool" ))
		{
			bool * bVal = (bool *) m_addr;
			//sprintf(tmp,"%hhu", *((bool *) m_addr));
			sprintf(tmp,"%s", (* bVal) >0? "TRUE" : "FALSE");
		}
		else if(m_type.equals(L"unsigned int" ))	sprintf(tmp,"%u", *((unsigned int *) m_addr));
		else if(m_type.equals(L"unsigned long" ))	sprintf(tmp,"%u", *((unsigned long *) m_addr));
		else if(m_type.equals(L"short"))	sprintf(tmp,"%hd", *((short *) m_addr));
		else if(m_type.equals(L"char"))	sprintf(tmp,"%c", *((char *) m_addr));
		else if(m_type.equals(L"unsigned char"))	sprintf(tmp,"%hhu", *((int *) m_addr));
		else if(m_type.equals(L"lhex"))	sprintf(tmp,"%x", *((long *) m_addr));
		else if(m_type.equals(L"BSTR"))	sprintf(tmp,"%S", *((BSTR *) m_addr));
		else if(m_type.equals(L"char *"))
		{
			strncpy(tmp,(char *) m_addr, 128);
		}
#if 0
		else if(m_type.equals(L"COleDateTime"))	
		{
		StringEx tmp =L"";
			try{
				tmp =(LPCTSTR) ((COleDateTime *)m_addr)->Format(_T("%A, %B %d, %Y"));
			} catch(...){}
		return tmp;

		}
#endif
		else if(m_type.equals(L"class _variant_t"))
		{
			StringEx tmp =L"";
			try{
				_variant_t ptr;
				ptr=(VARIANT*)m_addr;
				tmp = (BSTR) (_bstr_t) ptr;
			} catch(...)
			{
				OutputDebugString(StringEx("ParameterTable Retrieve \"%S\" of type _variant_t Exception\n", (BSTR) m_name));
				ATLTRACE2(atlTraceGeneral,2,_T("ParameterTable Retrieve \"%S\" of type _variant_t Exception\n"), (BSTR) m_name);
			}
			return tmp;
		}
		else if(m_type.equals(L"Enumeration"))
		{
			int *iPtr =(int *) m_addr;
			*iPtr= 1;
			/*
			int  *pnValue = (int *) m_addr;
			CEnumWrapper enums;
			enums.ParseEnumerations(m_sComments);
			return enums.FindEnumeration(*pnValue);
			*/
		}
		else if(m_type.equals(L"class StringEx")  || m_type.equals(L"Filename")  || m_type.equals(L"Directory") )
		{
			StringEx * ptr = (StringEx *) m_addr;
			if(ptr->IsNull()) return L"";
			else return StringEx(ptr);
		}
#ifdef CSTRING

		else if(m_type.equals(L"class CString"))
		{
			WTL::CString * ptr = (WTL::CString *) m_addr;
			if(ptr==NULL) return L"";
			StringEx tmp = (LPTSTR) (LPCTSTR) (*ptr);
			return tmp;
		}
#endif
		return tmp;
	}

};

typedef std::map<StringEx,TypeInfo> TypeInfoTable;

/**
* ParameterTable is a class to interface to an ini file like StringEx.
* The table is initialized with a parameter name as a StringEx,
* the parameter type using either the C++ construct typeid or supplying
* a StringEx, and an address for storing and retrieving data values.
* Currently, the Parameter Table supports the following types:
* double, long, int, unsigned int, class CString (MFC), and class
* StringEx (OMAC). The methods ToString() and FromString()
* provide the basis for mapping StringEx to binary and vice versa.
* MissingToString() provides a means to determine if an ini StringEx omitted
* any parameters that are expected to included in the ParamterTable. Updates
* to the ParameterTable using FromString() do not need to include all the
* parameter entries, as only those that are "dirty" need be supplied.
* <P> The parameter table is a convenient mechanism to interface to
* ini config files, as well as creating StringEx for a MFC CListCtrl 
* GUI window. It is used extensively for interpreting config, parameter,
* and option data entries within the sample OMAC controller.
*<P> The parameter table is relatively easy to use. First, you declare
* an instance in a  header file, then you AddEntry(parameters) to entries
* to the parameter table. Then, you change the values using FromString giving
* it a StringEx containing parameter1=value cr/lf parameter2=value....
* Likewise, you create a new ini StringEx using the ToString to get
* a StringEx using each parameters current value.
* @example
	ParameterTable                     parameters;
	In the source file:

	parameters.AddEntry("Module Graph Index", typeid(m_nModuleGraphIndex).name(), &m_nModuleGraphIndex, L"-1");

  Get an ini like StringEx from somewhere...
	StringEx sOptions = m_directory.getSectionString(L"GUI");
  And have the parameter table read its value FromString(StringEx).
	parameters.FromString(sOptions);
  TO retrieve the values if they have been modified,
    StringEx newValues parameters.ToString();
*/
class ParameterTable  
{
public:
	TypeInfoTable entries;
	std::set<StringEx> categories;
	// NOTE CAN"T COPY AS REFERENCES MAYBE TO TRANSIENT CLASSES
	ParameterTable& operator=(const ParameterTable& s){ return *this; } 


	TypeInfo * AddEntry(StringEx name, const char * type="", void * addr=NULL, StringEx sComments=L"", StringEx sDescription=L"",  StringEx sDefaultvalue=L"", StringEx sMinVal=L"" , StringEx sMaxVal=L"")  
	{
		// Check to make sure entry does not already exist (does this matter?)
		// TypeInfoTable::iterator it = entries.find(name);
		// if(it!=entries.end()) return;  
		TypeInfo entry (name, type, addr, sComments, sDefaultvalue, sMinVal, sMaxVal);
		entries[name]= entry;
		return &entries[name];
	}
	TypeInfo & AddOptionalEntry(StringEx name, const char * type="", void * addr=NULL, StringEx sComments=L"",  StringEx sDescription=L"", StringEx sDefaultvalue=L"", StringEx sMinVal=L"" , StringEx sMaxVal=L"")  
	{
		// Check to make sure entry does not already exist (does this matter?)
		// TypeInfoTable::iterator it = entries.find(name);
		// if(it!=entries.end()) return;  
		TypeInfo entry (name, type, addr, sComments, sDefaultvalue, sMinVal, sMaxVal);
		entry.m_bIsSet=true;
		entries[name]= entry;
		return entries[name];
	}
	TypeInfo & AddReadOnlyEntry(StringEx name, const char * type="", void * addr=NULL, StringEx sDefaultvalue=L"", StringEx sDescription=L"", StringEx sMinVal=L"" , StringEx sMaxVal=L"")  
	{
		// Check to make sure entry does not already exist (does this matter?)
		// TypeInfoTable::iterator it = entries.find(name);
		// if(it!=entries.end()) return;  
		TypeInfo entry (name, type, addr, sDefaultvalue, sMinVal, sMaxVal);
		entry.m_bReadOnly=true;
		entries[name]= entry;
		return entries[name];
	}
	TypeInfo & AddCategoryEntry(StringEx category, StringEx name, const char * type="", void * addr=NULL, StringEx sComments=L"",  StringEx sDescription=L"", StringEx sDefaultvalue=L"", StringEx sMinVal=L"" , StringEx sMaxVal=L"")  
	{
		TypeInfo entry (name, type, addr, sComments, sDefaultvalue, sMinVal, sMaxVal);
		entry.m_category = category;
		entries[name]= entry;
		categories.insert(category);
		return entries[name];
	}

	void Clear()
	{
		entries.clear();
	}
	void Merge(ParameterTable &table)
	{
		entries.insert(table.entries.begin(), table.entries.end());
		categories.insert(table.categories.begin(), table.categories.end());
	}

	HRESULT RemoveEntry(StringEx name)
	{
		if(name.IsNull() || name.IsEmpty())
			return E_INVALIDARG;
		// Check to make sure entry does not already exist (does this matter?)
		// TypeInfoTable::iterator it = entries.find(name);
		// if(it!=entries.end()) return;  
		TypeInfoTable::iterator it;
		it = entries.find(name);
		if(it==entries.end())
			return E_FAIL;
		entries.erase(it);
		return S_OK;
	}
	TypeInfo * GetEntryProfile(StringEx parameter)
	{
		StringEx tmp;
		TypeInfoTable::iterator it = entries.find(parameter);
		if(it==entries.end())
			return NULL;

		return &((*it).second);

	}

	TypeInfo * SetEntryProfileParameter(StringEx parameter, StringEx profileParam, StringEx value)
	{
		StringEx tmp;
		TypeInfoTable::iterator it = entries.find(parameter);
		if(it==entries.end())
			return NULL;

		TypeInfo * info=&(*it).second;
		if(profileParam.equals(L"DefaultValue"))
				info->m_sDefaultValue=value;
		else if(profileParam.equals(L"Description"))
				info->m_sDescription=value;
		else if(profileParam.equals(L"Category")) 
				info->m_category=value;
		else if(profileParam.equals(L"Comments")) 
				info->m_sComments=value;
		
		return info;

	}
	StringEx GetEntry(StringEx parameter)
	{
		StringEx tmp;
		TypeInfoTable::iterator it = entries.find(parameter);
		if(it==entries.end())
			return StringEx(L"");

		return (*it).second.Retrieve();

	}

	HRESULT SetEntry(StringEx parameter, StringEx value)
	{
		StringEx tmp;
		TypeInfoTable::iterator it = entries.find(parameter);
		if(it==entries.end())
			return E_FAIL;

		(*it).second.Store(value);
		return S_OK;

	}

	/**
	* Accept a StringEx based on an ini StringEx format, with an
	* equal sign delimiting parameter=value pairs separated by line feeds.
	*/
	virtual StringEx FromString(StringEx iniSection)
	{
		StringEx error=L"";
		//iniSection.Replace(L";", L"\n", L"");
		unsigned int i;
		StringVector lines = iniSection.Tokenize("\n");
		for(i=0; i< lines.size(); i++)
		{
			StringVector  alltokens=lines[i].Tokenize(";");
			if(alltokens.size() < 1) 
				continue;
			if(!alltokens[0].Find(L"="))continue;
			StringVector tokens = alltokens[0].Tokenize("=");
			// May be blank
			if(tokens.size()==1)
			{
				tokens.push_back(L"");
			}
			tokens[0].Trim();
			tokens[1].Trim();
			StringEx parameter = tokens[0];

			TypeInfoTable::iterator it = entries.find(parameter);
			if(it!=entries.end())
			{
				TypeInfo typeinfo = (*it).second;
				// May need type and enumeration
				if(alltokens.size()>1 && alltokens[1].Size() > 0) 
					typeinfo.m_type=alltokens[1].Trim();
				if(alltokens.size()>2) 
					typeinfo.m_sComments=alltokens[2].Trim();
				typeinfo.Store(tokens[1]);
			}
			else
			{
				 error.Append("Parameter %S is not a valid parameter\n", (BSTR) tokens[0]);
			}
		}
		return error;
	}
	StringEx ToString()
	{
		StringEx tmp=L"";
		TypeInfoTable::iterator it;
		for(it=entries.begin(); it!=entries.end(); it++)
		{
			TypeInfo typeinfo = (*it).second;
			tmp+=typeinfo.m_name;
			tmp+=" = ";
			tmp+=typeinfo.Retrieve();
			tmp+=" ; ";
			tmp+=typeinfo.m_type;
			tmp+=" ; ";
			tmp+=typeinfo.m_sComments;
			tmp+=" \n";
		}
		return tmp;
	}



/**
* Create an ini-like StringEx describing the parameter/value pairs, using
* an equal sign to separate the parameter=value pair, and a line feed
* to separate parameter entries.
*/
	StringEx ToDisplayString()
	{
		StringEx tmp=L"";
		TypeInfoTable::iterator it;
		for(it=entries.begin(); it!=entries.end(); it++)
		{
			TypeInfo typeinfo = (*it).second;
			tmp+=typeinfo.m_name;
			tmp+=" = ";
			tmp+=typeinfo.Retrieve();
			tmp+=" \n";
		}
		return tmp;
	}
	StringEx MissingToString()
	{
		StringEx tmp=L"";
		TypeInfoTable::iterator it;
		for(it=entries.begin(); it!=entries.end(); it++)
		{
			StringEx name = (*it).first;
			if(!(*it).second.IsSet()) 
			{
				tmp+=(*it).first;
				tmp+=" not set\n";
			}
		}
		return tmp;
	}

	TypeInfo &  operator [] (const StringEx str)
	{
		return entries[str];
		
	}

	TypeInfo &  operator [] (const int n)
	{
		TypeInfoTable::iterator it=entries.begin();
		if(n<0) 
			return (*it).second;;
		int i=0;
		for(it=entries.begin(); i!=n && it!=entries.end(); it++, i++)
		{
		}
		if(it==entries.end())
			it= entries.begin();
		return (*it).second;
		
	}

	/**
	* Get a parameter's value as a StringEx given an ini StringEx, ith an
	* equal sign delimiting parameter=value pairs separated by line feeds.
	* @return blank StringEx if not found
	*/
	static StringEx ParameterValueFromIniString(StringEx iniSection, StringEx parameter)
	{

		// Remove comments
		//iniSection.Replace(L";", L"\n", L"");

		// Tokenize lines
		StringVector lines = iniSection.Tokenize("\n");

		// Search for matching parameter=value entry
		for(unsigned int i=0; i< lines.size(); i++)
		{
			if(!lines[i].Find(L"="))continue;
			StringVector  tokens=lines[i].Tokenize("=;");
			// May be blank

			// Skip as may not valie
			if(tokens.size()<2) continue;

			tokens[0].Trim();
			tokens[1].Trim();

			if(parameter.equals(tokens[0], true))
			{
				return tokens[1];
			}
	
		}
		return L"";
	}
	StringEx strings[100];
	unsigned int CreateDynamicTable(StringEx iniSection, int nFields=2, StringEx delimiters = L"=")
	{
		unsigned int n=0;
		entries.clear();
		// Need to change from var1=2 var2=3 .. to add line feeds
		iniSection.Append("\n");

		StringVector lines = iniSection.Lines();
		// Search for matching parameter=value entry
		for(unsigned int i=0; i< lines.size(); i++)
		{
			StringVector  tokens=lines[i].Tokenize(delimiters);
			// May be blank
			
			
			// Skip as may not valie
			if(tokens.size()<nFields) continue;
			for(int j=0; j<nFields; j++) tokens[j].Trim();
			
			strings[i]=tokens[1];
			TypeInfo * entry = AddEntry(tokens[0], "class StringEx", &strings[n], tokens[1]);
			entry->Store(tokens[1]);
			n++;
			
		}
		return n;
	}

	StringVector ToStringVector(const TCHAR* col1=NULL, ...)
	{
		va_list argList;
		va_start(argList, col1);
		StringVector cols;
		StringVector strs;
		TypeInfoTable::iterator it;
		if (col1) {
			const TCHAR* ptr = col1;

			while (ptr) {
				cols.push_back(ptr);
				ptr = va_arg(argList, TCHAR*);
			}
			va_end(argList);
		}
		else
		{
			for(it=entries.begin(); it!=entries.end(); it++)
			{
				cols.push_back((*it).second.m_name);
			}
		}

		for(int i=0; i< cols.size(); i++)
		{
			it=entries.find(cols[i]);
			if(it!=entries.end())
			{
				TypeInfo typeinfo = (*it).second;
				strs.push_back(StringEx(typeinfo.Retrieve()));

			}
		}
		return strs;
	}
};


class ParameterString  : public ParameterTable
{
public:
	StringEx FromString(StringEx iniSection, StringEx delimiter = L";")
	{
		// Need to change from var1=2 var2=3 .. to add line feeds
		iniSection.Replace(delimiter, StringEx(L"\n"));
		iniSection.Append("\n");
		return ParameterTable::FromString(iniSection);
	}

	StringEx ToString(StringEx delimiter = L" ")
	{
		StringEx tmp = ParameterTable::ToString();
		tmp.Replace(L"\n", (BSTR) delimiter);
		return tmp;
	}

};
/*
<table width="100%" border="0" cellpadding="0" cellspacing="0">
<tr><td bgcolor="#9999CC">
*/
class  HTMLParameterTable : public ParameterTable
{
public:
	HTMLParameterTable& operator=(const HTMLParameterTable& s){ return *this; } 
	StringEx tablewidth;
	StringEx tableborder ;
	StringEx tablebgcolor ;
	StringEx cellpadding;
	StringEx cellspacing;
	StringEx tablestyle;
	StringEx headeralign;
	StringEx headerstyle;
	long headercolor;

	StringTable m_strTableData;
	StringVector m_strTableHeader;
	HTMLParameterTable()
	{
		tablewidth=L"400";
		tableborder=L"3" ;
		cellpadding=L"5";
		cellspacing=L"2";
		//tablebgcolor=_L"#FFFFCC");
		// style="background-color:yellow;border:3px dashed black;">
		//  text-align:left
		//tablestyle=_L"font-size: 12pt; color: fuchsia");
		tablebgcolor=L"#FFFFFF";
		tablestyle=L"font-size: 12pt; color: black";  
		headercolor= 0xFFFFCC;
		headercolor = 0x9999CC;
		headeralign=L"center";
		headerstyle=L"font-size: 12pt; font-weight: bold; font-style : italic; color: blue";

	}
	void Clear() {
		m_strTableData.clear(); 
		m_strTableHeader.clear();
	}
	void AppendTableRow(StringVector data )
	{
		m_strTableData.push_back(data);
	}
	void LoadTableRow(int i, StringVector data )
	{
		if(i< m_strTableData.size())
			m_strTableData[i]=data;
	}

	void LoadTableHeader(const TCHAR* col1, va_list argList)
	{
		StringVector cols;
		m_strTableHeader.clear();
		if (col1) {
			const TCHAR* ptr = col1;

			while (ptr) {
				cols.push_back(ptr);
				m_strTableHeader.push_back(ptr);
				ptr = va_arg(argList, TCHAR*);
			}
			va_end(argList);
		}
	}
	void LoadTableHeader(const TCHAR* col1,...)
	{
		va_list argList;
		va_start(argList, col1);
		LoadTableHeader(col1,  argList);
	}

	void LoadTableCells(const TCHAR* col1, va_list argList)
	{
		StringVector cols;
		m_strTableData.clear();
		LoadTableHeader(col1, argList);
		TypeInfoTable::iterator it;
		StringVector strs;
		for(int i=0; i< cols.size(); i++)
		{
			it=entries.find(cols[i]);
			if(it!=entries.end())
			{
				TypeInfo typeinfo = (*it).second;
				strs.push_back(StringEx(typeinfo.Retrieve()));

			}
		}
		m_strTableData.push_back(strs);
	}
	StringEx TableFormat()
	{
		return StringEx("<table bgcolor=\"%S\" width=\"%S\" border=\"%S\" cellpadding=\"%S\" cellspacing=\"%S\" style=\"%S\">\n",
			(BSTR) tablebgcolor, (BSTR) tablewidth, (BSTR) tableborder, (BSTR) cellpadding, (BSTR) cellspacing,(BSTR) tablestyle);
	}
	StringEx TableHeaderRowFormat(){
		return StringEx("<tr align=\"%S\" bgcolor=\"#%06X\" style=\"%S\">\n", (BSTR) headeralign, headercolor, (BSTR) headerstyle); } 
	StringEx TableRowFormat() {}

	StringEx ToHtmlTable(const TCHAR* col1, ...)
	{
		StringEx tmp;
		va_list argList;
		va_start(argList, col1);
		LoadTableCells(col1, argList);
		return  ToHtmlTable();
	}

	StringEx ToHtmlTable()
	{
		int i,j;
		StringEx tmp;
		tmp+=TableFormat();
		tmp+=TableHeaderRowFormat();
		for(i=0; i< m_strTableHeader.size(); i++)
		{
			tmp.Append("<TD>%S</TD>\n", (BSTR) m_strTableHeader[i]);
		}
		tmp+=_T("</TR>\n");

		for(i=0; i< m_strTableData.size(); i++)
		{
			tmp+=_T("<TR>");
			for(j=0; j< m_strTableData[i].size(); j++)
			{
				tmp+=_T("<TD>");
				tmp+=m_strTableData[i][j];
				tmp+=_T("</TD>");
			}
			tmp+=_T("</TR>\n");

		}
		tmp+=_T("</TABLE>\n");

		return (const TCHAR *) tmp;		
	}

};