#pragma once
#include <memory>

#include "Board.hpp"
#include "SDL/SDLHandler.hpp"

namespace chess {

	constexpr int WINDOW_WIDTH = 800;
	constexpr int WINDOW_HEIGHT = 800;

	class Renderer
	{
	public:
		Renderer();
		//void render_promotion_selection(chess::PieceColor color); TODO
		void renderBoard(const std::string& fenBoardString);
		void renderBoard(const chess::Board& board);
		bool isQuit() const;
		void close();

	private:
		void renderChessBoard();
		void renderPieces(const chess::Board& board);
		void renderPiece(char fenChar, int x, int y);
		//void render_pieces_with_selected_on_mouse_position(const RenderInformation& renderInfo);
		//void render_previous_move(const ceg::Move& previousMove);
		//void render_piece_on_mouse_position(char piece, int mouseX, int mouseY);
		//void render_all_possible_moves_for_selected_piece(const ceg::BitBoard& board, int selected_x, int selected_y);
		//void render_checkmate();
		//void render_stalemate();
		std::string getPieceFileString(char fenChar) const;
		std::string getPieceTypeString(char fenChar) const;
		std::string getColorString(chess::PieceColor color) const;

		static constexpr int PIECE_WIDTH = WINDOW_WIDTH / chess::BOARD_WIDTH;
		static constexpr int PIECE_HEIGHT = WINDOW_HEIGHT / chess::BOARD_HEIGHT;
		std::unique_ptr<SDLHandler> m_sdlHandler = nullptr;
	};
}