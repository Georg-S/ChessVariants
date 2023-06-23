#pragma once

#include "Chess.hpp"

namespace chess
{
	class SwapChess : public chess::Chess
	{
		virtual bool isMovePossible(const Move& move) const override;
		virtual void makeMove(const Move& move) override;
	};
}