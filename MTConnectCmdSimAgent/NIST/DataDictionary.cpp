//
// DataDictionary.cpp
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "stdafx.h"
#include "DataDictionary.h"
#include "StdStringFcn.h"

std::string String(std::vector<std::string> &list)
{
	std::string tmp;
	for(int i=0; i<list.size(); i++) 
	{
		if(i>0) tmp+=",";
		tmp+=list[i];
	}
	return tmp;
}


void DataDictionary::Update(double seconds)
{
	if(this->find("Srpm")!=this->end())
	{
		int srpm = ConvertString<int>(this->operator[]("Srpm"), 0);
		accumulations["Srpm"]+= srpm * seconds/60.0 ; // number of revolutions
	}
	// add loads

}
