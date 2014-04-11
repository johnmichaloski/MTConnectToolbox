//
// adevs_rand.h


/***************
Copyright (C) 2000-2006 by James Nutaro

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Bugs, comments, and questions can be sent to nutaro@gmail.com
***************/
#ifndef __adevs_rand_h_
#define __adevs_rand_h_
//#include "adevs.h"
#include <cstdlib>

namespace adevs
{

typedef enum 
{
	NDURATION,
	NCOUNT,EMPTY,
	NDELAY,
	ADDTOQ,
	EMPTYQ,
	HNCLMS,
	HMODE,
	PROBVAL,
	ERRUNIFORM,
	ERRNORMAL,
	ERRLOGNORM,
	ERRTRIANG,
	ERRGAMMA,
	ERRBETA,
	ERREXPONENT,
	ERRERLANG,
	ERRHYPGEO,
	NULLEV,
	NOHISTO,
	INITERR,
	AMODE,
	HFORM,
	ERRFILE,
	SAMPLE,
	FRACTION,
	LEVEL,
	SCAN,
	SUPPRESS,
	SEED
} errorType;

/**
 * The random_seq class is an abstract interface to a random sequence
 * generator.
 */
class random_seq 
{
	public:
		/// Set the seed for the random number generator
		virtual void set_seed(unsigned long seed) = 0;
		/// Get the next double uniformly distributed in [0, 1]
		virtual double next_dbl() = 0;
		/// Copy the random number generator
		virtual random_seq* copy() const = 0;
		virtual unsigned long next_long() = 0;
		/// Destructor
		virtual ~random_seq(){}
};

/**
 * The mtrand class is a random number generator based on a GNU implementation
 * of the Mersenne Twister (see mtrand.cpp for more info).  Each copy
 * of the object has its own state, and so multiple copies
 * will produce independent random number streams.
 */
class mtrand: public random_seq 
{
	public:
		/// Create a generator with an unsigned long seed 
		mtrand(unsigned long seed = 1);
		/// Create a generator with an arbitrary length seed
		mtrand(const unsigned long* seed_array); 
		/// Copy constructor performs a deep copy
		mtrand(const mtrand& src);
		/// Assignment operator does a deep copy
		const mtrand& operator=(const mtrand& src);
		/// Set the seed
		void set_seed(unsigned long seed);
		/// Create a copy of this mtrand
		random_seq* copy() const;
		/// Get the next unsigned long value.
		unsigned long next_long();
		/// Get the next value in the range [0,1]
		double next_dbl();
		/// Destructor
		~mtrand ();
	private:	
		// Wrappers around the original function calls
		void sgenrand(unsigned long);
		void lsgenrand(const unsigned long*);
		unsigned long genrand();
		// The array for the state vector
		unsigned long* mt;
		int mti;
		unsigned long init_seed;
};

/**
 * The crand class provides random number sequences using the standard
 * C rand() function.  Since the underlying stream generator is rand(),
 * the state of the random number generator can not be captured with
 * a call to copy.  Also, each instance of the object samples the same
 * random number sequence.
 */
class crand: public random_seq 
{
	public:
		/// Create a generator with the default seed
		crand(){}
		/// Create a generator with the given seed
		crand(unsigned long seed) { srand (seed); }
		/// Set the seed for the random number generator
		void set_seed(unsigned long seed) { srand (seed); }
		/// Get the next double uniformly distributed in [0, 1]
		double next_dbl() { return (double)rand()/(double)RAND_MAX; } 
		/// Copy the random number generator
		unsigned long next_long() { return (unsigned long)rand(); }

		random_seq* copy() const { return new crand (); }
		/// Destructor
		~crand(){}
};

/**
 * The rv class provides a random variable based on a selectable
 * implementation.  By default, this implementation is the Mersenne
 * Twister (see mtrand).
 */
class rv 
{
	public:
		/// Create a random variable with the default implementation.
		rv (unsigned long seed = 1);
		/**
		 * Create a random variable with the desired implementation.  The
		 * implementation class is adopted by the rv.
		 */
		rv(random_seq* rand);
		/// Copy constructor relies on copy method of underlying stream.
		rv(const rv& src);
		/// Assignment operator relies on copy method of underlying stream.
		const rv& operator=(const rv& src);
		/// See the random number generator implementation
		void set_seed(unsigned long seed);
		/// Get a raw value from the underlying random number generator
		unsigned long next_long();
		/// Sample a triangular distribution with (a, b, c)
		double triangular(double a, double b, double c);
		/// Sample a uniform distribution in the range [a, b]
		double uniform(double a, double b);
		/**
		 * Sample a normally distributed random variable with mean m and 
		 * standard deviation s.
		 */
		double normal(double m, double s);
		/**
		 * An assortment of other random variable types contributed by
		 * Alex Cave (who, at the time, was with the Intelligent 
		 * Systems Automation Group
		 * in the School of Engineering at Deakin University).
		 */
		double exponential(double a);
		double hyperexponential(double p,double a,double b);
		double laplace(double a);
		double chisquare(unsigned int n);
		double student(unsigned int n);
		double lognormal(double a,double b);
		double erlang(unsigned int n,double a);
		double gamma(double a,double b);
		double beta(double a,double b);
		double fdistribution(unsigned int n,unsigned int m);
		double poisson(double a);
		double geometric(double p);
		double hypergeometric(unsigned int m,unsigned int n,double p);
		double weibull(double a,double b);
		double binomial(double p,unsigned int n);
		double negativebinomial(double p,unsigned int n);
		double triangular(double a);
		int probability(double p);
		double lngamma(double xx);
		/// Destructor
		~rv();
	private:	
		random_seq* _impl;
		void err(errorType n);
};

} // end of namespace

#endif


