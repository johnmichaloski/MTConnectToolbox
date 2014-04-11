//
// KPI.h
//

#pragma once

#include <string>
#include <StdStringFcn.h>

#include "StateModel.h"

class KPI
{
public:
	KPI(Stats stats);
	~KPI(void);

	void Init(Stats &stats1);
	void Compute(); // throws error
	std::string ByResources(Stats &stats1);

	static std::string EquationCSVString();
	std::string AbbrvCSVString();

	double dTotalTime; // seconds
	int  nTotalParts;
	int  nGoodParts;
	double nBlockedTime;
	double nStarvedTime;
	double nDownTime;
	double nProductionTime;
	double nOffTime;
	double nRepairTime;
	double nIdleTime;
	double 	MTBF;
	double MTTR;
	double MTTP;


	// KPI 
	double 	PSUT; //Planned set up time (PRZ)\n";

	double BZ, WT, PRU,OEE, GT, GQ, GC, TPT,QR, PDT, PCT ;
	// Line info - actual

	double  ASUT; //=0.0; // actual setup time
	double  AUPT, APT; // nProductionTime;  ///Actual production time 
	double   ADET; //=nBlockedTime+ nStarvedTime; // Actual unit delay time
	double   PBZ, BT; //=APT + ADET + ASUT; //Busy time (BLZ)\n";
	double  SU; //= nStarvedTime' //, Delay time (SU)\n";
	double SZ; //=nDownTime; //Down time Do (SZ)\n";
	double ESUT; //=0; // Effective setup time == Real set up time / effective set up time (TRZ)\n";
	//OEE  = Availability * Effectiveness * Quality rate 
	double PQ; // = nTotalParts; // produced quantity\n";
	double SQ; // Scrap Quantity\n";
	double OT, PEZ;
	double IP; //  Inspected Part (IP)\n";
	double FPY; //  Yield(FPY) =GP/IP\n";
	
	double GP; //nGoodParts // Good pieces (GT) good quantity (GM)\n";

	// Unit operation (not resources) defintions
	//tmp+="OP, Operation process (AG)\n";
	//tmp+="OS, Operation sequence (AFO)\n";
	//tmp+="OT, Operation time (BZ)\n";

	double AUP; // (APT + ASUT) / nTotalPart; //  or AUP Actual unit processing time = APT + ASUT (Divided by parts?)
	double AUBT; // AUPT + ADET; //  Actual unit busy time (Divided by parts?)
	double ADOT; //nDownTime; //  Actual unit down time (Divided by parts?)
	double AOET; // dTotalTime; //  Actual order execution time
	//double AUBT; // AUPT + ADET; // Actual unit busy time 
	double PBT; //
	double PTU;
	double POQ; // Production order quantity
};

