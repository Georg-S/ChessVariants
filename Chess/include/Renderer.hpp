#pragma once
#include <memory>
#include <optional>

#include "Board.hpp"
#include "SDL/SDLHandler.hpp"

namespace chess {

	constexpr int WINDOW_WIDTH = 800;
	constexpr int WINDOW_HEIGHT = 800;

	struct RenderInformation
	{
		chess::Board board;
		std::optional<PieceColor> promotionSelectionColor;
		std::optional<Position> positionToRenderOnMousePosition;
		std::optional<Move> previousMove;
		std::optional<Position> mousePos;
	};

	class Renderer
	{
	public:
		Renderer();
		void start();
		void renderBoard(const std::string& fenBoardString);
		void renderBoard(const chess::Board& board);
		void render(const RenderInformation& renderInfo);
		bool isQuit() const;
		void close();

	private:
		void renderNormalGameState(const RenderInformation& renderInfo);
		void renderChessBoard();
		void renderPieces(const chess::Board& board);
		void renderPiece(char fenChar, const Position& pos);
		void renderPreviousMove(const chess::Move& previousMove);
		void renderPiecesWithSelectedOnMousePosition(const Board& board, const Position& mousePos, const Position& selectedPiece);
		void render_piece_on_mouse_position(char piece, const Position& mousePos);
		void renderAllPossibleMovesForSelectedPiece(const chess::Board& board, const chess::Position& selectedPos);
		void render_promotion_selection(chess::PieceColor color);
		//void render_checkmate();
		//void render_stalemate();
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