#define NOMINMAX
#include <fstream>
#include <algorithm>
#include <GameModes/Chess.hpp>
#include <GameModes/FogOfWarChess.hpp>
#include <GameModes/SwapChess.hpp>
#include <GameModes/TrapChess.hpp>

#include "Button.hpp"

#undef main

static std::vector<std::unique_ptr<chess::Game>> createGames(chess::GAME_MODES gameMode) 
{
	std::vector<std::unique_ptr<chess::Game>> result;

	switch (gameMode)
	{
	case chess::GAME_MODES::NORMAL:
		result.emplace_back(std::make_unique<chess::Chess>());
		return result;
	case chess::GAME_MODES::SWAP:
		result.emplace_back(std::make_unique<chess::SwapChess>());
		return result;
	case chess::GAME_MODES::TRAP:
		result.emplace_back(std::make_unique<chess::TrapChess>(chess::PieceColor::NONE));
		result.emplace_back(std::make_unique<chess::TrapChess>(chess::PieceColor::WHITE));
		result.emplace_back(std::make_unique<chess::TrapChess>(chess::PieceColor::BLACK));
		return result;
	case chess::GAME_MODES::FOGOFWAR:
		result.emplace_back(std::make_unique<chess::FogOfWarChess>(chess::PieceColor::NONE));
		result.emplace_back(std::make_unique<chess::FogOfWarChess>(chess::PieceColor::WHITE));
		result.emplace_back(std::make_unique<chess::FogOfWarChess>(chess::PieceColor::BLACK));
		return result;
	}
	return result;
}


std::vector<std::string> getGameStates(const std::string& filePath) 
{
	std::vector<std::string> result;
	try
	{
		std::ifstream file(filePath);
		std::string line;
		while (std::getline(file, line))
			result.emplace_back(line);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error reading file: " << filePath << " " << e.what() << std::endl;
	}

	return result;
}


int main() 
{
	const std::string gameToLoad = "3_TrapChess_loss.txt";
	const std::string gameToLoadPath = "Games_with_Chris/" + gameToLoad;
	const auto gameStates = getGameStates(gameToLoadPath);
	auto splitted = chess::stringSplit(gameToLoad, "_");
	const auto gameMode = chess::gameModeFromString(splitted[1]);

	int currentGameStatesIndex = 0;
	if (gameStates.empty() || (gameMode == chess::GAME_MODES(-1)))
	{
		std::cout << "No valid gamestates found" << std::endl;
		getchar();
		return 0;
	}

	auto games = createGames(gameMode);
	for (auto& game : games) 
	{
		game->enableRendering();
		game->setGameState(gameStates.front());
		game->update();
	}

	Button leftArrow = Button("Left arrow", VK_LEFT);
	Button rightArrow = Button("Left arrow", VK_RIGHT);

	auto moveForward = [&gameStates, &games, &currentGameStatesIndex]() -> void
	{
		currentGameStatesIndex = std::min(currentGameStatesIndex + 1, static_cast<int>(gameStates.size() - 1));
		for (auto& game : games) 
		{
			game->setGameState(gameStates[currentGameStatesIndex]);
			game->update();
		}
	};

	auto moveBackward = [&gameStates, &games, &currentGameStatesIndex]() -> void
	{
		currentGameStatesIndex = std::max(0, currentGameStatesIndex - 1);
		for (auto& game : games) 
		{
			game->setGameState(gameStates[currentGameStatesIndex]);
			game->update();
		}
	};

	leftArrow.setOnClickCallback(moveBackward);
	rightArrow.setOnClickCallback(moveForward);

	while (true) 
	{
		leftArrow.update();
		rightArrow.update();
		for (auto& game : games) 
			game->update();
	}

	return 0;
}