#pragma once
#include <memory>
#include <optional>

#include "Board.hpp"
#include "SDL/SDLHandler.hpp"

namespace chess 
{
	static const std::string imagesPath = "Images/";
	constexpr int WINDOW_WIDTH = 800;
	constexpr int WINDOW_HEIGHT = 800;

	struct RenderInformation
	{
		chess::Board board;
		std::optional<PieceColor> promotionSelectionColor;
		std::optional<Position> positionToRenderOnMousePosition;
		std::optional<Move> previousMove;
		std::optional<Position> mousePos;
		std::optional<chess::PieceColor> playerWon;
	};

	class Renderer
	{
	public:
		Renderer();
		virtual ~Renderer() = default;
		virtual void start(const std::string& windowName);
		virtual void renderBoard(const std::string& fenBoardString);
		virtual void renderBoard(const chess::Board& board);
		virtual void render(const RenderInformation& renderInfo);
		virtual bool isQuit() const;
		virtual void close();

	protected:
		void renderNormalGameState(const RenderInformation& renderInfo);
		void renderChessBoard();
		void renderPieces(const chess::Board& board);
		void renderPiece(char fenChar, const Position& pos);
		void renderPreviousMove(const chess::Move& previousMove);
		void renderPiecesWithSelectedOnMousePosition(const Board& board, const Position& mousePos, const Position& selectedPiece);
		void render_piece_on_mouse_position(char piece, const Position& mousePos);
		void renderAllPossibleMovesForSelectedPiece(const chess::Board& board, const chess::Position& selectedPos);
		void render_promotion_selection(chess::PieceColor color);
		void renderGameOver(chess::PieceColor playerWon);
		std::string getPieceFileString(char pieceChar, PieceColor color) const;
		std::string getPieceFileString(char fenChar) const;
		std::string getPieceTypeString(char fenChar) const;
		std::string getColorString(chess::PieceColor color) const;

		bool m_running = false;
		bool m_renderPreviousMove = true;
		static constexpr int PIECE_WIDTH = WINDOW_WIDTH / chess::BOARD_WIDTH;
		static constexpr int PIECE_HEIGHT = WINDOW_HEIGHT / chess::BOARD_HEIGHT;
		std::unique_ptr<SDLHandler> m_sdlHandler = nullptr;
	};
}