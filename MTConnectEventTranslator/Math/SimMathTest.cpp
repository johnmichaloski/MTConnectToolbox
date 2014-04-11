//
// SimMathTest.cpp
//

#include "stdafx.h"
#include "SimMath.h"
#include "StdStringFcn.h"

void StatTest()
{

	Distribution stat, stat2,stat3,stat4;
	stat.SetParameters(_T("normal"), 10, 5);
	stat2.SetParameters(_T("uniform"), 5, 10);
	stat3.SetParameters(_T("exponential"), 5, 10,18);
	stat4.SetParameters(_T("weibull"),1,5);  // gamma, k
	std::vector<double> normdata,unidata,expdata,weibdata;
	
	for(int i=0; i< 10000; i++)
	{
		normdata.push_back(stat.RandomVariable());
		unidata.push_back(stat2.RandomVariable());
		expdata.push_back(stat3.RandomVariable());
		weibdata.push_back(stat4.RandomVariable());
	
	}

	std::string results;
	StatFitting statfit;
	statfit.EstimateAll(normdata);
	results=statfit.ToString();
	OutputDebugString(results.c_str());


	StatFitting statfit1;
	statfit1.EstimateAll(unidata);
	results=statfit1.ToString();
	OutputDebugString(results.c_str());

	StatFitting statfit2;
	statfit2.EstimateAll(expdata);
	OutputDebugString(statfit2.ToString().c_str());

	StatFitting statfitW;
	double a,b;
	std::vector<double> T = TokenList<double>("16,34,53,75,93,120", ","); // vlist_of<double>( 16 )( 34)( 53)( 75)( 93)( 120 );
	statfitW.ComputeWeibull( T, a, b);



	//http://home.comcast.net/~pstlarry/BaikoMan.htm
	StatFitting statfit3;
	//std::vector<double> weibulldata = data.GetData("D:\\Program Files\\NIST\\proj\\DES\\SimulationModel\\CapacityCalculator\\Data\\WeibullTestDat1.txt");
	//statfit3.EstimateAll(weibdata);
	Distribution dist = statfit3.BestFit(weibdata);
	OutputDebugString(dist.ToString().c_str());

}

// http://www.qualitydigest.com/jan99/html/weibull.html
