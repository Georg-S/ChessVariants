#include "Utility.hpp"

using namespace chess;

std::string chess::toChessPositionString(const Position& pos)
{
	Position convPos = pos;

	convPos.y = 8 - pos.y; // Convert from internal board coordinates to "official" coordinates
	char column = 'a' + convPos.x;
	std::string rowStr = std::to_string(convPos.y);

	return column + rowStr;
}

std::optional<Position> chess::toPositionFromChessString(const std::string& str)
{
	Position result = { -1, -1 };
	for (char c : str)
	{
		c = tolower(c);
		if (c >= 'a' && c <= 'h')
			result.x = c - 'a';
		if (c >= '1' && c <= '8')
			result.y = '8' - c;	// Convert from "official" coordinates to internal board coordinates
	}

	if (result.x == -1 || result.y == -1)
		return std::nullopt;

	return result;
}

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
