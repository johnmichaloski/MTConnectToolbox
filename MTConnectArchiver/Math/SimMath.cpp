//
// SimMath.cpp
//

#include "stdafx.h"
#include "SimMath.h"
#include <stdarg.h>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <boost/math/distributions.hpp>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////
double  CMath::round(double number)
{
	return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}
/// Represents a Lanczos approximation of the Gamma function.
double  CMath::gamma(double x)
{
	static double LanczosCoefficients[7] = { 1.000000000190015, 76.18009172947146, 
		-86.50532032941677, 24.01409824083091, -1.231739572450155, 1.208650973866179e-3, -5.395239384953e-6 };         
	double sum = LanczosCoefficients[0];
	for (int index = 1; index <= 6; index++)
	{
		sum += LanczosCoefficients[index] / (x + index);
	}

	return sqrt(2.0 * M_PI ) / x * pow(x + 5.5, x + 0.5) / exp(x + 5.5) * sum;
}

//Given the arrays datal [1.. nil and data2 El. n2J this routine returns Student’s t as t,
//and its significance as prob. small values of prob indicating that the arrays have significantly
//different means. The data arrays are assumed to be drawn from populations with the same
//true variance.
void ttest(std::vector<double> data1, std::vector<double>  data2,   double &t,  double &prob)
{
	//float betai(float a, float b, float x);
	double var1 = 0, var2 = 0, svar, df, ave1 = 0, ave2 = 0, stdx1=0, stdx2=0;
	double n1=data1.size();
	double n2=data2.size();

	StatFitting().ComputeMeanStdDeviation(data1,  ave1,  var1,  stdx1);
	StatFitting().ComputeMeanStdDeviation(data2,  ave2,  var2,  stdx2);
	df = n1 + n2 - 2;                                         //Degrees of freedom.
	svar = ((n1 - 1) * var1 + (n2 - 1) * var2) / df;                  // Pooled variance.
	t = (ave1 - ave2) / sqrt(svar * (1.0 / n1 + 1.0 / n2));
	//prob=betai(O.5*df,O.5,df/(df+(t*t))) ;          See equation (6.4.9).

}

//Returns the value ln[􀀀(xx)] for xx > 0. Ch. 6
//Internal arithmetic will be done in double precision, a nicety that you can omit if ve-figure
//accuracy is good enough.
double CMath::gammln(double xx)
{
	static double cof[6] = { 76.18009172947146, -86.50532032941677, 24.01409824083091, -1.231739572450155, 0.1208650973866179e-2, -0.5395239384953e-5 };
	double x, y, tmp, ser;
	int j;
	y = x = xx;
	tmp = x + 5.5;
	tmp -= (x + 0.5) *log(tmp);
	ser = 1.000000000190015;
	for (j = 0; j <= 5; j++) ser += cof[j] / ++y;
	return -tmp + log(2.5066282746310005 * ser / x);
}


///////////////////////////////////////////////////////////////////////////////////////////////
void StatFitting::AddStat(std::string dist, int n, double d1, ...)
{
	// Trace model: ATL::CTrace::s_trace.TraceV(m_pszFileName, m_nLineNo, dwCategory, nLevel, pszFmt, ptr);
	std::vector<double>  params;
	va_list ptr; 
	va_start(ptr, d1);

	for(int i=0; i<n ; i++)
	{
		params.push_back(d1);
		d1=va_arg(ptr,double);
	}
	//params.push_back(d1); params.push_back(d2); params.push_back(d3); params.push_back(d4);
	stats[dist]=params;
	va_end(ptr);

}

std::vector<double> StatFitting::GetStat(LPCTSTR distname)
{
	static std::vector<double> empty;
	MapIterator it;
	it=stats.find(distname);
	if(it!=stats.end())
		return (*it).second;
	return empty;

}
std::string StatFitting::ToString()
{
	std::stringstream tmp;
	
	for(MapIterator it=stats.begin(); it!=stats.end(); it++)
	{
		tmp.precision(3);
		std::vector<double> & params((*it).second);
		tmp << (*it).first;
		for(int j=0; j< params.size(); j++)
			tmp<< params[j];
		tmp<< " D=" << D[(*it).first];
		tmp<< " Goodness=" <<  goodness[(*it).first];
		tmp<<_T("\n");
	}
	return tmp.str();
}
void StatFitting::EstimateAll(std::vector<double> & x)
{
	ComputeMeanStdDeviation( x,  _mean, _variance,  _stdx);

	sort(x.begin(), x.end());
	_min=x.front(); _max=x.back(); _mode=x[x.size()/2];

	// Add  estimate 
	AddStat("normal", 2, _mean, _stdx);
	AddStat("poisson",2,  _mean, _stdx);
	AddStat("exponential",  1, 1/_mean); // gamma value //http://www.aiaccess.net/English/Glossaries/GlosMod/e_gm_exponential.htm
	AddStat("gamma", 2, pow(_mean/_stdx, 2), _variance/_mean); // gamma: alpha, berta
	AddStat("erlang", 2, (double) Math.round(pow(_mean/_stdx, 2)), _variance/_mean); // erlang: k(rounded alpha), berta
	AddStat("lognormal", 2, _mean, _stdx);

	// beta
	double alpha1= _mean*(1-_mean)*_mean/(_variance-1);
	double alpha2= _mean*((1-_mean)*(((1-_mean) * _mean*_variance)- 1));
	AddStat("beta", 2, alpha1, alpha2); // beta: alpha1, alpha2

	AddStat("uniform", 2, _min, _max); // min, max, if min==0, 
	AddStat("triangular", 3,  _min, _mean* 3.0 - _min - _max, _max); // 

	// http://www.weibull.com/LifeDataWeb/general_examples_using_the_exponential_distribution.htm ??

	// Weibull
	// beta < 1.0 indicates infant mortality
	// beta = 1.0 means random failures (independent of age)
	// beta > 1.0 indicates wear out failures
	// http://www.treesearch.fs.fed.us/pubs/6975 
	// http://www.statsoft.com/textbook/process-analysis/
	// root mean square (abbreviated RMS or rms), also known as the quadratic mean
	// Method of moments
	//double betahat= _variance/(_mean*_mean);
	//double alphahat= _mean/(Math.gamma(1.0+(1.0/betahat)));
	double alphahat, betahat;
	this->ComputeWeibull(x,alphahat, betahat);
	AddStat("weibull", 2, alphahat, betahat); // b

	// lognormal
	// http://www.chinarel.com/onlinebook/LifeDataWeb/estimation_of_the_parameters_log.htm

	for(MapIterator it=stats.begin(); it!=stats.end(); it++)
	{
		std::vector<double> & params((*it).second);	// Estimate goodness
		double d, prob;
		Distribution dist((*it).first.c_str(), params);
		try
		{
			OutputDebugString(dist.ToString().c_str());
			ksone(x,dist,&d,&prob);
			goodness[(*it).first]=prob;
			D[(*it).first]=d;
		}
		catch(std::exception e)
		{
			std::stringstream str;
			str << dist.ToString() << " Exception: " << e.what() << "\n";
			OutputDebugString(str.str().c_str());
			goodness[(*it).first]=-1.0;
			D[(*it).first]=-1;

		}

	}


}
void StatFitting::ComputeMeanStdDeviation(std::vector<double> & x,  double& mean,  double& variance,  double &stdx)
{
	int n = x.size();

	//http://www.johndcook.com/standard_deviation.html
	stdx = 0.0;
	mean = 0.0;

	for (int i = 0; i < n; ++i)
	{
		mean += x[i];
		stdx += x[i] * x[i];
	}
	_rootmeansq = sqrt(stdx/n);
	variance=(n * stdx - mean * mean) / (n * (n - 1.0));
	stdx = sqrt(variance);
	mean /= (double)n;
}

// http://www.koders.com/c/fid9EDA8BAE53A917548017A2D0053EBAB572538D5B.aspx?s=crc

#define EPS1 0.001
#define EPS2 1.0e-8

static double probks(double alam)
{
	int j;
	double a2,fac=2.0,sum=0.0,term,termbf=0.0;

	a2 = -2.0*alam*alam;
	for (j=1;j<=100;j++) 
	{
		term=fac*exp(a2*j*j);
		sum += term;
		if (fabs(term) <= EPS1*termbf || fabs(term) <= EPS2*sum) 
			return sum;
		fac = -fac;
		termbf=fabs(term);
	}
	return 1.0;
}
#undef EPS1
#undef EPS2

/**
	D - floating scalar giving the Kolmogorov-Smirnov statistic.   It 
		specified the maximum deviation between the cumulative 
		distribution of the data and the supplied function 
	prob - floating scalar between 0 and 1 giving the significance level of
		the K-S statistic.   Small values of PROB show that the 
       	cumulative distribution function of DATA is significantly 
		different from FUNC_NAME.

*/

#define FMAX(X,Y) ((X>Y)?X:Y)
void StatFitting::ksone(std::vector<double> data,Distribution & dist, double *d,	double *prob)
{
	unsigned long n=data.size()-1;
	unsigned long j;
	double dt,en,ff,fn,fo=0.0;

	std::sort(data.begin(), data.end());
	en=n;
	*d=0.0;
	for (j=1;j<=n;j++) 
	{
		fn=j/en;
		ff= dist.cdf(data[j]);
		//ff=(*func)(data[j]);
		dt=FMAX(fabs(fo-ff),fabs(fn-ff));
		if (dt > *d) 
			*d=dt;
		fo=fn;
	}
	en=sqrt(en);
	*prob=probks((en+0.12+0.11/en)*(*d));
}
/////////////////////////////////////////////////////////////////////////////
double Distribution::RandomVariable()
{
	if(_szDistribution=="constant")
		return _parameters[0];
	else if(_szDistribution=="beta")
		return rv.beta(_parameters[0],_parameters[1]);
	else if(_szDistribution=="exponential")
		return rv.exponential(_parameters[0]);
	else if(_szDistribution=="erlang")
		return rv.erlang(_parameters[0],_parameters[1]);
	else if(_szDistribution=="gamma")
		return rv.gamma(_parameters[0],_parameters[1]);
	else if(_szDistribution=="laplace")
		return rv.erlang(_parameters[0],_parameters[1]);
	else if(_szDistribution=="lognormal")
		return rv.lognormal(_parameters[0],_parameters[1]);	
	else if(_szDistribution=="normal")
		return rv.normal(_parameters[0],_parameters[1]);
	else if(_szDistribution=="poisson")
		return rv.poisson(_parameters[0]);
	else if(_szDistribution=="student")
		return rv.student(_parameters[0]);
	else if(_szDistribution=="triangular")
		return rv.triangular(_parameters[0],_parameters[1],_parameters[2]);
	else if(_szDistribution=="uniform")
		return rv.uniform(_parameters[0],_parameters[1]);
	else if(_szDistribution=="weibull")
	{
		double x = random.next_dbl();
		std::stringstream s;
		s<< "X = " << x << std::endl;
		OutputDebugString(s.str().c_str());
		// a*b*pow(x,b-1)*pow(M_E,-pow(a*x,b))

		//http://www.xycoon.com/wei_random.htm
		// RNG Wei(b,c): b*(pow(-log(x), 1/c)
		// ~ a*(pow(-log(x), 1/b)
		double a=_parameters[0];  // Lambda (1/beta) is the hazard rate. 
		double b=_parameters[1];  // alpha, the shape and beta, the scale, at 1/lambda

#if 0
		// F(t) = 1 – exp[-(t/a)b]   where "a" is the scale factor and "b" is the shape factor

		return (b/a) *pow( (x/a), b-1) * pow(M_E, - pow(x/a,b) );
		//boost::weibull_distribution(RealType shape, RealType scale = 1)
		boost::math::weibull_distribution<> _weibull(_parameters[0],_parameters[1]);
		//return boost::math::pdf(_weibull, x);
#endif
		return a*(pow(-log(x), 1/b));  //this worked
	}
	throw _T("No distribution set");
	return 0.0;
} 
// fixme check vector size
double Distribution::cdf(double x, LPCTSTR lpstrDistribution, std::vector<double> parameters )
{
	std::string szDistribution(lpstrDistribution);
	if(szDistribution=="constant")
	{
		// not continous
	}
	else if(szDistribution=="triangular")
	{
		boost::math::triangular_distribution<> _triangular(parameters[0],parameters[1],parameters[2]);
		return boost::math::cdf(_triangular, x);
	}
	else if(szDistribution=="normal")
	{
		boost::math::normal_distribution<> _normal(parameters[0],parameters[1]);
		return boost::math::cdf(_normal, x);
	}
	else if(szDistribution=="uniform")
	{
		boost::math::uniform_distribution<> _uniform(parameters[0],parameters[1]);
		return boost::math::cdf(_uniform, x);
	}
	else if(szDistribution=="poisson")
	{
		boost::math::poisson_distribution<> _poisson(parameters[0]);
		return boost::math::cdf(_poisson, x);
	}
	else if(szDistribution=="weibull")
	{
		//weibull_distribution(RealType shape, RealType scale = 1)
		boost::math::weibull_distribution<> _weibull(parameters[1],parameters[0]);
		return boost::math::cdf(_weibull, x);
	}
	else if(szDistribution=="lognormal")
	{
		boost::math::lognormal_distribution<> _lognormal(parameters[0],parameters[1]);
		return boost::math::cdf(_lognormal, x);
	}
	else if(szDistribution=="student")
	{
		boost::math::students_t_distribution<> _students_t(parameters[0]);
		return boost::math::cdf(_students_t, x);
	}
	else if(szDistribution=="beta")
	{
		boost::math::beta_distribution<> _beta(parameters[0],parameters[1]);
		return boost::math::cdf(_beta, x);
	}
	else if(szDistribution=="gamma")
	{
		boost::math::gamma_distribution<> _gamma(parameters[0],parameters[1]);
		return boost::math::cdf(_gamma, x);
	}
	else if(szDistribution=="erlang")
	{
		//boost::math::erlang_distribution<> _erlang(parameters[0],parameters[1]);
		//return boost::math::cdf(_erlang, x);
	}
	else if(szDistribution=="laplace")
	{
		boost::math::laplace_distribution<> _laplace(parameters[0],parameters[1]);
		return boost::math::cdf(_laplace, x);
	}
	else if(szDistribution=="exponential")
	{
		boost::math::exponential_distribution<> _exponential(parameters[0]);
		return boost::math::cdf(_exponential, x);
	}
	throw std::exception(_T("No distribution set"));
	return 0.0;
}
// fixme check vector size
double Distribution::pdf(double x, LPCTSTR lpstrDistribution, std::vector<double> parameters )
{
	std::string szDistribution(lpstrDistribution);
	if(szDistribution=="constant")
	{
		// not continous
	}
	else if(szDistribution=="beta")
	{
		boost::math::beta_distribution<> _beta(parameters[0],parameters[1]);
		return boost::math::pdf(_beta, x);
	}
	else if(szDistribution=="erlang")
	{
		//boost::math::erlang_distribution<> _erlang(parameters[0],parameters[1]);
		//return boost::math::pdf(_erlang, x);
	}
	else if(szDistribution=="exponential")
	{
		boost::math::exponential_distribution<> _exponential(parameters[0]);
		return boost::math::pdf(_exponential, x);
	}	
	else if(szDistribution=="gamma")
	{
		boost::math::gamma_distribution<> _gamma(parameters[0],parameters[1]);
		return boost::math::pdf(_gamma, x);
	}

	else if(szDistribution=="normal")
	{
		boost::math::normal_distribution<> _normal(parameters[0],parameters[1]);
		return boost::math::pdf(_normal, x);
	}

	else if(szDistribution=="laplace")
	{
		boost::math::laplace_distribution<> _laplace(parameters[0],parameters[1]);
		return boost::math::pdf(_laplace, x);
	}

	else if(szDistribution=="lognormal")
	{
		boost::math::lognormal_distribution<> _lognormal(parameters[0],parameters[1]);
		return boost::math::pdf(_lognormal, x);
	}

	else if(szDistribution=="poisson")
	{
		boost::math::poisson_distribution<> _poisson(parameters[0]);
		return boost::math::pdf(_poisson, x);
	}
	else if(szDistribution=="student")
	{
		boost::math::students_t_distribution<> _students_t(parameters[0]);
		return boost::math::pdf(_students_t, x);
	}


	else if(szDistribution=="triangular")
	{
		boost::math::triangular_distribution<> _triangular(parameters[0],parameters[1],parameters[2]);
		return boost::math::pdf(_triangular, x);

	}	else if(szDistribution=="uniform")
	{
		boost::math::uniform_distribution<> _uniform(parameters[0],parameters[1]);
		return boost::math::pdf(_uniform, x);
	}
	else if(szDistribution=="weibull")
	{
		boost::math::weibull_distribution<> _weibull(parameters[1],parameters[0]);
		return boost::math::pdf(_weibull, x);
	}	throw _T("No distribution set");
	return 0.0;
}

std::string Distribution::ToString()
{
	using namespace std;
	std::stringstream tmp;
	tmp.setf(ios::fixed,ios::floatfield);
	tmp.precision(2);
	tmp<< ( !_szDistribution.IsEmpty() ?  _szDistribution : "None"); 
	for(int j=0; j< _parameters.size(); j++)
	{
		if(j>=0 && _parameters[j]!= 0)
			tmp << " " << _parameters[j] ;
	}
	tmp<<_T("\n");

	return tmp.str();
}

Distribution StatFitting::BestFit(std::vector<double> & x)
{
	Distribution tmp;
	EstimateAll(x);
	GoodnessMap::iterator it,itmax=goodness.end();
	double max=0;
	for(it=goodness.begin();it!=goodness.end(); it++)
	{
		if((*it).second > max)
		{
			itmax=it; max=(*it).second;
		}
	}

	if(itmax!=goodness.end() && itmax->second > 0)
		return Distribution((*itmax).first.c_str(), stats.find((*itmax).first)->second);
	return tmp;
}
// trim from start 
static inline std::string &ltrim(std::string &str) 
{ 
	std::string::iterator i;
	for (i = str.begin(); i != str.end(); i++) {
		if (!isspace(*i)) {
			break;
		}
	}
	if (i == str.end()) {
		str.clear();
	} else {
		str.erase(str.begin(), i);
	}
	return str;
} 
#include <boost/lexical_cast.hpp> 
std::vector<double> StatFitting::GetData(std::string filename, std::string commentchar)
{
	std::ifstream file(filename.c_str());
	std::string line;
	std::vector<double> x;

	if(!file.is_open())
		return x;

	while(!file.eof())
	{
		getline(file, line);

		// Remove any comments
		size_t commIdx = line.find(commentchar);
		if(commIdx != std::string::npos)
			line = line.substr(0, commIdx);
		ltrim(line);
		if(line.size() <1)
			continue;
		double d;
		try{
		//d=::strtod( line.c_str(),0);
		d=boost::lexical_cast<double>(line); 
		x.push_back(d);
		}
		catch(...){}  // not a valid number skip
	}
	return x;
}

void StatFitting::ComputeWeibull(std::vector<double> & T, double &a, double &b)
 {
	 int i;
	 double N=(int) T.size();
	 std::vector<double> rank;
	 std::vector<double> logT;
	 std::vector<double> y;
	 std::vector<double> x;
	 
	 //i is the failure order number and N is the total sample size
	 double SumlogT=0, SumlogT2=0, SumY=0, SumY2=0, SumLogTY=0;
	 std::sort(T.begin(), T.end());
	 for(i=0; i<T.size(); i++)
	 {
		 rank.push_back(((i+1)-0.3)/(N+0.4));  // F(Ti) renumber 0..n to 1..n
		 y.push_back(log(-log(1-rank.back())));
		 x.push_back(log(T[i]));

		 SumlogT+=log(T[i]);
		 SumY+=y.back();
		 SumlogT2+=log(T[i])*log(T[i]);
		 SumY2+=y.back()*y.back();
		 SumLogTY+=log(T[i]) * y[i];

	 }


	 double bHat = (SumLogTY - ((SumlogT * SumY)/N))/(SumlogT2 - (SumlogT*SumlogT/N));
	 double aHat = (SumY/N) - (bHat * SumlogT / N);
	 double gammaHat = exp(-(aHat/bHat));

	 // compute linear regression correlation coefficient (-1,1)
	 double SumNumerator=0, SumXDenominator=0,SumYDenominator=0;
	 double xMean = accumulate(x.begin(), x.end(),0.0)/N;
	 double yMean = accumulate(y.begin(), y.end(),0.0)/N;
	 for(i=0; i< T.size(); i++)
	 {
		 SumNumerator+= (x[i]-xMean) * (y[i]-yMean);
		 SumXDenominator+= (x[i]-xMean) *  (x[i]-xMean);
		 SumYDenominator+= (y[i]-yMean) *  (y[i]-yMean);
	 }
	 double p = SumNumerator/sqrt(SumXDenominator*SumYDenominator);
	 a=gammaHat;
	 b=bHat;

 }