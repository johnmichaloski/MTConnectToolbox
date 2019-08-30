//
// CrclInterface.h
//

#pragma once


/** 
	For crcl example xml files visit: 
	https://github.com/ros-industrial/crcl/blob/master/instances/statusExample.xml
*/
//#include "StdStringFcn.h"
#include <string>

//#include "DataPrimitives.hxx"
#include <boost/numeric/ublas/matrix.hpp>

class CStatus
{
public:
	typedef double StrVector[3];
	unsigned long long CommandID;
	unsigned long long StatusID;
	std::string CommandState;
	std::string Alarm;
	StrVector Point;
	StrVector XAxis;
	StrVector ZAxis;
	double roll, pitch, yaw;
	std::vector<double> joint_pos;
	std::vector<std::string> joint_names;
	double gripperpos;
	//bool GetRPY(VectorType Xrot, 
	//	VectorType Zrot,
	//	double & roll, 
	//	double & pitch, 
	//	double & yaw);
	void Dump()
	{
		std::cout << CommandID   << std::endl;
		std::cout << StatusID  << std::endl;
		std::cout << CommandState  << std::endl;
		std::cout << Point[0] << ":" ;
		std::cout << Point[1] << ":" ;
		std::cout << Point[2] << std::endl ;
		std::cout << XAxis[0] << ":" ;
		std::cout << XAxis[1] << ":" ;
		std::cout << XAxis[2] << std::endl ;
		std::cout << ZAxis[0] << ":" ;
		std::cout << ZAxis[1] << ":" ;
		std::cout << ZAxis[2] << std::endl ;
	}
};

class CrclInterface
{
public:
	CrclInterface() { _commandnum=0; }
	int ParseCRCLStatus(std::string filename);
	int ParseCRCLStatusString(std::string str);
	std::string SerializeCRCLGetStatusCmd(int cmdnum=0);
	std::string CRCLGetStatusCmd();
	std::string CRCLInitCanonCmd();

	CStatus _status;
	int _commandnum;
};