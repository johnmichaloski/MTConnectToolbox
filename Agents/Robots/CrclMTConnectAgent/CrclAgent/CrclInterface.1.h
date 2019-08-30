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
	void ParseCRCLStatus(std::string filename);
	void ParseCRCLStatusString(std::string str);
	void SerializeCRCLStatus(std::string filename);
	std::string SerializeCRCLGetStatusCmd(int cmdnum=0);
	std::string CRCLGetStatusCmd();
	std::string CRCLInitCanonCmd();

	CStatus _status;
	int _commandnum;
};