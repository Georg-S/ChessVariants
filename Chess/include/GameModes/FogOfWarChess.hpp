#pragma once

#include "Game.hpp"

namespace chess
{
	struct FogOfWarRenderInformation : public RenderInformation
	{
		PieceColor toRenderColor;
	};

	class FogOfWarRenderer : public Renderer 
	{
	public:
		void render(const FogOfWarRenderInformation& renderInfo);
	private:
		void renderFogOfWarGameState(const FogOfWarRenderInformation& renderInfo);
	};


	class FogOfWarChess : public Game
	{
	public:
		FogOfWarChess(PieceColor playerColor);
		virtual void enableRendering() override;
		virtual bool update() override; // Should return true if the window is still open
		virtual bool isMovePossible(const Move& move) const override;
		virtual void makeMove(const Move& move) override;
		virtual bool isGameOver() const override;
		virtual void setPreviousMove(const Move& move) override;

	private:
		void updateRenderInfo();

		FogOfWarRenderer m_renderer;
		PieceColor m_renderColor;
		FogOfWarRenderInformation m_renderInfo;
	};
}