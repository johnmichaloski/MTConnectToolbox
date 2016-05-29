//
// Utils.h
//

#pragma once
#include <vector>
#include <string>
class Utils
{
public:
	Utils(void);
	~Utils(void);
	static std::vector<std::string> FileList(std::string folder);
	static void TrimExtension(std::vector<std::string>& strings, std::string extension);
};
