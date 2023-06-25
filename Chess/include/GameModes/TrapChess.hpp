#pragma once

#include <set>
#include "Game.hpp"
#include "Renderer.hpp"

namespace chess 
{
	struct TrapChessRenderingInformation :public RenderInformation
	{
		std::vector<Position> bombPositions;
	};

	class TrapChessRenderer : public Renderer 
	{
	public:
		void render(const TrapChessRenderingInformation& renderInfo);
	private:
		void renderTrapChess(const TrapChessRenderingInformation& renderInfo);
		void renderBombsExceptSelected(const std::vector<Position>& bombPositions, std::optional<Position> selected = {});
		void renderBomb(const Position& pos);
		void renderBomb(int windowX, int windowY);
		void renderPiecesWithSelectedOnMousePositionWithBombs(const TrapChessRenderingInformation& renderInfo);
	};


	class TrapChess : public Game 
	{
	public:
		TrapChess(PieceColor playerColor);

		virtual void enableRendering() override;
		virtual bool update() override; // Should return true if the window is still open
		virtual bool isMovePossible(const Move& move) const override;
		virtual void makeMove(const Move& move) override;
		virtual bool isGameOver() const override;
		virtual void setPreviousMove(const Move& move) override;

		virtual bool isGameReady() const override;
		void placeBomb(const Position& pos, PieceColor color);

	private:
		void updateRenderInfo();

		static constexpr int MaxAllowedBombsPerPlayer = 3;
		std::array<std::array<bool, BOARD_HEIGHT>, BOARD_WIDTH> m_blackPlayerBomb = {};
		std::array<std::array<bool, BOARD_HEIGHT>, BOARD_WIDTH> m_whitePlayerBomb = {};
		PieceColor m_playerPerspective;
		bool m_gameReady = false;
		TrapChessRenderer m_renderer;
		TrapChessRenderingInformation m_renderInfo;
	};
}