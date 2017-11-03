
//
// RCSMutexVar.h
//

/*
DISCLAIMER:
This software was produced by the National Institute of Standards
and Technology (NIST), an agency of the U.S. government, and by statute is
not subject to copyright in the United States.  Recipients of this software
assume all responsibility associated with its operation, modification,
maintenance, and subsequent redistribution.

See NIST Administration Manual 4.09.07 b and Appendix I.
*/

#ifndef __RCSMutexVar_h
#define __RCSMutexVar_h

//#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>


namespace RCS
{
	template <typename T=bool>
	class MutexVariable
	{
	public:
		MutexVariable() 
		{ 
			mVar=0; 
//			mMutex= new boost::shared_mutex();
		}

		void set(T val=1)
		{
			boost::unique_lock< boost::shared_mutex > lock(mMutex);
			mVar=val;
		}
		T get()
		{
			// get shared access
			boost::shared_lock< boost::shared_mutex > lock(mMutex);
			T tmp;
			tmp=mVar;
			return tmp;
		}

	protected:
		boost::shared_mutex  mMutex;
		T mVar;
	};

}
#endif