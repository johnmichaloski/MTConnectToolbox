//
// SimMath.h
//

#pragma once
#include <atlstr.h>
#include <vector>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "adevs_rand.h"

extern void StatTest();

class CMath
{
public:
	double round(double number);
	/// Represents a Lanczos approximation of the Gamma function.
	double gamma(double x);
	double gammln(double xx);

};
__declspec(selectany)  CMath Math;

class Distribution
{
public:
	Distribution(LPCTSTR distribution=_T(""), double a=0, double b=0, double c=0, double d=0)
	{
		_parameters.resize(4);
		SetParameters(distribution, a, b,  c, d);
	}
	Distribution(LPCTSTR distribution, std::vector<double> ds)
	{
		_parameters.resize(4);
		_szDistribution=distribution;
		for(int i=0; i<ds.size(); i++) 
			_parameters[i]=ds[i];
	}
	void SetParameters(LPCTSTR distribution=_T(""), double a=0, double b=0, double c=0, double d=0)
	{
		_szDistribution=distribution;
		_parameters[0]=a; _parameters[1]=b; _parameters[2]=c; _parameters[3]=d;
	}
	CString & Name() { return _szDistribution; }
	double & operator[](int i) { return _parameters[i]; }

	double RandomVariable();
	double cdf(double x) { return cdf(x, _szDistribution,_parameters);}
	double pdf(double x) { return pdf(x, _szDistribution,_parameters);}
	double cdf(double x, LPCTSTR szDistribution, std::vector<double> parameters );
	double pdf(double x, LPCTSTR szDistribution, std::vector<double> parameters );
	void SetSeed(unsigned long seed){ rv.set_seed(seed); }
	std::string ToString();
	////////////////////////////////////////////////////////////////////////////////////////////
	CString _szDistribution;
	std::vector<double> _parameters;
	adevs::rv rv;
	adevs::mtrand   random; 

};

// Notes: mean variance of poisson are equal
class StatFitting
{
	typedef std::map<std::string, std::vector<double> >::iterator MapIterator;
	typedef std::map<std::string, double > GoodnessMap;
public:
	Distribution BestFit(std::vector<double> & x);
	std::vector<double> GetData(std::string filename, std::string commentchar=std::string(";"));

	void EstimateAll(std::vector<double> & x);
	void ComputeMeanStdDeviation(std::vector<double> & x,  double& mean,  double& variance,  double &stdx);
	double & Mean() { return _mean; }
	double & Std() { return _stdx; }
	double & Variance() { return _variance; }
	void ksone(std::vector<double> data, Distribution & dist, double *d,	double *prob);
	std::vector<double> GetStat(LPCTSTR distname);
	std::string ToString();
	void ComputeWeibull(std::vector<double> & T, double &a, double &b);
	//////////////////////////////////////////////////////////////////////
private:
	void AddStat(std::string dist, int n, double d1, ...);
	double _stdx;
	double _variance;
	double _mean;
	double _rootmeansq;
	std::map<std::string, std::vector<double> > stats;
	GoodnessMap goodness;
	std::map<std::string, double> D;
	double _min, _max, _mode;

};


class DescriptiveResult
{
public:
	// sortedData is used to calculate percentiles
	std::vector<double> sortedData;
	DescriptiveResult() { }
	UINT Count;
	double Sum;
	double Mean;
	double GeometricMean;
	double HarmonicMean;
	double Min;
	double Max;
	double Range;
	double Variance;
	double StdDev;
	double Skewness;
	double Kurtosis;
	double IQR;
	double Median;
	double FirstQuartile;
	double ThirdQuartile;
	double SumOfError;
	double SumOfErrorSquare;
	double Percentile(double percent);
} ;


/// <summary>
/// Descriptive class
/// </summary>
class Descriptive
{
public:
	std::vector<double> data;
	std::vector< double> sortedData;

	DescriptiveResult Result;
	Descriptive()
	{ 

	} // default empty constructor
	Descriptive(std::vector< double> dataVariable)
	{
		data = dataVariable;
	}

	void Analyze()
	{

		// initializations
		Result.Count = 0;
		Result.Min = Result.Max = Result.Range = Result.Mean =
			Result.Sum = Result.StdDev = Result.Variance = 0.0;

		double sumOfSquare = 0.0;
		double sumOfESquare = 0.0; // must initialize

		std::vector< double> squares( data.size());
		double cumProduct = 1.0; // to calculate geometric mean
		double cumReciprocal = 0.0; // to calculate harmonic mean

		// First iteration
		for (int i = 0; i < data.size(); i++)
		{
			if (i==0) // first data point
			{
				Result.Min = data[i];
				Result.Max = data[i];
				Result.Mean = data[i];
				Result.Range = 0.0;
			}
			else
			{ // not the first data point
				if (data[i] < Result.Min) Result.Min = data[i];
				if (data[i] > Result.Max) Result.Max = data[i];
			}
			Result.Sum += data[i];
			squares[i] = pow(data[i], 2); //TODO: may not be necessary
			sumOfSquare += squares[i];

			cumProduct *= data[i];
			cumReciprocal += 1.0 / data[i];
		}

		Result.Count = (UINT)data.size();
		double n = (double)Result.Count; // use a shorter variable in double type
		Result.Mean = Result.Sum / n;
		Result.GeometricMean = pow(cumProduct, 1.0 / n);
		Result.HarmonicMean = 1.0 / (cumReciprocal / n); // see http://mathworld.wolfram.com/HarmonicMean.html
		Result.Range = Result.Max - Result.Min;

		// second loop, calculate Stdev, sum of errors
		//double[] eSquares = new double[data.Length];
		double m1 = 0.0;
		double m2 = 0.0;
		double m3 = 0.0; // for skewness calculation
		double m4 = 0.0; // for kurtosis calculation
		// for skewness
		for (int i = 0; i < data.size(); i++)
		{
			double m = data[i] - Result.Mean;
			double mPow2 = m * m;
			double mPow3 = mPow2 * m;
			double mPow4 = mPow3 * m;

			m1 += abs(m);

			m2 += mPow2;

			// calculate skewness
			m3 += mPow3; 

			// calculate skewness
			m4 += mPow4; 

		}

		Result.SumOfError = m1;
		Result.SumOfErrorSquare = m2; // Added for Excel function DEVSQ
		sumOfESquare = m2;

		// var and standard deviation
		Result.Variance = sumOfESquare / ((double)Result.Count - 1);
		Result.StdDev = sqrt(Result.Variance);

		// using Excel approach
		double skewCum = 0.0; // the cum part of SKEW formula
		for (int i = 0; i < data.size(); i++)
		{
			skewCum += pow((data[i] - Result.Mean) / Result.StdDev, 3);
		}
		Result.Skewness = n / (n - 1) / (n - 2) * skewCum;

		// kurtosis: see http://en.wikipedia.org/wiki/Kurtosis (heading: Sample Kurtosis)
		double m2_2 = pow(sumOfESquare, 2);
		Result.Kurtosis = ((n + 1) * n * (n - 1)) / ((n - 2) * (n - 3)) *
			(m4 / m2_2) -
			3 * pow(n - 1, 2) / ((n - 2) * (n - 3)); // second last formula for G2

		// calculate quartiles 
		sortedData=data; 
		std::sort(sortedData.begin(), sortedData.end());


		// copy the sorted data to result object so that
		// user can calculate percentile easily
		Result.sortedData=sortedData;

		Result.FirstQuartile = percentile(sortedData, 25);
		Result.ThirdQuartile = percentile(sortedData, 75);
		Result.Median = percentile(sortedData, 50);
		Result.IQR = percentile(sortedData, 75) -
			percentile(sortedData, 25);

	} // end of method Analyze

	static double percentile(std::vector<double> & sortedData, double p)
	{
		// algo derived from Aczel pg 15 bottom
		if (p >= 100.0) 
			return sortedData[sortedData.size() - 1];

		double position = (double)(sortedData.size() + 1) * p / 100.0;
		double leftNumber = 0.0, rightNumber = 0.0;

		double n = p / 100.0 * (sortedData.size()- 1) + 1.0;

		if (position >= 1)
		{
			leftNumber = sortedData[(int)floor(n) - 1];
			rightNumber = sortedData[(int)floor(n)];
		}
		else
		{
			leftNumber = sortedData[0]; // first data
			rightNumber = sortedData[1]; // first data
		}

		if (leftNumber == rightNumber)
			return leftNumber;
		else
		{
			double part = n - floor(n);
			return leftNumber + part * (rightNumber - leftNumber);
		}
	} // end of internal function percentile
	CString ToString()
	{
		CString tmp;

		tmp.AppendFormat("Count = %d \n", Result.Count);
		tmp.AppendFormat("Sum  = %f \n", Result.Sum);
		tmp.AppendFormat("Mean  = %.3f \n", Result.Mean);
		tmp.AppendFormat("Variance  = %.3f \n", Result.Variance);
		tmp.AppendFormat("StdDev  = %.3f} \n", Result.StdDev);
		tmp.AppendFormat("Skewness= %.3f \n", Result.Skewness);
		tmp.AppendFormat("Kurtosis= %.3f \n", Result.Kurtosis);
		tmp.AppendFormat("GeometricMean  = %.3f \n", Result.GeometricMean);
		tmp.AppendFormat("HarmonicMean  = %.3f  \n", Result.HarmonicMean);
		tmp.AppendFormat("Min  = %.3f  \n", Result.Min);
		tmp.AppendFormat("Max  = %.3f  \n", Result.Max);
		tmp.AppendFormat("Range  = %.3f  \n", Result.Range);
		tmp.AppendFormat("IQR  = %.3f  \n", Result.IQR);
		tmp.AppendFormat("Median  = %.3f  \n", Result.Median);
		tmp.AppendFormat("FirstQuartile  = %.3f } \n", Result.FirstQuartile);
		tmp.AppendFormat("ThirdQuartile  = %.3f  \n", Result.ThirdQuartile);
		tmp.AppendFormat("ThirdQuartile  = %.3f } \n", Result.ThirdQuartile);
		return tmp;
	}
};


inline double DescriptiveResult::Percentile(double percent)
{
	return Descriptive::percentile(sortedData, percent);
}

// http://www.codeproject.com/Articles/58289/C-Round-Function.aspx in comments
inline double Round(double dVal, short iPlaces)
{
	double dRetval;
	double dMod = 0.0000001;

	if (dVal < 0)
		dMod = -0.0000001;

	dRetval =  dVal;
	dRetval += (5/pow(10.,iPlaces+1));
	dRetval *= pow(10.,iPlaces);
	dRetval =  floor(dRetval+dMod);
	dRetval /= pow(10.,iPlaces);

	return (dRetval);
}

// Fisher-Yates shuffle (stop after M iterations):
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>
template<class bidiiter>
inline bidiiter random_unique(bidiiter begin, bidiiter end, size_t num_random) {
    size_t left = std::distance(begin, end);
    while (num_random--) {
        bidiiter r = begin;
        std::advance(r, rand()%left);
        std::swap(*begin, *r);
        ++begin;
        --left;
    }
    return begin;
}