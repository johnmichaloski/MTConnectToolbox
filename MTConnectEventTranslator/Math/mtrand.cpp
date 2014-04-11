/* A C-program for MT19937: Integer version (1999/10/28)          */
/*  genrand() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. sgenrand(seed) sets initial values to the working area   */
/* of 624 words. Before genrand(), sgenrand(seed) must be         */
/* called once. (seed is any 32-bit integer.)                     */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */

/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997, 1999 Makoto Matsumoto and Takuji Nishimura. */
/* When you use this, send an email to: matumoto@math.keio.ac.jp   */
/* with an appropriate reference to your work.                     */

/* REFERENCE                                                       */
/* M. Matsumoto and T. Nishimura,                                  */
/* "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform  */
/* Pseudo-Random Number Generator",                                */
/* ACM Transactions on Modeling and Computer Simulation,           */
/* Vol. 8, No. 1, January 1998, pp 3--30.                          */

/**
The following is a C++ implementation of Nishumura's random number
generator.  Each mtrand object has its own state variables.  The
assignment and copy operators can be used to make deep copies
of the random number generator state.  
*/
#include "stdafx.h"
#include "adevs_rand.h"

using namespace adevs;
 
#define ULONG_MAX_32BIT 0xffffffffUL

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

/* mtrand constructor.  Initializes the array with a seed.  */
mtrand::mtrand(unsigned long seed) 
{
	mt = new unsigned long [N];
	// mti==N+1 means mt[N] is not initialized
	mti = N + 1;
	sgenrand(seed);
}

/* mtrand constructor.  Initializes the array with an array
   of seeds.
*/
mtrand::mtrand(const unsigned long* seed_array) 
{
	mt = new unsigned long [N];
	// mti==N+1 means mt[N] is not initialized
	mti = N + 1;
	lsgenrand(seed_array);
}

/* mtrand copy constructor */
mtrand::mtrand(const mtrand& src)
{
	mt = new unsigned long [N];
	for (int i = 0; i < N; i++)
		mt[i] = src.mt[i];
	mti = src.mti;
}

/* Assignment operator */
const mtrand&
mtrand::operator=(const mtrand& src)
{
	for (int i = 0; i < N; i++)
		mt[i] = src.mt[i];
	mti = src.mti;
	return *this;
}

random_seq*
mtrand::copy() const 
{
	return new mtrand(*this);
}

/* Return the next unsigned long integer. */
unsigned long 
mtrand::next_long() 
{
	return genrand();
}

/* Return the next double in the interval [0, 1] */
double 
mtrand::next_dbl() 
{
	unsigned long result = genrand ();
	return ((double)result / (double)ULONG_MAX_32BIT);
}

void
mtrand::set_seed(unsigned long seed) 
{
	sgenrand (seed);
}

/* Destructor */
mtrand::~mtrand() 
{
	delete [] mt;
}

/* Initializing the array with a seed */

void
mtrand::sgenrand(unsigned long seed)
{
    int i;

    for (i=0;i<N;i++) {
         mt[i] = seed & 0xffff0000;
         seed = 69069 * seed + 1;
         mt[i] |= (seed & 0xffff0000) >> 16;
         seed = 69069 * seed + 1;
    }
    mti = N;
}

/* Initialization by "sgenrand()" is an example. Theoretically,      */
/* there are 2^19937-1 possible states as an intial state.           */
/* This function allows to choose any of 2^19937-1 ones.             */
/* Essential bits in "seed_array[]" is following 19937 bits:         */
/*  (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1]. */
/* (seed_array[0]&LOWER_MASK) is discarded.                          */ 
/* Theoretically,                                                    */
/*  (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1]  */
/* can take any values except all zeros.                             */
void
mtrand::lsgenrand(const unsigned long* seed_array)
    /* the length of seed_array[] must be at least N */
{
    int i;

    for (i=0;i<N;i++) 
      mt[i] = seed_array[i];
    mti=N;
}

unsigned long 
mtrand::genrand()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if sgenrand() has not been called, */
            sgenrand(4357); /* a default initial seed is used   */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }
  
    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

	if( y == 0)
	{
		// Make it impossible to return zero to elimiate divide by zero errors
		y = 1;
	}

    return y; 
}
