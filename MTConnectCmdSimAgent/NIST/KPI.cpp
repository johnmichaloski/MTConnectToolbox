#include "StdAfx.h"
#include "KPI.h"
#include "HtmlTable.h"

KPI::KPI(Stats stats)
{
	Init(stats);
}

void KPI::Init(Stats & stats1) 
{
		
	nTotalParts=stats1["TotalParts"];
	nGoodParts=stats1["GoodParts"];
	dTotalTime=stats1["TotalTime"];
	nIdleTime=stats1["idle"];
	TPT= dTotalTime;

	this->nBlockedTime=stats1.nBlockedTime;
	this->nStarvedTime=stats1.nStarvedTime;
	this->nDownTime=stats1.nDownTime;
	this->nProductionTime=stats1.nProductionTime;
	this->nOffTime=stats1.nOffTime;
	this->nRepairTime=stats1.nRepairTime;
	this->nIdleTime=stats1.nIdleTime;

	MTBF=stats1["MTBF"];
	MTTR=stats1["MTTR"];
	MTTP=stats1["MTTP"];
		
}


std::string KPI::ByResources(Stats &stats1)
{
	std::string tmp;
	double POET=stats1["POET"]; //Planned order execution time
	double POT=stats1["POET"];  //Planned operation time
	double PSUT=0;  //Planned unit setup time
	double PBT=POET-0.0; //Planned busy time - planned PM
	double PRI=MTTP; ///Planned run time per unit - FIXME what if > 1 quantity unit
	double APWT=0.0; //Actual personnel work time 
	double AUPT=nProductionTime+0.0; //Actual unit processing time - processing
	double AUBT=nProductionTime; //Actual unit processing time - processing + setup
	double AOET=dTotalTime; //SOFAR - not planned . Actual order execution time time difference between start time and end time of a
				//production order. It includes the actual busy time, the actual transport and the actual queuing time.
	double APAT=0;  //Actual personnel attendance time
	double APT=nProductionTime;  //Actual production time  It includes only the value-adding functions.
	double AQT=nIdleTime; //Actual queuing time - time spent idle on queue
	double  ADOT=nDownTime; // Actual unit down time
	double  ADET=nBlockedTime+ nDownTime;;  //Actual unit delay time
	double ASUT=0;  // Actual unit setup time
	double ATT=0;  // Ac tual transport time
	double TBF= MTBF;  // Time between failures
	double TTR=MTTR;  //Time to repair
	double TTF=MTBF; //Time to failure
	double FE=stats1["FE"];  //Failure event count
	double CMT=MTTR;  //Corrective maintenance time
	double PMT=99;  //Preventive maintenance time
	double  POQ=stats1["OQ"];  // Order quantity
	double SQ=nTotalParts-nGoodParts;   // Scrap quantity
	double PSQ=0;    //  Planned scrap quantity
	double  GQ=nGoodParts;  //Good quantity
	double  RM=99;  // Rework quantity string - type not defined
	double RMI=99;  // Raw materials inventory - string - type not defined, plus amount?
	double FGI=nGoodParts;  //Finished goods inventory
	double CI=99;  //Consumable inventory
	double CM=99;  //Consumed material
	double IGQ=nGoodParts;  //,Integrated good quantity
	double PL=nTotalParts-nGoodParts; //Production loss
	double STL=0;  // Storage and transportation loss 
	double OL=0;  // Other loss
	double  EPC= MTTP*24*60*60; //Equipment production capacity 24 hours MTTP
	double GP=nGoodParts;  // AGAIN?Good part
	double  IP=0; //Inspected part
	double PQ=nGoodParts;  //Produced quantity shall be the quantity that a work unit has produced in relation to a production order.
	double  QR=GQ / PQ;
	double RQ=0.0; // Rework quantity;

	double Effectiveness=PRI * PQ / APT;
	double Availability =APT / PBT;
	double SetupRatio = ASUT / AUPT;
	double TechnicalEfficiency = APT / (APT + ADET);
	double FPY = GP/IP;
	double ScrapRatio= SQ / PQ;
	double UtilizationEfficiency =APT / AUBT;
	double AllocationEfficiency  =  AUBT / PBT;
	double ThroughputRate = PQ/AOET;
	double WorkerEfficiency = APWT / APAT;


	double NEE= AUPT / PBT * Effectiveness * QR;
	double OEE= Availability * Effectiveness * QR;



	//Planned times
	tmp+="#"+StdStringFormat("<TR><TD colspan=\"%d\"> <CENTER><BOLD> Planned times</BOLD></CENTER></TD></TR>\n" , 5);
	tmp+= StdStringFormat("%8.2f",  POET )+ "@POET@ Planned order execution time@@ ";
	tmp+= "The planned order execution time shall be the planned time for executing an order.";
	tmp+= "NOTE: It is often calculated from the planned run time per item multiplied by the order quantity plus";
	tmp+= "the planned setup time.\n";

	tmp+= StdStringFormat("%8.2f",  POT )+ "@POT@Planned operation time@@ ";
	tmp+= "The planned operation time shall be the planned time in which a work unit can be used. The operation";
	tmp+= "time is a scheduled time.\n";

	tmp+= StdStringFormat("%8.2f",  PSUT )+ "@PSUT@Planned unit setup time@@ ";
	tmp+= "The planned unit setup time shall be the planned time for the setup of a work unit for an order.\n";
	
	tmp+= StdStringFormat("%8.2f",  PBT )+ "@PBT@Planned busy time@@ ";
	tmp+= "The planned busy time shall be the operating time minus the planned downtime.";
	tmp+= "NOTE: The planned down time may be used for planned maintenance work. The planned busy period";
	tmp+= "is available for the detailed planning of the work unit for expected production orders.\n";
	
	tmp+=  StdStringFormat("%8.2f",  PRI )+"@PRI@Planned run time per unit@@";
	tmp+= "The run time per item shall the planned time for producing one quantity unit.\n";
	
	// Actual times
	tmp+="#"+StdStringFormat("<TR><TD colspan=\"%d\"> <CENTER><BOLD> Actual times</BOLD></CENTER></TD></TR>\n" , 5);
	tmp+= StdStringFormat("%8.2f",  APWT )+ "@APWT@ Actual personnel work time @@";
	tmp+= "The actual personnel work time shall be the time that a worker needs for the execution of a production";
	tmp+= "order.\n";

	tmp+= StdStringFormat("%8.2f",  AUPT )+ "@AUPT@ Actual unit processing time@@";
	tmp+= "The actual unit processing time shall be the time needed for setup and for the production\n";

	tmp+= StdStringFormat("%8.2f",  AUBT )+ "@AUBT@Actual unit busy time@@";
	tmp+= "The actual unit busy time shall be the actual time that a work unit is used for the execution of a";
	tmp+= "production order.\n";

	tmp+= StdStringFormat("%8.2f",  AOET )+ "@AOET@ Actual order execution time@@";
	tmp+= "The actual order execution time shall be the time difference between start time and end time of a";
	tmp+= "production order. It includes the actual busy time, the actual transport and the actual queuing time.\n";

	tmp+= StdStringFormat("%8.2f",  APAT )+ "@APAT@ Actual personnel attendance time @@";
	tmp+= "The actual personnel attendance time shall be the actual time that a worker is available to work on";
	tmp+= "production orders. It does not include actual time for company authorized break periods (i.e. lunch). It";
	tmp+= "shall be the difference between login and logout excluding breaks.\n";

	tmp+= StdStringFormat("%8.2f",  APT )+ "@APT@Actual production time @@";
	tmp+= "The actual production time shall be the actual time during which a work unit is producing. It includes";
	tmp+= "only the value-adding functions.\n";

	tmp+= StdStringFormat("%8.2f",  AQT )+ "@AQT@ Actual queuing time@@ ";
	tmp+= "The actual queuing time shall be the actual time in which the material is either in transport or";
	tmp+= "progressing through a manufacturing process, i.e., the material is waiting for the process to begin.\n";

	tmp+= StdStringFormat("%8.2f",  ADOT )+ "@ADOT@Actual unit down time @@ ";
	tmp+= "The actual unit down time shall be the actual time when the work unit is not executing order production ";
	tmp+= "although it is available.\n";

	tmp+= StdStringFormat("%8.2f",  ADET )+ "@ADET@Actual unit delay time @@ ";
	tmp+= "The actual unit delay time shall be the actual time associated with malfunction-caused interrupts, ";
	tmp+= "minor stoppages, and other unplanned time intervals that occur while tasks are being completed that ";
	tmp+= "lead to unwanted extension of the order processing time\n";

	tmp+= StdStringFormat("%8.2f",  ASUT )+ "@ASUT@Actual unit setup time @@ ";
	tmp+= "The actual unit setup time shall be the time consumed for the preparation of an order at a work unit.\n";

	tmp+= StdStringFormat("%8.2f",  ATT )+ "@ATT@Actual transport time @@";
	tmp+= "The actual transport time shall be the actual time required for transport between work units.\n";

	tmp+= StdStringFormat("%8.2f",  AUPT )+ "@AUPT@Actual unit processing time @@ ";
	tmp+= "The actual unit processing time shall be the actual production time plus the actual unit setup time.\n ";

	tmp+= StdStringFormat("%8.2f",  AUBT )+ "@AUBT@Actual unit busy time @@ ";
	tmp+= "The actual unit busy time shall be the actual unit processing time plus the actual unit delay time. \n";

	tmp+= StdStringFormat("%8.2f",  AOET )+ "@AOET@Actual order execution time @@ ";
	tmp+= "The actual order execution time shall be the time from the start of the order until the time of the ";
	tmp+= "completion of the order. \n";

	// Maintenance times
	tmp+="#"+StdStringFormat("<TR><TD colspan=\"%d\"> <CENTER><BOLD> Maintenance elements</BOLD></CENTER></TD></TR>\n" , 5);
	tmp+= StdStringFormat("%8.2f",  TBF )+ "@TBF@Time between failures@@ ";
	tmp+= "The time between failures shall be the actual unit busy time (AUBT) between two consecutive failures ";
	tmp+= "of a work unit including setup time and production time related to the orders being processed and ";
	tmp+= "without delay times. \n";

	tmp+= StdStringFormat("%8.2f",  TTR )+ "@TTR@Time to repair @@ ";
	tmp+= "Time to repair shall be the actual time during which a work unit is unavailable due to a failure. \n";

	tmp+= StdStringFormat("%8.2f",  TTF )+ "@TTF@Time to failure @@ ";
	tmp+= "The time to failure shall be the time between failures minus the time to repair. \n";

	tmp+= StdStringFormat("%8.2f",  FE )+ "@FE@Failure event count @@ ";
	tmp+= "The Failure event count shall be the count over a specified time interval of the terminations of the ";
	tmp+= "ability for a work unit to perform a required operation. \n";

	tmp+= StdStringFormat("%8.2f",  CMT )+ "@CMT@Corrective maintenance time @@";
	tmp+= "The corrective maintenance time shall be the part of the maintenance time, during which corrective ";
	tmp+= "maintenance is performed on a work unit, including technical delays and logistic delays inherent in ";
	tmp+= "corrective maintenance (IEC 60050-191). \n";

	tmp+= StdStringFormat("%8.2f",  PMT )+ "@PMT@Preventive maintenance time @@ ";
	tmp+= "The preventive maintenance time shall be that part of the maintenance time during which preventive ";
	tmp+= "maintenance is performed on a work unit, including technical delays and logistic delays inherent in ";
	tmp+= "preventive maintenance (IEC 60050-191). \n";
	
	// Logistical elements
	tmp+="#"+StdStringFormat("<TR><TD colspan=\"%d\"> <CENTER><BOLD> Logistical elements</BOLD></CENTER></TD></TR>\n" , 5);
	tmp+= StdStringFormat("%8.2f",  POQ )+ "@POQ@Order quantity @@ ";
	tmp+= "The planned order quantity shall be the planned quantity of products for a production order (lot size,";
	tmp+= "production order quantity).\n";
	
	tmp+= StdStringFormat("%8.2f",  SQ )+ "@SQ@ Scrap quantity@@";
	tmp+= "The scrap quantity shall be the produced quantity that did not meet quality requirements and either";
	tmp+= "has to be scrapped or recycled.\n";
	
	tmp+= StdStringFormat("%8.2f",  PSQ )+ "@PSQ@ Planned scrap quantity @@";
	tmp+= "The planned scrap quantity shall be the amount of process-related scrap that is expected when";
	tmp+= "manufacturing the product (e.g., at the start or ramp-up phases of the manufacturing systems).\n";
	
	tmp+= StdStringFormat("%8.2f",  GQ )+ "@GQ@ Good quantity @@";
	tmp+= "The good quantity shall be the produced quantity that meets quality requirements.\n";

	tmp+=  StdStringFormat("%8.2f",  RQ )+ "@RQ@ Rework quantity @@";
	tmp+= "The rework quantity shall be the quantity that will be failed to meet the quality requirements but where";
	tmp+= "these requirements can be met by subsequent work.\n";
	
	tmp+=  StdStringFormat("%8.2f",  PQ )+ "@PQ@ Produced quantity @@";
	tmp+= "The produced quantity shall be the quantity that a work unit has produced in relation to a production";
	tmp+= "order.\n";

	tmp+= StdStringFormat("%8.2f",  RM )+ "@RM@ Raw materials @@";
	tmp+= "The materials that are changed into finished goods through the production.\n";

	tmp+= StdStringFormat("%8.2f",  RMI )+ "@RMI@ Raw materials inventory @@ ";
	tmp+= "The raw materials inventory shall be the inventory of materials that are changed into intermediates or";
	tmp+= "finished goods through production.\n";

	tmp+= StdStringFormat("%8.2f",  FGI )+ "@FGI@ Finished goods inventory@@ ";
	tmp+= "The finished goods inventory shall be the amount of acceptable quantity which can be delivered.\n";

	tmp+= StdStringFormat("%8.2f",  CI )+ "@CI@ Consumable inventory@@";
	tmp+= "These are materials which are transformed in quantity or quality during the production process and no";
	tmp+= "longer available for use in production operations.\n";

	tmp+= StdStringFormat("%8.2f",  CM )+"@CM@ Consumed material@@ ";
	tmp+= "The consumed material shall be the summed quantity of materials consumed by a process.";
	tmp+= "NOTE:";
	tmp+= "In the process industry (e.g., oil refining and chemicals), consumed material is usually used in the";
	tmp+= "denominator to calculate the related KPIs. In some industrial processes, input can be less than the";
	tmp+= "output. Many chemical and physical changes occur during production, and product yield has";
	tmp+= "fluctuation and uncertainty. It is therefore difficult to calculate and measure the output.\n";

	tmp+= StdStringFormat("%8.2f",  IGQ )+ "@IGQ@Integrated good quantity@@";
	tmp+= "The integrated good quantity shall be the summed product count or quantity resulting from a multiproduct";
	tmp+= "production process used in KPI calculations instead of GQ.";
	tmp+= "NOTE Since IGQ represents the quantity of all products during production, all products need to be";
	tmp+= "measured in the same unit of measure, or be converted to the same unit of measure. A list of";
	tmp+= "conversion coefficients can be used to unify the measurement modes of different products.\n";
	
	tmp+= StdStringFormat("%8.2f",  PL )+ "@PL@Production loss @@";
	tmp+= "The production loss shall be the quantity lost during production, calculated as output minus input.";
	tmp+= "NOTE Used in batch and continuous manufacturing\n";

	tmp+= StdStringFormat("%8.2f",  STL )+ "@STL@Storage and transportation loss @@";
	tmp+= "The storage and transportation loss shall be the quantity lost during storage and transportation, e.g.,";
	tmp+= "inventory lost during an inventory calculation or material lost during movement from one place to";
	tmp+= "another.";
	tmp+= "NOTE Used in batch and continuous manufacturing\n";

	tmp+= StdStringFormat("%8.2f",  OL )+ "@OL@ Other loss @@";
	tmp+= "Other loss shall be the quantity lost due to extraordinary incidents, e.g., natural disasters.";
	tmp+= "NOTE Used in batch and continuous manufacturing\n";
	
	tmp+= StdStringFormat("%8.2f",  EPC )+ "@EPC@ Equipment production capacity @@";
	tmp+= "EPC is the maximum production quantity of production equipment.";
	tmp+= "NOTE:";
	tmp+= "Used in batch and continuous manufacturing\n";

	//Quality elements
	tmp+="#"+StdStringFormat("<TR><TD colspan=\"%d\"> <CENTER><BOLD> Quality elements</BOLD></CENTER></TD></TR>\n" , 5);
	tmp+= StdStringFormat("%8.2f",  GP )+"@GP@Good part @@";
	tmp+= "A good part shall be the count of individual identifiable parts, e.g., by serialization, which meets the";
	tmp+= "quality requirements.";
	tmp+= "NOTE: In discrete manufacturing a part is typically a single produced item. In batch manufacturing a";
	tmp+= "party refers to a defined material lot\n";

	tmp+= StdStringFormat("%8.2f",  IP )+"@IP@Inspected part@@";
	tmp+= "An inspected part shall be the count of individual identifiable parts, e.g., by serialization, which was";
	tmp+= "tested against the quality requirements.";
	tmp+= "NOTE: In discrete manufacturing a part is typically a single produced item. In batch manufacturing a";
	tmp+= "party refers to a defined material lot.\n";


	tmp+= StdStringFormat("%8.2f",  APWT / APAT )+ "@@ Worker Efficiency @ APWT / APAT@";
	tmp+= "The worker efficiency considers the relationship between the Actual";
	tmp+= "personnel work time (APWT) related to production orders and the";
	tmp+= "actual personnel attendance time (APAT) of the employee.\n";

	tmp+= StdStringFormat("%8.2f", AUBT/AOET )+ "@@Allocation Ratio @  AUBT/AOET@";
	tmp+= "AUBT = sum of the AUBT of all work units invovled in a ";
	tmp+= "production order, where AUBT is the Actual unit busy time";
	tmp+= "The allocation ratio is the relationship of the complete actual busy";
	tmp+= "time over all work units (AUBT) involved in a production order to the";
	tmp+= "actual order execution time of a production order (AOET).\n";

	tmp+=StdStringFormat("%8.2f",  PQ/AOET )+  "@@Throughput rate @ PQ/AOET@";
	tmp+= "Process performance in terms of produced quantity of an order";
	tmp+= "(PQ) and the actual execution time of an order (AOET).\n";

	tmp+= StdStringFormat("%8.2f",  AUBT / PBT )+ "@@Allocation Efficiency @  AUBT / PBT@";
	tmp+= "The allocation efficiency is the ratio between the actual allocation";
	tmp+= "time of a work unit expressed as the actual unit busy time (AUBT)";
	tmp+= "and the planned time for allocating the work unit expressed as the";
	tmp+= "planned unit busy time (PBT).\n";

	tmp+= StdStringFormat("%8.2f",  APT / AUBT )+ "@@Utilization Efficiency @ APT / AUBT@";
	tmp+= "The utilization efficiency is the ratio between the actual production";
	tmp+= "time (APT) and the actual unit busy time (AUBT)\n";

	tmp+= StdStringFormat("%8.2f",  OEE )+ "@OEE@OEE Index @ Availability * Effectiveness * Quality ratio@";
	tmp+= "The OEE Index represents the availability of a work unit,";
	tmp+= "the effectiveness of the work unit (see 5.9), and the quality ratio";
	tmp+= "KPI’s integrated in a single indicator.\n";

	tmp+=  StdStringFormat("%8.2f",  APT / PBT )+"@NEE @ Net Equipment Effectiveness @ AUPT / PBT * Effectiveness * Quality ratio@";
	tmp+= "The Net Equipment Effectiveness (NEE) index combines the ratio";
	tmp+= "between actual unit processing time (AUPT) and planned busy time";
	tmp+= "(PBT), the Effectiveness KPI and the Quality Ratio KPI into a single";
	tmp+= "indicator\n";
	
	tmp+= StdStringFormat("%8.2f",  APT / PBT )+"@@Availability @ APT / PBT@";
	tmp+= "Availability is a ratio that shows the relation between the actual";
	tmp+= "production time (APT) and the Planned busy time (PBT) for a work";
	tmp+= "unit.\n";

	tmp+= StdStringFormat("%8.2f", PRI * PQ / APT )+ "@@Effectiveness @ PRI * PQ / APT@";
	tmp+= "It Effectiveness represents the relationship between the planned";
	tmp+= "target cycle and the actual cycle expressed as the planned runtime";
	tmp+= "per item (PRI) multiplied by the produced quantity (PQ) divided by";
	tmp+= "the actual production time (APT).\n";

	tmp+= StdStringFormat("%8.2f", QR )+"@QR@Quality Ratio @ GQ / PQ@";
	tmp+= "The Quality Ratio is the relationship between the good quantity";
	tmp+= "(GQ) and the produced quantity (PQ).\n";

	tmp+= StdStringFormat("%8.2f", ASUT / AUPT )+"@@Setup ratio @ ASUT / AUPT@";
	tmp+= "The Setup ratio is the ratio of Actual Setup Time (ASUT) to Actual";
	tmp+= "Unit Processing Time (AUPT). It defines the percentage time used";
	tmp+= "for setup compared to the actual time used for processing.\n";

	tmp+= StdStringFormat("%8.2f", APT / (APT + ADET) )+"@@Technical efficiency @ APT / (APT + ADET)@";
	tmp+= "The technical efficiency of a work unit is the relationship between";
	tmp+= "the actual production time (APT) and the sum of the actual";
	tmp+= "production time (APT) and the actual unit delay time (ADET) which";
	tmp+= "includes the delays and malfunction-caused interruptions.\n";

	//tmp+= ",,Production process ratio ,APT / AOET,";
	//tmp+= "APT = sum of the APT of all work units and work centres involved";
	//tmp+= "in a production order.";
	//tmp+= "The Production Process Ratio defines the relationship between the";
	//tmp+= "actual production time (APT) over all work units and work centres";
	//tmp+= "involved in a production order and the whole throughput time of a";
	//tmp+= "production order which is the actual order execution time (AOET)\n";

	tmp+=  StdStringFormat("%8.2f", GP / IP)+"@FPY@ First Pass Yield@GP / IP@";
	tmp+= "The FPY designates the percentage of products, which full fill the";
	tmp+= "quality requirements in the first process run without reworks (good";
	tmp+= "parts). It is expressed as the ratio between good parts (GP) and";
	tmp+= "inspected parts (IP).\n";

	tmp+=  StdStringFormat("%8.2f", SQ / PQ)+"@@Scrap Ratio @ SQ / PQ@";
	tmp+= "The scrap ratio is the relationship between scrap quantity (SQ) and";
	tmp+= "produced quantity (PQ).\n";


	return tmp;
}


KPI::~KPI(void)
{
}


void KPI::Compute() // throws error
{
	


}


std::string KPI::AbbrvCSVString()
{
	std::string tmp;


		BZ=dTotalTime; //=nBlockedTime+nStarvedTime+nDownTime+nProductionTime+nOffTime+nRepairTime+nIdleTime;
	PQ = nTotalParts; // produced quantity\n";
	
	// Primitives
	AUPT=APT= nProductionTime;  ///Actual production time 
	ADOT=nDownTime;				//  Actual unit down time	
	PEZ=PTU= nProductionTime; // Production time per unit (PEZ)";

		SU= nBlockedTime + nDownTime; //, Delay time (SU)\n";
	SZ=nDownTime; //Down time Do (SZ)\n";

	PSUT=0.0; //Planned set up time (PRZ)\n";
	PBZ=PBT= AUPT + ADOT; //Planned Busy Time (PBZ) (seems per order)

	ASUT=0.0; // actual setup time
	ADET=nBlockedTime+ nDownTime; // Actual unit delay time
	BT=APT + ADET + ASUT; //Busy time (BLZ)\n";
	GC=GT=GQ=GP=nGoodParts; // Good pieces (GT) good quantity (GM)\n";
	AUP= (APT + ASUT) ; //  or AUP Actual unit processing time = APT + ASUT (NOT Divided by parts?)
	AUBT= AUPT + ADET; //  Actual unit busy time (NOT Divided by parts? unit=resource)
	WT=this->nStarvedTime+this->nBlockedTime+this->nDownTime;  // wait time

	if(nTotalParts>0) 
	{
		AOET= dTotalTime/nTotalParts; //  Actual order execution time
		PRU=AUPT/ nGoodParts;
		QR= nGoodParts/nTotalParts;
	}
	else
	{
		AOET=0.0;
		PRU=99.99;
		QR=0;
	}
	OEE = AUBT/ PBT * PRU * PQ / APT * GC / PQ;
	// PDT = The main usage time is the producing time of the machine. It includes only the value-adding functions
	PDT=nProductionTime;
	// Process time (BAZ) - processing time + setup time
	PCT=nProductionTime+0.0; 
if(nTotalParts<=0)
		throw std::exception("Need parts to compute KPI \n");


	// (PTU The production time per unit is the scheduled time for producing one unit.

	// Line info - planned
	//tmp+="POT, Planned Operation Time =PBT + Planned Down time\n";


	// Line info - actual

	ESUT=0; // Effective setup time == Real set up time / effective set up time (TRZ)\n";
	//OEE  = Availability * Effectiveness * Quality rate 
	SQ=0.0; // , Scrap Quantity\n";
	POQ=0; // FIXME: job total parts - // Production order quantity
	IP = 0; // Inspected Part (IP)\n";
	if(IP!= 0) 
		FPY = GP/IP; // Yield(FPY) =GP/IP\n";
	else
		FPY=0;
	

	tmp+=StdStringFormat("%8.2f,", ADET)+ "ADET, Actual unit delay time,resource BlockedTime+resource DownTime\n";
	tmp+=StdStringFormat("%8.2f,", ADOT ) + "ADOT, Actual unit down time,resource DownTime\n";
	tmp+=StdStringFormat("%8.2f,", (nProductionTime+nDownTime)/nTotalParts )+"AOET, Actual order execution time,\n";
	tmp+=StdStringFormat("%8.2f,", APT )+"APT, Actual production time, resource production time \n";
	tmp+="0,ASUT, Actual set up time,resource setup - none \n";
	tmp+=StdStringFormat("%8.2f,", AUBT )+"AUBT, Actual unit busy time , AUPT + ADET\n";
	tmp+=StdStringFormat("%8.2f,", AUPT )+"AUPT, Actual unit processing time , APT + ASUT\n";
	tmp+="0,BT, Busy time (BLZ),TotalTime\n";
	tmp+="0,CI, Consumables inventory,\n";
	tmp+=StdStringFormat("%8.2f,", SU )+"DeT, Delay time (SU), Resource blocked + downtime \n";
	tmp+=StdStringFormat("%8.2f,", SZ )+"DoT, Down time (SZ), Resource downtime\n";
	tmp+="0,ESUT, Real set up time / effective set up time (TRZ),\n";
	tmp+="0,FGI, Finished goods inventory,\n";
	tmp+="0,FPY, Yield(FPY) ,GP/IP\n";
	tmp+=StdStringFormat("%8.2f,", GP )+"GQ, Good quantity (GM GoodParts GP- Good pieces (GT)),N parts\n";
	tmp+="0,IP, Inspected Part (IP)(PT), \n";
	tmp+="0,LT, Labor time (PZt),\n";
	tmp+="0,MA, Machine,\n";
	tmp+=StdStringFormat("%8.2f,", OEE )+"OEE, OEE index , Availability * Effectiveness * Quality rate (AUBT/ PBT * PRU * PQ / APT * GC / PQ) \n";
	tmp+="0,OP, Operation process (AG),\n";
	tmp+="0,OS, Operation sequence (AFO),\n";
	tmp+=StdStringFormat("%8.2f,", BZ )+"OT, Operation time (BZ),TotalTime\n";
	tmp+=StdStringFormat("%8.2f,", PBT )+"PBT, Planned Busy Time , AUPT + ADOT \n";
	tmp+=StdStringFormat("%8.2f,", PBT )+ "PBZ, Planned busy time (PBZ), nParts*Processing/part\n";
	tmp+="0,PCT, Process time (BAZ), Resource processing\n";
	tmp+=StdStringFormat("%8.2f,", PDT )+"PDT, Production time (HNZ), Resource processing\n";
	tmp+="0,PO, Production order (FA),?\n";
	tmp+=StdStringFormat("%8.2f,", PQ )+"PQ, Production quantity,total parts good and bad\n";
	tmp+="0,POQ, Production order quantity (FAM),\n";
	tmp+="0,POT, Planned Operation Time ,PBT + Planned Down time\n";
	tmp+=StdStringFormat("%8.2f,", PQ )+ "PQ, produced quantity, N parts\n";
	tmp+="0,PRU, PlannedRunTimePerUnit, \n";
	tmp+="0,PSQ, Planned scrap quantity (GAM),\n";
	tmp+="0,PSUT, Planned set up time (PRZ),\n";
	tmp+=StdStringFormat("%8.2f,", PTU )+"PTU, Production time per unit (PEZ),\n";
	tmp+="0,PU, Production unit (PE),\n";
	tmp+=StdStringFormat("%8.2f,", PRU)+"PRU, PlannedRunTimePerUnit, \n";
	tmp+=StdStringFormat("%8.2f,", QR )+"QR,QualityRate,GC/TP\n";
	tmp+="0,RMI, Raw material inventory,\n";
	tmp+="0,RQ, Rework quantity (NM),\n";
	tmp+="0,SQ, Scrap quantity (AM),\n";
	tmp+="0,TAT, Total attendance time (GAZ,)\n";
	// TPT, Execution time is the time difference between start time and end time of a manufacturing order. It includes the busy time as well as the drop and transportation time., ";
	tmp+=StdStringFormat("%8.2f,", TPT)+"TPT, Lead time / throughput time (DLZ),\n";
	tmp+="0,TT, Transportation time (TZ),\n";
	tmp+="0,WG, Working group (AGR),\n";
	tmp+="0,WIP, Work in process inventory (WPI,)\n";
	tmp+="0,WOT, Working time (PAZ,)\n";
	tmp+="0,WP, Work place (AP),\n";
	tmp+=StdStringFormat("%8.2f,", WT)+"WT, Wait time (LZ),\n";


	
	tmp+=StdStringFormat("%8.2f,", AUBT/ PBT )+",Availability	, AUBT/ PBT\n";
	tmp+=StdStringFormat("%8.2f,", PRU * PQ / APT )+",Effectiveness	, PRU * PQ / APT or PTU * PQ / PDT\n";
	tmp+=StdStringFormat("%8.2f,", AUBT / AOET )+",Allocation ratio , AUBT / AOET\n";
	tmp+=StdStringFormat("%8.2f,", APT / AOET )+",Production process ratio , APT / AOET\n";

	tmp+=StdStringFormat("%8.2f,", BT/TPT)+",Allocation degree , BT/TPT\n";
	tmp+=StdStringFormat("%8.2f,",  PDT/BT)+",Efficiency, PDT/BT\n";	
	double test = PCT/PBT *  PRU * PQ / APT * QR;
	tmp+=StdStringFormat("%8.2f,", PCT/PBT *  PRU * PQ / APT * QR )+"NEE,NEE Index, PCT/PBT * Effectiveness * Quality rate\n";	


	return tmp;
}
std::string KPI::EquationCSVString()
{
	std::string tmp;

	tmp+="Allocation degree, 	,Allocation degree = BT/TPT	";
	tmp+="Allocation efficiency, 	,Allocation efficiency = BT/PBT\n";		
	tmp+="Availability, 	,= PDT/PBT	\n";	
	tmp+="Comprehensive Energy Consumption,	e, e = E/PQ =（∑Mi*Ri + Q）/ PQ\n";	

	tmp+="Critical Machine Capability Index, (Cmk),	Cmko = (ULV - xqq) / (3 * s) ; Cmku = (Xqq - LLV) / (3 * s)	\n";	
	tmp+="Critical Process Capability Index, (Cpk),	Cpko = (ULV - xqq) / (3 *  ) ;   Cpku = (xqq - LLV) / (3 *  )\n";	

	tmp+="Efficiency, 	,Efficiency = PDT/BT\n";	
	tmp+="Emission ratio,	,Emission ratio = (CO2energy + CO2transported goods + CO2travel + CO2internal) / VA	\n";
	tmp+="Energy ratio, 	,Energy ratio = (energy bought + energy internally produced) / VA\n";	
	tmp+="Equipment Load Rate,	,Equipment Load Rate = PQ / maximum equipment production capacity	\n";
	tmp+="Fall off Rate	, ,Fall off Rate = SQ / PQ of the first manufacturing step	\n";
	tmp+="Finished Goods Rate,	,Finished Goods Rate = GQ / consumed material\n";	
	tmp+="First Pass Yield, (FPY),	FPY = GP / PT\n";	
	tmp+="Harmful substances,	,Harmful substances = total used amount of harmful substances in tons / VA\n";	
	tmp+="Hazardous waste,	,Ratio of hazardous waste= total amount of hazardous waste/ VA\n";	
	tmp+="Integrated Goods Rate,	,Finished Goods Rate =  (Integrated Good quantity )/ (consumed material)\n";	
	tmp+="Inventory turns,	,Inventory Turns = Throughput / average inventory\n";	
	tmp+="Machine Capability Index, (Cm),	Cm = (ULV - LLV ) / (6 * s)\n";	
	tmp+="NEE Index, 	,NEE Index = PCT/PBT * Effectiveness * Quality rate\n";	
	tmp+="OEE Index,	,OEE Index = Availability * Effectiveness * Quality rate	\n";
	tmp+="Other Lost Rate,	,Other Lost Rate = other lost / consumed material	\n";
	tmp+="Preparation degree, 	,Preparation Degree = ESUT/BRZ	\n";
	tmp+="Process Capability Index, Cp,	Cp = (ULV - LLV) / (6 * Sigma )\n";

	tmp+="Production Lost Rate,	,Production Lost Rate = production lost / consumed material	\n";
	tmp+="Quality Rate,	,Quality Rate = GQ / PQ	\n";
	tmp+="Ratio of used material,	,Ratio of used material = total amount of material used / VA	\n";
	tmp+="Reworking Ratio,	,Reworking Ratio = RQ / PQ	\n";
	tmp+="Storage and Transportation Lost Rate,	,Storage and Transportation Lost Rate = (storage and transportation )/(lost consumed material)	\n";
	tmp+="Technical Usage Level,	,Technical Usage Level = PDT / (PDT + DeT)\n";	
	tmp+="Throughput,	,Throughput = PQ/TPT\n";

	tmp+="Wastage Degree, 	, Wastage Degree = SQ / PSQ	<BR> Wastage Ratio, 	Wastage Ratio = SQ / PQ	\n";
	tmp+="Wastage Ratio, ,Wastage Ratio = SQ / PQ	\n";
	tmp+="Worker productivity, 	,Worker Productivity = WOT/TAT	\n";

	return tmp;
}

//OEE Calculation	OEE index = Availability * Effectiveness * Quality rate	
//Availability	= (Actual production time) / ( Planned Busy Time)
//= MTCpower == ON / ShiftDuration
//Effectiveness	= PRU * PQ / APT
//= PlannedRunTimePerUnit * ProducedQuantity / ActualProductionTime
//= MTCCycleTime * MTCPartCount / MTCpower == ON
//Quality Rate	= GoodQuantity / ProductionQuantity
//= MTCPartCountGood /MTCPartCountGood
//Planned Operation Time 
//	MTCmode != PreventiveMaintenance?
//Planned Down Time 	Capacity Multiplier for processing times e.g., 1.1 for 90% capacity 	
//Scrap Quantity (SQ)	MTCPartCountBad
//GoodParts (GP)	MTCPartCountGood
//Inspected Part (IP)	MTCPartCount (until we add sequencing to CMM)
//Yield(FPY)	GP/IP
//Produced Quantity (PQ)	MTCPartCount

std::string ActualPeriods()
{
	std::string tmp;

	tmp+= "WOT, Work time ,";
	tmp+= "The work time is the time that a production worker needs for the execution of a manufacturing order.";
	
	tmp+= "PCT, Processing time ,";
	tmp+= "The processing time is the time needed for set-up and for the main usage.";

	tmp+= "BT, Busy time ,";
	tmp+= "The busy time is the time that a production unit is used for the execution of a manufacturing order.";
	
	tmp+= "TPT, Execution time {TD},";
	tmp+= "The execution time is the time difference between start time and end time of a manufacturing order. It includes the busy time as well as the drop and transportation time.";
	
	tmp+= "TAT, Total attendance time,";
	tmp+= "The total attendance is the time that a production employee is present for working in the company. It is the difference between Come and Go but without breaks.";
	
	tmp+= "PDT, Main usage time {th},";
	tmp+= "The main usage time is the producing time of the machine. It includes only the value-adding functions.";
	
	tmp+= "LZ, Set aside time,";
	tmp+= "The set aside time is the time in which the material is not in progress of the manufacturing process and also is not on transport.";
	
	tmp+= "PZ, Staff time ,";
	tmp+= "The staff time is the total time consumed by the production staff to execute a manufacturing order.";
	
	tmp+= "DOT, Downtime,";
	tmp+= "The down time is the time when the machine is not running with orders, although they are available.";
	
	tmp+= "SU, Malfunction-caused interrupts {TBS, TMS},";
	tmp+= "The malfunction-caused interrupts are times, which occur unplanned during the order processing and thus lead to an unwanted extension of busy times.";
	
	tmp+= "ESUT, Actual set-up time,";
	tmp+= "The actual set-up time is the time that was consumed for the preparation of an order at a production unit.";
	
	tmp+= "TT, Transport time ,";
	tmp+= "The transport time is the time required for transport between production units, or to and from the warehouse of the used material.";

	return tmp;
}
std::string LogisticalQuantities()
{
	std::string tmp;

	tmp+= "POQ, Order quantity,";
	tmp+= "The order quantity is the planned quantity for a production order (lot size, production order quantity).";
	
	tmp+= "SQ, Wastage quantity,";
	tmp+= "The wastage quantity is the produced quantity that did not meet quality requirements and either has to be scrapped or recycled.";
	
	tmp+= "PSQ, Planned wastage quantity,";
	tmp+= "The planned wastage quantity is the amount of process-related wastage that is expected when manufacturing the product (e.g. at the start or ramp-up phases of the manufacturing systems).";
	
	tmp+= "GQ, Good quantity,";
	tmp+= "The good quantity is the produced quantity that meets quality requirements (see also 6.7.1).";
	
	tmp+= "RQ, Rework quantity,";
	tmp+= "The rework quantity is the produced quantity that missed the quality requirements. However, these requirements can be met by subsequent work.";
	
	tmp+= "PQ, Produced quantity,";
	tmp+= "The produced quantity is the quantity that a production unit has produced in relation to a production order.";

	return tmp;
}