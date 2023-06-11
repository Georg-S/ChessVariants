#include "Utility.hpp"

std::vector<std::string> chess::stringSplit(std::string str, const std::string& delimiter)
{
	std::vector<std::string> result;
	size_t pos = 0;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		result.emplace_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.length());
	}
	result.emplace_back(str);

	return result;
}
