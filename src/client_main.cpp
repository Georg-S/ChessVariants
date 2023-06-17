#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <TCPClient.hpp>

#include <SDL/SDLHandler.hpp>
#include <Board.hpp>
#include <Renderer.hpp>
#include <Game.hpp>
#include <GameLogic.hpp>
#include "ChessClient.hpp"


using namespace boost;

#undef main

int main(int argc, char* argv[])
{
	//const std::string startPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	//const std::string testPositionCanBeDeleted = "4k3/3pr3/8/4P3/8/8/8/R3K2R w KQ - 0 1";

	//chess::Game game = chess::Game(testPositionCanBeDeleted);

	//std::cout << std::is_trivial<chess::Board>::value << std::endl;
	//std::cout << std::is_trivially_copyable<chess::Board>::value << std::endl;

	//while (game.update()) 
	//{
	//	Sleep(1);

	//	if (game.isInPromotion()) 
	//	{
	//		auto selectedPiece = game.getSelectedPromotionPosition();
	//		if (selectedPiece)
	//			game.selectPieceForPromotion(*selectedPiece);

	//		continue;
	//	}

	//	auto selectedPosition = game.getSelectedBoardPosition();
	//	if (!selectedPosition)
	//		continue;

	//	if (game.isPieceSelected()) 
	//		game.makeMoveWithSelectedPiece(*selectedPosition);
	//	else 
	//		game.selectPiece(*selectedPosition);

	//	//Sleep(1);
	//	//game.update();
	//	//auto pos = game.getSelectedPosition();
	//	//if (pos) 
	//	//	std::cout << "X: " << pos->x << " Y: " << pos->y << std::endl;

	//}
	ChessClient client = ChessClient();
	client.run();


	return 0;
}