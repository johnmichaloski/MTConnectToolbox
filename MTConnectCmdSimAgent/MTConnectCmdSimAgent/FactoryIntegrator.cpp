//
//
//
#include "stdafx.h"
#include "FactoryIntegrator.h"
#include "StdStringFcn.h"

void SetupFactory(double dFeedoveride, double dSetupFactor)
{
	Resource * r1 = Resource::Create<Resource>("r1");
	r1->name = "M1";
	r1->nMaxPalletSize=10;

	Resource * r2 = Resource::Create<Resource>("r2");
	r2->name = "M2";
	r2->nMaxPalletSize=20;

	Resource * r3old = Resource::Create<Resource>("r3old");
	r3old->name = "M3Old";
	r3old->nMaxPalletSize=10;

	Resource * r3new = Resource::Create<Resource>("r3new");
	r3new->name = "M3New";
	r3new->nMaxPalletSize=10;

	Resource * r4 = Resource::Create<Resource>("r4");
	r4->name = "M4";
	r4->nMaxPalletSize=20;

	Cell * c1 = Cell::Create<Cell>("c1");
	c1->name = "C1";
	c1->resourceIds.push_back("r1");
	Cell * c2 = Cell::Create<Cell>("c2");
	c2->name = "C2";
	c2->resourceIds.push_back("r2");
	Cell * c3 = Cell::Create<Cell>("c3");
	c3->name = "C3";
	c3->resourceIds.push_back("r3old");
	c3->resourceIds.push_back("r3new");
	Cell * c4 = Cell::Create<Cell>("c4");
	c4->name = "C4";
	c4->resourceIds.push_back("r4");

	///////////////////////////////////////////////////
	Process * pp1p1= Process::Create<Process>("pp1p1");
	Process * pp1p2= Process::Create<Process>("pp1p2");
	Process * pp1p3= Process::Create<Process>("pp1p3");
	Process * pp1p4= Process::Create<Process>("pp1p4");

	pp1p1->cellsRequired.push_back("c1");
	pp1p1->AddProperty("r1","SetupPP1[P1]C1","setup","triangular,6,8,10"); // ConvertToString(60*dSetupFactor));
	pp1p1->AddProperty("r1","MachinePP1[P1]C1","program","triangular,6,8,10"); // ConvertToString(100*dFeedoveride));

	pp1p2->cellsRequired.push_back("c2");
	pp1p2->AddProperty("r2","SetupPP1[P2]C2","setup","triangular,5,8,10"); // ConvertToString(15*dSetupFactor));
	pp1p2->AddProperty("r2","MachinePP1[P2]C2","program","triangular,5,8,10"); // ConvertToString(25*dFeedoveride));

	pp1p3->cellsRequired.push_back("c3");
	pp1p3->AddProperty("r3old","SetupPP1[P3]C3Old","setup","triangular,15,20,25"); // ConvertToString(150*dSetupFactor));
	pp1p3->AddProperty("r3old","MachinePP1[P3]C3Old","program","triangular,15,20,25"); // ConvertToString(240*dFeedoveride));
	pp1p3->AddProperty("r3new","SetupPP1[P3]C3New","setup","triangular,15,20,25"); // ConvertToString(20*dSetupFactor));
	pp1p3->AddProperty("r3new","MachinePP1[P3]C3New","program","triangular,15,20,25"); // ,ConvertToString(120*dFeedoveride));

	pp1p4->cellsRequired.push_back("c4");
	pp1p4->AddProperty("r4","SetupPP1[P4]C4","setup","triangular,8,12,16"); // ConvertToString(20*dSetupFactor));
	pp1p4->AddProperty("r4","MachinePP1[P4]C4","program","triangular,8,12,16"); // ConvertToString(30*dFeedoveride));
	pp1p4->AddProperty("r4","InspectPP1[P4]C4","inspect","30");
	pp1p4->scrapPercentage="10";

	Process * pp2p1= Process::Create<Process>("pp2p1");
	Process * pp2p2= Process::Create<Process>("pp2p2");
	Process * pp2p3= Process::Create<Process>("pp2p3");
	Process * pp2p4= Process::Create<Process>("pp2p4");
	Process * pp2p5= Process::Create<Process>("pp2p5");

	pp2p1->cellsRequired.push_back("c1");
	pp2p1->AddProperty("r1","SetupPP2[P1]C1","setup","triangular,11,13,15"); // ConvertToString(10*dSetupFactor));
	pp2p1->AddProperty("r1","MachinePP2[P1]C1","program","triangular,11,13,15"); // ConvertToString(30*dFeedoveride) );

	pp2p2->cellsRequired.push_back("c2");
	pp2p2->AddProperty("r2","SetupPP2[P2]C2","setup","triangular,4,6,8"); // ConvertToString(25*dSetupFactor));
	pp2p2->AddProperty("r2","MachinePP2[P2]C2","program","triangular,4,6,8"); // ConvertToString(35*dFeedoveride));

	pp2p3->cellsRequired.push_back("c4");
	pp2p3->AddProperty("r4","SetupPP2[P3]C4","setup", "triangular,15,18,21"); // ConvertToString(25*dSetupFactor));
	pp2p3->AddProperty("r4","MachinePP2[P3]C4","program","triangular,15,18,21"); //  ConvertToString(45*dFeedoveride));

	pp2p4->cellsRequired.push_back("c2");
	pp2p4->AddProperty("r2","SetupPP2[P4]C2","setup", "triangular,6,9,12"); //  ConvertToString(25*dSetupFactor));
	pp2p4->AddProperty("r2","MachinePP2[P4]C2","program","triangular,6,9,12"); // ConvertToString(50*dFeedoveride));
	
	pp2p5->cellsRequired.push_back("c3");
	pp2p5->AddProperty("r3old","SetupPP2[P5]C3Old","setup","triangular,20,23,27"); // ConvertToString(50*dSetupFactor));
	pp2p5->AddProperty("r3old","MachinePP2[P5]C3Old","program", "triangular,20,23,27"); // ConvertToString(300*dFeedoveride));
	pp2p5->AddProperty("r3old","InspectPP2[P5]C3Old","inspect","triangular,20,23,27"); //);
	pp2p5->AddProperty("r3new","SetupPP2[P5]C3New","setup","triangular,10,13,17");
	pp2p5->AddProperty("r3new","MachinePP2[P5]C3New","program","triangular,10,13,17");
	pp2p5->AddProperty("r3new","InspectPP2[P5]C3New","inspect","triangular,10,13,17");
	pp2p5->scrapPercentage="10,5";

	Process * pp3p1= Process::Create<Process>("pp3p1");
	Process * pp3p2= Process::Create<Process>("pp3p2");
	Process * pp3p3= Process::Create<Process>("pp3p3");

	pp3p1->cellsRequired.push_back("c2");
	pp3p1->AddProperty("r2","SetupPP3[P1]C2","setup","triangular,7,9,11"); // ConvertToString(15*dSetupFactor));
	pp3p1->AddProperty("r2","MachinePP3[P1]C2","program","triangular,7,9,11"); //  ConvertToString(20*dFeedoveride));

	pp3p2->cellsRequired.push_back("c1");
	pp3p2->AddProperty("r1","SetupPP3[P2]C1","setup","triangular,7,10,13"); // ConvertToString(25*dSetupFactor));
	pp3p2->AddProperty("r1","MachinePP3[P2]C1","program", "triangular,7,10,13"); // ConvertToString(45*dFeedoveride));

	pp3p3->cellsRequired.push_back("c3");
	pp3p3->AddProperty("r3old","SetupPP3[P3]R3Old","setup","12"); // ConvertToString(120*dSetupFactor));
	pp3p3->AddProperty("r3old","MachinePP3[P3]R3Old","machine","triangular,18,23,24"); // ConvertToString(360*dFeedoveride));
	pp3p3->AddProperty("r3old","InspectPP3[P3]R3Old","inspect", "triangular,10,13,15");
	pp3p3->AddProperty("r3new","SetupPP3[P3]R3New","setup","10");
	pp3p3->AddProperty("r3new","MachinePP3[P3]R3New","machine", "triangular,20,22,23");
	pp3p3->AddProperty("r3new","InspectPP3[P3]R3New","inspect", "triangular,8,10,12");
	pp3p3->scrapPercentage="15,5";

	///////////////////////////////////////////////////
	ProcessPlan * pp1= ProcessPlan::Create<ProcessPlan>("pp1");
	ProcessPlan * pp2= ProcessPlan::Create<ProcessPlan>("pp2");
	ProcessPlan * pp3= ProcessPlan::Create<ProcessPlan>("pp3");

	pp1->processes.push_back(pp1p1); pp1->processIds.push_back(pp1p1->identifier);
	pp1->processes.push_back(pp1p2); pp1->processIds.push_back(pp1p2->identifier);
	pp1->processes.push_back(pp1p3); pp1->processIds.push_back(pp1p3->identifier);
	pp1->processes.push_back(pp1p4); pp1->processIds.push_back(pp1p4->identifier);

	pp2->processes.push_back(pp2p1); pp2->processIds.push_back(pp2p1->identifier);
	pp2->processes.push_back(pp2p2); pp2->processIds.push_back(pp2p2->identifier);
	pp2->processes.push_back(pp2p3); pp2->processIds.push_back(pp2p3->identifier);
	pp2->processes.push_back(pp2p4); pp2->processIds.push_back(pp2p4->identifier);
	pp2->processes.push_back(pp2p5); pp2->processIds.push_back(pp2p5->identifier);

	pp3->processes.push_back(pp3p1); pp3->processIds.push_back(pp3p1->identifier);
	pp3->processes.push_back(pp3p2); pp3->processIds.push_back(pp3p2->identifier);
	pp3->processes.push_back(pp3p3); pp3->processIds.push_back(pp3p3->identifier);



	Part * bracket = Part::Create<Part>("bracket");
	Part * shim = Part::Create<Part>("shim");
	Part * bodyjoint = Part::Create<Part>("bodyjoint");

	bracket->processplanidentifier="pp1";
	shim->processplanidentifier="pp2";
	bodyjoint->processplanidentifier="pp3";

	Job * job1 = Job::Create<Job>("job1");
	job1->partIds.push_back("bracket");
	job1->partIds.push_back("shim");
	job1->partIds.push_back("bodyjoint");
	job1->partQuantity.push_back("10");
	job1->partQuantity.push_back("10");
	job1->partQuantity.push_back("10");
	job1->AddProperty("MaxQueueSize","4","","");
	job1->AddProperty("KWH","0.11", "KWH","COST OF KWH");

}