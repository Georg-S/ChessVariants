#pragma once
#include <string>
#include <vector>
#include <optional>

#include "Vec2D.hpp"

namespace chess
{
	static const std::string defaultStartPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	using Position = Vec2D<int>;

	struct Move
	{
		Position from = {};
		Position to = {};
	};

	std::string toChessPositionString(const Position& pos);
	std::optional<Position> toPositionFromChessString(const std::string& str);
	std::vector<std::string> stringSplit(std::string str, const std::string& delimiter);
}
