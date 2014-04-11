//
//
//
#include "stdafx.h"
#define BOOST_ALL_NO_LIB


#include "CmdSim.h"
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>

#include "Device.hpp"
#include "Agent.hpp"


#include "MTConnectStreamsParser.h"

#include "FactoryIntegrator.h"
#include "CmdCell.h"
#include "Config.h"
#include "StateModel.h"
#include "Reporting.h"
#include "HtmlTable.h"


CCmdSim::CCmdSim(AgentConfigurationEx * mtcagent, // mtconnect agent
	std::vector<std::string> devices
	) : AdapterT(mtcagent,  "127.0.0.1", ""), _devices(devices)
{
	cmdnum=0;
	nHeartbeat=0;
	jobId=0;
}


CCmdSim::~CCmdSim(void)
{
}

void CCmdSim::SetMTCTagValue(std::string device, std::string tag, std::string value)
{
	Agent * agent = _agentconfig->getAgent();
	Device *dev = agent->getDeviceByName(device);
	DataItem *di = dev->getDeviceDataItem(tag);
	if (di != NULL)
	{
		std::string time = getCurrentTime(GMT_UV_SEC);
		agent->addToBuffer(di, value, time);
	}

}
/*
r1=CNC1_RESOURCE
r2=CNC2_RESOURCE
r3new=CNC3New_RESOURCE
r3old=CNC3Old_RESOURCE
r4=CNC4_RESOURCE

* Setup up factory components (workorders, parts, process plans, processes, cells, resources via SetupFactory(); which remove CMSD or other
* xml mechanism to read configuration.
* Initialize all workorders to be done, by creating N workorders where N=n1+n2+..+nm is the total number of workorders. These workorders are then randomized
* to be executed, although this might not be the most efficient manner, as once a resources (usually a machine tool) has a setup, it 
* takes time and money to change the setup. (THis is true of all manufacturing processes, with some processes exhibiting a predominant
* cost in the setup.
* Next, the queue size for the active workorders (from the all workorders) is set. For the arena example it is set to 6 (5 resources + 1) to illustrate
* that workorders will not be running until a resource is idel and ready for a workorder, and  the next workorder matches this case. This pairing of workorders 
* to resources is not the most efficient assignment of workorders to resources, however, in an actual shop you cannot be micromanaging the assignment
* of resources as planning is not trivial, and once set up needs to be followed for best results.

* Because the MTConnect interface to the factory resources is a late binding (as the communication  could be some other mechanism such as procy agent)
* the  command agent and statusagent (status from the actual resource)  needs to assigned, so that resources command interface can talk with the 
* actual resources. This implementation mechanism has been customized to accomodate MTConnect via mailbox, so that a command is issued
* with a incremented command number and is asscpted by the "actual" resource only when it is done with all workorders (and programs) and is the
* in the ready state.
*/
void CCmdSim::Configure()
{
	NIST::Config config;

	workorders = new CWorkOrders( );
	SetupFactory();

	workorders->InitAllJobs((Job *)  Factory.workorders->at(0));// there may be multiple workorders - but only 1st counts in our world
	OutputDebugString(workorders->JobsToString().c_str());

	//CCellHandler(NULL, &factory).CreateFactoryResourceHandlers(); 

	std::string cfgfile = Globals.inifile; 
	std::string sLastLogDate;
	try {
		if(GetFileAttributesA(cfgfile.c_str())== INVALID_FILE_ATTRIBUTES)
			throw std::exception("Could not find ini file \n");
		config.load( cfgfile );

		Globals.devices = config.getmap("RESOURCES") ;
		CResourceHandler::CreateFactoryResourceHandlers(_agentconfig); 
		std::cout<< CResourceHandlers::GenerateReport()<< std::endl;


	}
	catch(...)
	{
		OutputDebugString("Exception CCmdSim::Configure()\n");
	}

}
void CCmdSim::MTConnectKpiReport()
{
	SetMTCTagValue("KPI", "heartbeat", StdStringFormat("%d", nHeartbeat++ ));
	std::string tmp;
	BOOST_FOREACH (std::string s, workorders->parts)  
		tmp += StdStringFormat("%d,",	workorders->finishedparts[s]);

	SetMTCTagValue("KPI","AllFinished", tmp);
	SetMTCTagValue("KPI","TotalBusyTime", StdStringFormat("%8.2f", ControlThread::GetAllMachineTiming("running")));
	SetMTCTagValue("KPI","TotalFaultTime", StdStringFormat("%8.2f", ControlThread::GetAllMachineTiming("faulted")));
	SetMTCTagValue("KPI","TotalIdleTime", StdStringFormat("%8.2f", ControlThread::GetAllMachineTiming("ready")));
	SetMTCTagValue("KPI","ShiftStart", NIST::Time.GetTimeStamp(NIST::Shift.GetCurrentShiftStart()));
	SetMTCTagValue("KPI","ShiftEnd", NIST::Time.GetTimeStamp(NIST::Shift.GetCurrentShiftEnd()));
	SetMTCTagValue("KPI","ShiftNow", NIST::Time.GetTimeStamp());
	SetMTCTagValue("KPI","factoryTime", workorders->serviceTime.ElapsedString());

	

	SetMTCTagValue("KPI","QueuedPartids", StringFromVectorVar<CWorkOrder , &CWorkOrder::_partid>(*workorders));
	tmp.clear();
	BOOST_FOREACH (CWorkOrder * wo, *workorders)  
		if(wo->CurrentStep()>=0)
			tmp += StdStringFormat("%s(%d[%s]),",	wo->CurrentPartId().c_str(),wo->CurrentStep(), (LPCSTR) wo->CurrentResourceAssigned()->_resource->identifier );

	SetMTCTagValue("KPI","ActivePartids", tmp);
	//////////////////////////////////////////////////////
	int nGood=0,nBad=0;
	BOOST_FOREACH (CResourceHandler * rh, CResourceHandler::_rhandlers)  
	{	
		nGood+=rh->_statemachine->_nLastPartCountGood; 
		nBad+=rh->_statemachine->_nLastPartCountBad; 
	}
	if(nGood+nBad>0) 	
		SetMTCTagValue("KPI","Yield", StdStringFormat("%d:%6.2f%%", (nGood+nBad), 100.0 * ((double) nGood / ((double)nGood+nBad) )));
	else
		SetMTCTagValue("KPI","Yield", "0%" );
	//////////////////////////////////////////////////////
	std::string maxResourceUtilization;
	double nMax=0;
	BOOST_FOREACH (CResourceHandler * rh, CResourceHandler::_rhandlers)  
	{
		if(rh->_statemachine->Utilization() > nMax)
		{
			maxResourceUtilization=rh->_resource->identifier;
			nMax=rh->_statemachine->Utilization();
		}
	}
	
	if(nMax>0) 	
		SetMTCTagValue("KPI","Bottleneck", StdStringFormat("%s at %6.2f%% utilization", maxResourceUtilization.c_str(), 100.0 * nMax ));

	//////////////////////////////////////////////////////
	tmp.clear();
	BOOST_FOREACH (std::string s, workorders->parts)  
	{
		double throughput=0;
		BOOST_FOREACH (CResourceHandler * rh, CResourceHandler::_rhandlers)  
			throughput+=rh->_statemachine->GetPartTiming(s);
		tmp+=StdStringFormat("%s,", NIST::Time.HrMinSecFormat((int) throughput).c_str());
	}
	SetMTCTagValue("KPI","Throughput", tmp);


}

/**
 The command simulator cycle method implements the interface to the factory. First, COM must be initialized, as the 
 * MSXML COM component is used to parse status from teh "actual" resource agents. 

 * Next, the comamnd simulator configuration is done, which is described in that method.

 * Next, the command "RESET" is sent to all resources so that they can reset command and command numbers and be
 * ready to accept comands, incremented command numbers, and associated programs. After the "RESET" is issued to each
 * resource, the command simulator waits for a "READY" acknowlegement from each rewsource, thus indicating that the resource
 * is ready for new commands. 

 * It is the responsibility of the Command simulator cycle to update all the resources (thus allowing their state machine to run).
 * Thus, the issued reset causes a Resource to invoke Reset() and then move into the state "resetting", while it awaits acknoledgement
 * from its "actual" resource. Once the actual resource is done, the commanded resource determines that the "actual" resource
 * has finished by issuing a READY in the execution state, and the commanded resource state machine is now  likewise READY.
 */
void CCmdSim::Cycle()
{
	int nHeartbeat=0;
	CoInitialize(NULL);

	Configure();
	ControlThread::_dSpeedup=60.0;


	//	double ControlThread::UpdateRateSec() { return _updaterate / 1000.0 * _dSpeedup; }
	// void ControlThread::SetUpdateRate(unsigned int milliseconds) {if(milliseconds>0) UpdateRate()=milliseconds; }


	std::string nMaxQueueSize = Factory.workorders->at(0)->GetPropertyValue("MaxQueueSize");
	CWorkOrders::MaxQueueSize=ConvertString<int>(nMaxQueueSize,1);
	//CWorkOrders::MaxQueueSize=1;
	//CResourceHandlers::SendVarToHandlers<Queue<CWorkOrder>, &Queue<CWorkOrder>::nMaxSize> (CWorkOrders::MaxQueueSize);
	CResourceHandlers::SendVarToHandlers<Queue<CWorkOrder>, &Queue<CWorkOrder>::nMaxSize> (5);



	CResourceHandler::SendAllResourceHandlers("reset");
	while(!CResourceHandlers::AllResourcesInState("ready")) 
	{ 
		CResourceHandlers::UpdateResourceHandlers();
		OutputDebugString(CResourceHandlers::ToString().c_str());
		::Sleep(_nServerRate); 
	}

	workorders->serviceTime.Start();
	_mRunning=true;

	//std::cout<< CResourceHandlers::ToStateTable()<< std::endl;
	Job * job = (Job *)  Factory.workorders->at(0);

	SetMTCTagValue("KPI","AllParts", StringFromVector(workorders->parts));
	SetMTCTagValue("KPI","AllQuantities", StringFromVector(job->partQuantity));
	SetMTCTagValue("KPI","avail", "AVAILABLE");
	
	while(_mRunning)
	{
		try {
			
			ControlThread::RestartTimer();

			//OutputDebugString(workorders->ToString().c_str());
			//OutputDebugString(CResourceHandlers::ToString().c_str());
			TIME ( workorders->Newworkorder());              // done with workorder - start new work order

			//	OutputDebugString(workorders->Dump().c_str());
			TIME (workorders->Update()) ;					// update workorder queues 

			//Reporting::AgentStatus(this, JobStatus, Jobs, DeviceStatus );
			FactoryHandlers.UpdateResourceHandlers();  // update state machine

			if(workorders->AllFinished())
				break;

			MTConnectKpiReport();
			::Sleep(_nServerRate);
		}
		catch(std::exception e)
		{
			OutputDebugString(e.what());
		}
		catch(...)
		{
			OutputDebugString("Cycle unknown exception\n");

		}
		// Actual time to service all workorders

	}
	workorders->serviceTime.Stop();
	MTConnectKpiReport();

	//Reporting::GenerateHtmlReport(workorders,File.ExeDirectory()+"Report.html");
	std::string html = CHtmlTable::CreateHtmlSytlesheetFrontEnd("Example 6");
	html+=	Reporting().CreateHtmlFactoryUseTable(1, 8.0, 100.00, workorders->serviceTime.Elapsed());
	html+="<BR>\n";
	html+=Reporting().CreateHtmlJobTable(workorders);
	html+="<BR>\n";
	html+=Reporting().CreateHtmlResourceStatesTable();
	html+="<BR>\n";
	html+=Reporting().CreateHtmlResourceKPITable();

	html+="</body></html>\n";
	WriteFile(File.ExeDirectory()+"JobReport.html", html);
	////////////////////////////////////////////////////
	
	html=CHtmlTable::CreateHtmlSytlesheetFrontEnd("Example Ch 6 Arena");
	html += Raphael::HtmlRaphaeleChart( );
	html += "</HEAD>\n";
	html+=Reporting().GeneratePieChart();
	html+="</body></html>\n";
	WriteFile(File.ExeDirectory()+"PieChart.html", html);
	////////////////////////////////////////////////////
	html=CHtmlTable::CreateHtmlSytlesheetFrontEnd("Example Ch 6 Arena");
	html+=Reporting().GenerateHtmlHistory();
	html+=Reporting().GenerateSetupUsage();
	html+="</body></html>\n";
	WriteFile(File.ExeDirectory()+"History.html", html);

}

#if 0
	virtual void		Cycle1();
	void				GetMTCTagValue(std::string device, std::string tag);
	bool				Parse(std::vector<std::string> tokens);
	void				SetMTCTagValue(std::string device, std::string tag, std::string value);
	void				UpdateAllDevicesTag(std::string tag, std::string value);
	void				UpdateAllDevicesStatus();
void CCmdSim::GetMTCTagValue(std::string device, std::string tag)
{
	Agent * agent = _agentconfig->getAgent();
	Device *dev = agent->getDeviceByName(device);
	DataItem *di = dev->getDeviceDataItem(tag);
	if (di != NULL)
	{

	}

}

void CCmdSim::SetMTCTagValue(std::string device, std::string tag, std::string value)
{
	Agent * agent = _agentconfig->getAgent();
	Device *dev = agent->getDeviceByName(device);
	DataItem *di = dev->getDeviceDataItem(tag);
	if (di != NULL)
	{
		std::string time = getCurrentTime(GMT_UV_SEC);
		agent->addToBuffer(di, value, time);
	}

}
void CCmdSim::UpdateAllDevicesTag(std::string tag, std::string value)
{
	for(int i=0; i< _devices.size(); i++)
	{
		SetMTCTagValue(_devices[i],tag, value);
	}
}
void CCmdSim::UpdateAllDevicesStatus()
{
	std::vector<DataDictionary>  cycledatum ;
	for(int i=0; i< _devices.size(); i++)
	{
		// Read Command Agent values
		cycledatum = _parser.ReadStream("127.0.0.1:5000/"+_devices[i]);
		if(cycledatum.size()<1)
			continue;
		DataDictionary data=cycledatum[0];
		for(DataDictionary::iterator it = data.begin(); it!=data.end(); it++)
		{
			items.SetTag(_devices[i] + "." + (*it).first, (*it).second);
		}

	}
}
bool CCmdSim::Parse(std::vector<std::string> tokens)
{
	if(tokens.size() > 3  && stricmp(tokens[0].c_str(),"UPDATE")==0)
	{
		//UPDATE DEVICE TAG VALUE
		SetMTCTagValue(tokens[1],tokens[2],tokens[3]);
		return true;
	}
	if(tokens.size() > 1  && stricmp(tokens[0].c_str(),"SEND")==0)
	{
		//SEND DEVICE
		SetMTCTagValue(tokens[1],"cmdnum", StdStringFormat("%d", ++cmdnum));
	}
	if(tokens.size() > 1  && stricmp(tokens[0].c_str(),"WAIT")==0)
	{
		// WAIT seconds
		int nSeconds = ConvertString<int>(tokens[1], 1) * 1000;
		::Sleep(nSeconds);
	}
	if(tokens.size() > 0  && stricmp(tokens[0].c_str(),"RESETALL")==0)
	{
		std::string sCmdnum= StdStringFormat("%d", ++cmdnum);
		UpdateAllDevicesTag("command", "RESET");
		UpdateAllDevicesTag("cmdnum", sCmdnum);
		// wait all cmdnum_echo is cmdnum
	}
	if(tokens.size() > 3  && stricmp(tokens[0].c_str(),"WAITUNTIL")==0)
	{
		if(stricmp(items.GetTag(tokens[1]+"."+tokens[2] , "").c_str() , tokens[3].c_str()) != 0 )
		{
			return false;
		}
	}

	return true;
}
void CCmdSim::Cycle1()
{
	int nHeartbeat=0;
	CoInitialize(NULL);

	Configure();

	_mRunning=true;
	UpdateAllDevicesTag("cmdnum", StdStringFormat("%d", 0));


	std::string line;
	getline(std::cin, line) ;
	while(_mRunning)
	{
		try {

			UpdateAllDevicesTag("heartbeat", StdStringFormat("%d", ++nHeartbeat));
			// Read in every device status
			UpdateAllDevicesStatus();

			// read in every line of stdin   
			std::vector<std::string> tokens = TrimmedTokenize(line," ");

			if(tokens.size() > 0  && stricmp(tokens[0].c_str(),"READ")==0)
			{
				std::string line;
				std::ifstream fd (tokens[1]);
				bool flag = getline(fd, line);
				while(flag )
				{
loop:
					tokens = TrimmedTokenize(line," ");
					UpdateAllDevicesStatus();
					UpdateAllDevicesTag("heartbeat", StdStringFormat("%d", ++nHeartbeat));
					while(!Parse(tokens)) 
					{
						// If false, do no advance file reading, redo last line
						::Sleep(_nServerRate);
						goto loop;
					}
					flag = getline(fd, line);
				}

			}
			else
			{
				Parse(tokens);
			}
			::Sleep(_nServerRate);
			getline(std::cin, line) ;
		}
		catch(std::exception e)
		{

		}
		catch(...)
		{

		}
	}
}

#endif