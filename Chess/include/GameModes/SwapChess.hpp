#pragma once

#include "Chess.hpp"

namespace chess
{
	class SwapChess : public Game
	{
	public:
		SwapChess(const std::string& fenString = defaultStartPos);

		virtual void enableRendering() override;
		virtual bool update() override; // Returns true if the window is still open
		virtual bool isMovePossible(const Move& move) const override;
		virtual void makeMove(const Move& move) override;
		virtual bool isGameOver() const override;
		virtual void setPreviousMove(const Move& move) override;
	private:
		bool isCheckMate(const Board& board, PieceColor color) const;
		bool isStaleMate(const Board& board, PieceColor color) const;
		void updateRenderInfo();

		RenderInformation m_renderInfo;
		Renderer m_renderer;
	};
}