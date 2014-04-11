//
// FactoryIntegrator.h
//
#pragma once


#include <string>
#include <vector>
#include <map>

#include <comdef.h>
//#include <boost/shared_ptr.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <algorithm>

#include "StdStringFcn.h"

#define ASSIGN(X,Y,Z) try { X=Y; } catch(...) { X=Z;};
#define CREATEIF(X,Y) try { if(Y != bstr_t(L"None") && Y != bstr_t(L"") ) X=std::string((LPCSTR)Y); } catch(...) { };
#define CREATEIFBSTR(X,Y) try { if(Y != bstr_t(L"None") && Y != bstr_t(L"")) X=Y; } catch(...) { };
class IObject;
typedef IObject * IObjectPtr;
class CResourceHandler;

extern void SetupFactory(double dFeedoveride=1.0, 
	double dSetupFactor=1.0);

class IObject
{
public:
	typedef std::multimap<std::string, std::vector<std::string> >::iterator PropIterator;
	typedef std::vector<std::string>  PropVector;
	//bstr_t identifier;
	//bstr_t name;
	//std::vector<bstr_t> properties;
	std::multimap<std::string, PropVector > properties;
	std::vector<bstr_t> distributions;
	////////////////////////////////////////////////////////
	virtual bstr_t GetClassname(){return L"None";}
	int GetNObject()
	{
		static int i=0;
		i++;
		return i;
	}
	std::vector<IObjectPtr> & GetObjects()
	{
		static std::vector<IObjectPtr>  _objects;
		return _objects;
	}
	template<typename T>
	static T * Create(std::string id="")
	{
		T* t=new T();
		T::_objects.push_back(t);
		if(!id.empty())
			t->identifier=id.c_str();
		return t;
	}

	template<typename T,  _bstr_t T::*Var>
	static T * FindObjectByVariable(bstr_t id)
	{
		for(int i=0; i< T::_objects.size(); i++)
		{
			T * t ( (T *) T::_objects.at(i));
			if(t->*Var==id)
				return t;

		}
		return NULL;
	}
	template<typename T, std::size_t N> std::size_t NVAL(T (&)[N]) { return N; }

	void AddProperty(std::string name, std::string value, std::string units, std::string description)
	{
		std::string props[]= {name, value, units, description } ; // ,"+units+","+description;
				
		std::vector<std::string> prop(&props[0], &props[0] +NVAL(props));
		properties.insert(std::pair<std::string, PropVector>(name, prop));
	
	}

	void AddProperty(std::string name, std::string value, std::string units, std::string description, std::string modifier)
	{
		std::string props[]= {name, value, units, description,modifier } ; // ,"+units+","+description;
				
		std::vector<std::string> prop(&props[0], &props[0] +NVAL(props));
		properties.insert(std::pair<std::string, PropVector>(name, prop));
	
	}

	template<int n >
	std::string GetPropertyValue(bstr_t name)
	{
		// Could be multiple keys with same name!
		std::pair<std::multimap<std::string, PropVector>::iterator, std::multimap<std::string, PropVector>::iterator> ppp;
		ppp = properties.equal_range((LPCSTR) name);
		if(ppp.first==properties.end())
			return "";
		//std::multimap<std::string, PropVector>::iterator it = ppp.first;
		std::vector<std::string> props = (*ppp.first).second; // properties[(LPCSTR) name] ;
		if(n>=props.size())
			return "";
		return props[n];
	}

	template<int n >
	std::vector<PropIterator> GetPropertyMatches(bstr_t filter)
	{
		std::vector<PropIterator > matches;
		std::string sfilter((LPCSTR) filter);
		for(PropIterator it = properties.begin(); it!=properties.end(); it++)
		{
			std::vector<std::string> props = (*it).second ;
			if(n>=props.size())
				continue;
			if(props[n] == sfilter)
				matches.push_back(it);
		}
		return matches;
	}

	std::string GetProperty(PropIterator index, int field) 
	{ 
		//if(index < properties.begin() || index >= properties.end())	return ""; 
		
		std::vector<std::string> props = (*index).second ;
		if(field<0 || field>=props.size())
			return "";
		
		return props[field];
	}
	std::string GetPropertyName(std::string name) { return GetPropertyValue<0>(name.c_str()); }
	std::string GetPropertyValue(std::string name) { return GetPropertyValue<1>(name.c_str()); }
	std::string GetPropertyUnits(std::string name) { return GetPropertyValue<2>(name.c_str()); }
	std::string GetPropertyDescription(std::string name){ return GetPropertyValue<3>(name.c_str()); }

};
#define	BEGIN_METADATA(X)\
	static  std::vector<IObjectPtr>  _objects;\
	virtual std::vector<IObjectPtr> * objectsPtr() { return &_objects; }\
	virtual bstr_t GetClassname(){return bstr_t(#X);}

#define	IMPLEMENT_METADATA(X)\
	__declspec(selectany) std::vector<IObjectPtr>  X::_objects;

class MyStructMeta;
class PropertyElement ;
struct DistributionReferenceProperty;
struct Shift;
struct Calendar;
struct Schedule;
struct Distribution;
struct Resource;
struct Cell;
struct Layout;
struct Part;
struct Process;
struct ProcessPlan;
struct Job;

typedef PropertyElement* PropertyElementPtr;
typedef DistributionReferenceProperty* DistributionReferencePropertyPtr;
typedef Shift* ShiftPtr;
typedef Calendar* CalendarPtr;
typedef Schedule* SchedulePtr;
typedef Distribution* DistributionPtr;
typedef Resource* ResourcePtr;
typedef Cell* CellPtr;
typedef Layout* LayoutPtr;
typedef Part* PartPtr;
typedef Process* ProcessPtr;
typedef ProcessPlan* ProcessPlanPtr;
typedef Job* JobPtr;


inline bool DataExists(bstr_t data) { return (data != bstr_t(L"None")); }
struct caseinsenstive_less
{
	bool operator() (const bstr_t & s1, const bstr_t & s2) const
	{
		return VarBstrCmp(const_cast<bstr_t&>( s1).GetBSTR() , const_cast<bstr_t&>( s2).GetBSTR(), LOCALE_USER_DEFAULT , NORM_IGNORECASE)  < 1;
	}
};

typedef std::map<bstr_t, bstr_t,caseinsenstive_less> MappedValues;

	// <Property><Name>SerialNumber</Name><Value>111-222-556677-6</Value></Property>
			//	<Property>
			//	<Name>arrivals</Name>
			//	<Description>interarrival time for parts of this type at machine 1</Description>
			//	<DistributionDefinitionReference>
			//		<DistributionDefinitionIdentifier>dddd:111</DistributionDefinitionIdentifier>
			//	</DistributionDefinitionReference>
			//</Property>

class PropertyElement //: public IObject
{
public:
	PropertyElement(bstr_t _name=L"None", bstr_t _value=L"None"){}

	bstr_t identifier; // for identification purposes only - not part of CMSD XML
	bstr_t type;
	
	bstr_t name;
	bstr_t value;
	bstr_t units;
	bstr_t description;
	//static std::vector<bstr_t>& GetTypeList();

};


struct Time
{
	Time() {time="None";  units=""; }
	bstr_t time;
	bstr_t units;
};
struct TimePeriod
{
	TimePeriod() { starttime="None";units=duration=""; }
	bstr_t starttime;
	bstr_t units;
	bstr_t duration;
};
struct Shift : public IObject
{
	Shift()
	{
		identifier=StdStringFormat("Shift%d",GetNObject()).c_str();
		description=startTime=duration=applicableDay=L"None";
	}
	bstr_t identifier;
	bstr_t description;

	bstr_t startTime;
	bstr_t duration;
	bstr_t applicableDay;
	std::vector<TimePeriod> breaks;
	BEGIN_METADATA(Shift);

};
struct Documentation : public IObject
{
	Documentation()
	{
		description=L"None";
	}
	bstr_t description;

	BEGIN_METADATA(Documentation);
};
struct CostAllocation : public IObject
{
	CostAllocation()
	{
		name=description=type=category=currencyType=L"None";
	}

	bstr_t name;
	bstr_t description;

	bstr_t type;
	bstr_t category;
	bstr_t currencyType;
	BEGIN_METADATA(CostAllocation);

};

struct Calendar : public IObject
{
	Calendar()
	{
		static int i=0;
		identifier=StdStringFormat("Calendar%d",GetNObject()).c_str();
		description=startdate=enddate=L"None";
	}
	int ShiftsPerWeek();
	double HoursPerWeek();

	bstr_t identifier;
	bstr_t startdate;
	bstr_t enddate;
	bstr_t description;
	std::vector<Shift*> shifts;
	std::vector<std::string> shiftIds; // ids to shifts
	BEGIN_METADATA(Calendar);
};

struct Schedule : public IObject
{
	Schedule()
	{
		identifier=StdStringFormat("Schedule%d",GetNObject()).c_str();
		description="";
	}

	bstr_t identifier;
	bstr_t description;
	BEGIN_METADATA(Schedule);

};

struct Distribution : public IObject
{
	Distribution()
	{
		identifier=StdStringFormat("Distribution%d",GetNObject()).c_str();
		name=description=distreference=distname=distdescription=units=L"None"; 
		name1= value1= name2= value2= name3= value3="";
	}

	//static std::map<bstr_t, Distribution> distributions;
	bool IsEmpty() { return name == _bstr_t(L"None"); }
	//bstr_t MapArenaDistribution() ;
	///////////////////////////////////////////////////
	// type DistributionDefinition or Property
	enum DistType { Property, DistributionDefinition};
	bstr_t identifier;
	bstr_t name;
	bstr_t description;
	bstr_t units;
	bstr_t type;

	// Property
	bstr_t distname; 
	bstr_t distdescription;
	bstr_t distreference;
	MappedValues mappedvalues;
	bstr_t name1;
	bstr_t value1;
	bstr_t name2;
	bstr_t value2;
	bstr_t name3;
	bstr_t value3;

	BEGIN_METADATA(Distribution);

};
struct PartProgram : public IObject
{
	PartProgram()
	{
		identifier=StdStringFormat("PartProgram%d",GetNObject()).c_str();

	}
	bstr_t name;
	bstr_t identifier;
	bstr_t type;
	int duration; // seconds

	BEGIN_METADATA(PartProgram);
};
struct Resource : public IObject
{
	Resource()
	{
		identifier=StdStringFormat("Resource%d",GetNObject()).c_str();
		//model=serial_number=capacity=name=manufacturer=
		type=description=hourlyRate=hourlyRateUnit=L"None";
		mtbfid=mttrid=L"None";
	}

	bool IsEmpty() { return name == _bstr_t(L"None"); }
	bstr_t name;
	bstr_t identifier;
	bstr_t type;
	bstr_t description;
	bstr_t hourlyRate;
	bstr_t hourlyRateUnit;

	int nMaxPalletSize; // maybe width x height instead

	// Properties
//	bstr_t serial_number;
//	bstr_t capacity;
//	bstr_t manufacturer;
	bstr_t model;
	bstr_t mtbfid;
	bstr_t mttrid;
	Distribution *mttr;
	Distribution *mtbf;

	std::map<Distribution,Distribution> downtimes;
	//std::vector<PropertyElement> simpleproperties;
	void Zero()
	{
		identifier = L"None";
		name= L"None";
		type= L"None";

		description=L"None";
		hourlyRate=L"None";
		hourlyRateUnit=L"None";
	}
	BEGIN_METADATA(Resource);

};

struct Cell : public IObject
{
	Cell()
	{
		identifier=StdStringFormat("Cell%d",GetNObject()).c_str();
		name=type=description=L"None"; 
	}
	bool IsEmpty() { return name == _bstr_t(L"None"); }

	bstr_t name;
	bstr_t identifier;
	bstr_t description;
	bstr_t type;
	std::vector<bstr_t> resourceIds;
	BEGIN_METADATA(Cell);
	
};
struct Layout : public IObject
{
	Layout()
	{
		identifier=StdStringFormat("Layout%d",GetNObject()).c_str();
		unit=CoordinateSystem=name=description=associatedResource=boundaryWidth=boundaryDepth=placementX=placementY=L"None";
	}

	bstr_t identifier;
	bstr_t name;
	bstr_t description;
	bstr_t associatedResource;
	bstr_t unit;
	bstr_t boundaryWidth;
	bstr_t boundaryDepth;
	bstr_t placementX;
	bstr_t placementY;
	bstr_t CoordinateSystem;
	BEGIN_METADATA(Layout);
}; 

struct Part   : public IObject
{
public:
	Part()
	{
		identifier=StdStringFormat("Part%d",GetNObject()).c_str();
		name=BOM=processplanidentifier=billOfMaterialsIdentifier=L"None";
		description=width= depth= height=sizeunits=weight=weightunits=L"";
		seenparts=goodparts=badparts=0;
	}
	bool IsEmpty() { return name == _bstr_t(L"None"); }
	bstr_t name;
	bstr_t identifier;
	bstr_t description;
	bstr_t BOM;
	bstr_t processplanidentifier;
	bstr_t billOfMaterialsIdentifier;
	bstr_t width, depth, height;
	bstr_t sizeunits;
	bstr_t weight;
	bstr_t weightunits;
	int goodparts;
	int badparts;
	int seenparts;

	//Distribution arrivals;
	//std::vector<PropertyElement> simpleproperties;
	BEGIN_METADATA(Part);

};

struct Process : public IObject
{
	Process()
	{
		identifier=StdStringFormat("Process%d",GetNObject()).c_str();
		machineProgramInformation=description=type=description=L"None"; 
		scrapPercentage=partsProducedId=partsProducedQuantity=partsConsumedId=partsConsumedQuantity=L"None";
		repetitionCount=L"1";
	}
	bool IsEmpty() { return identifier == _bstr_t(L"None"); }

	bstr_t identifier;
	std::vector<bstr_t> cellsRequired;
	Time setupTime;
	Time operationTime;
	bstr_t scrapPercentage;
	bstr_t partsProducedId;
	bstr_t partsProducedQuantity;
	bstr_t partsConsumedId;
	bstr_t partsConsumedQuantity;
	bstr_t type;
	bstr_t description;
	bstr_t repetitionCount;
	bstr_t machineProgramInformation;

	BEGIN_METADATA(Process);
};

struct ProcessPlan : public IObject
{
	ProcessPlan()
	{
		identifier=StdStringFormat("ProcessPlan%d",GetNObject()).c_str();
		firstProcessId=L"None";
		processSequenceName=L"None";
	}

	bool IsEmpty() { return identifier == _bstr_t(L"None"); }
	bool IsSequence() { return steps.size() > 0; }
	bstr_t identifier;
	bstr_t firstProcessId;
	std::vector<bstr_t> partsProduced;
	std::vector<bstr_t> partsConsumed;

	std::vector<bstr_t> processIds;
	std::vector<ProcessPtr> processes;

	std::vector<bstr_t> steps;
	//bstr_t repetitionCount;
	bstr_t processSequenceName;

	// Summary statistics
	std::vector<double> clockedsteps;
	double totalTime;
//	boost::posix_time::ptime start, stop;
//	boost::posix_time::ptime stepstart, stepstop;


	// IN-PRCESS MONITORING
	int currentStep;
	bstr_t jobId;
	bstr_t partId;
	bstr_t _currentState;
	Part * _pParentPart;
	Resource * _pCurrentResource;
	std::vector<void *> _pCellList;

	bstr_t PartProgram(int n=-1) 
	{ 
		bstr_t prg;
		if( n<0) n=currentStep;
		if(n<0 || n >= steps.size()) return "";
		try { 
			Process* process = FindProcess(steps[currentStep]);
			prg = process->machineProgramInformation;
			//prg= process->GetPropertyValue("PartProgram").c_str();
		} catch(...) {}
		return prg;
	}
	Process * ProcessPlan::FindProcess(bstr_t name)
	{
		Process* process;
		for(int i=0; i< this->processes.size(); i++)
		{
			process=(Process *) processes[i];//.get();
			if(process->identifier == name)
				return process;
		}
	return process;
}

	BEGIN_METADATA(ProcessPlan);
};

struct Job : public IObject
{
	Job()
	{
		identifier=StdStringFormat("Job%d",GetNObject()).c_str();
		name=L"None"; 

	}
	bool IsEmpty() { return name == _bstr_t(L"None"); }
	bstr_t name;
	bstr_t identifier;
	std::vector<bstr_t> partIds;
	std::vector<bstr_t> partQuantity;
	BEGIN_METADATA(Job);

};


//#include <sql.h>
//#include <sqltypes.h>
//#include <sqlext.h>

class CFactoryIntegrator 
{
public:
	CFactoryIntegrator(void)		
	{
			resources=(Resource().objectsPtr());
		cells=(Cell().objectsPtr());
		parts=(Part().objectsPtr());
		processplans=(ProcessPlan().objectsPtr());
		workorders=(Job().objectsPtr());
//		distributions=( Distribution().objectsPtr()),
		calendars=( Calendar().objectsPtr());
		layouts=(Layout().objectsPtr());
		costs=(CostAllocation().objectsPtr());
		documentation=(Documentation().objectsPtr());
		partprograms=(PartProgram().objectsPtr());

		//resourcehandlers=(CResourceHandler().objectsPtr());
	}
	~CFactoryIntegrator(void){}

	///////////////////
	std::vector<IObjectPtr> *resources;
	std::vector<IObjectPtr> *cells;
	std::vector<IObjectPtr> *parts;
	std::vector<IObjectPtr> *processplans;
	std::vector<IObjectPtr> *workorders;
	std::vector<IObjectPtr> *distributions;
	std::vector<IObjectPtr> *calendars;
	std::vector<IObjectPtr> *layouts;
	std::vector<IObjectPtr> *costs;
	std::vector<IObjectPtr> *documentation;
	std::vector<IObjectPtr> * partprograms;
	//std::vector<IObjectPtr> * resourcehandlers;

	/*Resource* FindResourceByName(bstr_t name);
	Resource* FindResourceById(bstr_t name);
	ProcessPlan* FindProcessPlanByPart(bstr_t name);
	ProcessPlan* FindProcessPlanById(bstr_t id);
	Layout* FindLayoutByAssociatedResource(bstr_t name);
	static Distribution* FindDistributionById(bstr_t name);
	Cell* FindCellById(bstr_t id);
	Part* FindPartById(bstr_t name);
	CostAllocation * FindCostByName(bstr_t name);
	Job * FindJobById(bstr_t id);
	GetJobResources(partid)
	*/

	Resource* FindResourceByName(bstr_t name){return IObject::FindObjectByVariable<Resource,&Resource::name>(name);  }
	Cell* FindCellById(bstr_t id){return IObject::FindObjectByVariable<Cell,&Cell::identifier>(id);  }
	Resource* FindResourceById(bstr_t id){return IObject::FindObjectByVariable<Resource,&Resource::identifier>(id);  }
	Part* FindPartById(bstr_t id){return IObject::FindObjectByVariable<Part,&Part::identifier>(id);  }
	ProcessPlan* FindProcessPlanById(bstr_t id){return IObject::FindObjectByVariable<ProcessPlan,&ProcessPlan::identifier>(id);  }
	CostAllocation* FindCostByName(bstr_t name){return IObject::FindObjectByVariable<CostAllocation,&CostAllocation::name>(name);  }
	Job* FindJobById(bstr_t id){return IObject::FindObjectByVariable<Job,&Job::identifier>(id);  }
	Distribution* FindDistributionById(bstr_t id){return IObject::FindObjectByVariable<Distribution,&Distribution::identifier>( id);  }
	//CResourceHandler* FindResourceHandlerById(bstr_t id){return IObject::FindObjectByVariable<CResourceHandler,&CResourceHandler::_identifier>((LPCSTR) id);  }

	/*Distribution* FindDistributionById(bstr_t id)
	{
		Distribution * dist = IObject::Create<Distribution>() ;
		for(int i=0; i< dist->objects().size(); i++)
		{
			Distribution* distribution ( (Distribution *) dist->objects()[i]);
			if(distribution->identifier==id)
				return distribution;

		}
		return NULL;
	}*/


	Layout* FindLayoutByAssociatedResource(bstr_t name){return IObject::FindObjectByVariable<Layout,&Layout::associatedResource>(name);  }

	ProcessPlan * FindProcessPlanByPart(bstr_t name)
	{
		for(int i=0; i< processplans->size(); i++)
		{
			ProcessPlan *processplan((ProcessPlan *) processplans->at(i));
			std::vector<bstr_t>::iterator it;
			it = std::find(processplan->partsProduced.begin(),processplan->partsProduced.end(), name);
			if(it!=processplan->partsProduced.end())
				return (ProcessPlan *) processplan;

		}
		return NULL;
	}
};

__declspec(selectany) CFactoryIntegrator Factory;

IMPLEMENT_METADATA(Part);
IMPLEMENT_METADATA(Resource);
IMPLEMENT_METADATA(ProcessPlan);
IMPLEMENT_METADATA(Job);
IMPLEMENT_METADATA(Schedule);
IMPLEMENT_METADATA(Shift);
IMPLEMENT_METADATA(Calendar);
IMPLEMENT_METADATA(Cell);
IMPLEMENT_METADATA(Process);
IMPLEMENT_METADATA(Layout);
IMPLEMENT_METADATA(Distribution);
IMPLEMENT_METADATA(CostAllocation);
IMPLEMENT_METADATA(Documentation);
IMPLEMENT_METADATA(PartProgram);
