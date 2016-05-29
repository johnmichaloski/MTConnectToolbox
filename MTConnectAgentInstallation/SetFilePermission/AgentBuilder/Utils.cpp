//
// Utils.cpp
//

#include "StdAfx.h"
#include "Utils.h"
#include <sys/types.h>

#include <errno.h>
#include <iostream>
#include <sstream>
#include "dirent.h"
#include "StdStringFcn.h"

Utils::Utils(void)
{
}

Utils::~Utils(void)
{
}

std::vector<std::string> Utils::FileList(std::string folder)
{
	std::vector<std::string> files;

	DIR *dp;
	struct dirent *dirp;
	if((dp = opendir(folder.c_str())) == NULL) 
	{
		std::stringstream ss (std::stringstream::in);
		ss<< "Error(" << errno << ") opening " << folder << std::endl;
		throw ss.str();
	}

	while ((dirp = readdir(dp)) != NULL) 
	{
		if(dirp->d_name[0]=='.')
			continue;
		files.push_back(std::string(dirp->d_name));
	}
	closedir(dp);
	return files;
}



void Utils::TrimExtension(std::vector<std::string>&strs, std::string extension)
{
	for(int i=0; i< strs.size(); i++)
	{
		strs[i]=ReplaceOnce(strs[i],extension, ""); 
	}
}
