cmake_minimum_required(VERSION 3.15)
project(Chess)
set(CMAKE_CXX_STANDARD 17)

set(SDL_HEADER
	"include/SDL/Mouse.hpp"
	"include/SDL/SDLHandler.hpp"
	)

set(PIECES_HEADER
	"include/Pieces/Piece.hpp"
	"include/Pieces/Bishop.hpp"
	"include/Pieces/King.hpp"
	"include/Pieces/Knight.hpp"
	"include/Pieces/Pawn.hpp"
	"include/Pieces/Queen.hpp"
	"include/Pieces/Rook.hpp"
	)
	
set(GAMEMODES_HEADER
	"include/GameModes/Game.hpp"
	"include/GameModes/Chess.hpp"
	"include/GameModes/SwapChess.hpp"
	"include/GameModes/FogOfWarChess.hpp"
	)

set(CHESS_HEADER
	"include/GameLogic.hpp"
	"include/Board.hpp"
	"include/Renderer.hpp"
	"include/Utility.hpp"
	"include/Vec2D.hpp"
	)
	
set(SDL_SOURCE
	"src/SDL/Mouse.cpp"
	"src/SDL/SDLHandler.cpp"
	)
	
set(PIECES_SOURCE
	"src/Pieces/Piece.cpp"
	"src/Pieces/Bishop.cpp"
	"src/Pieces/King.cpp"
	"src/Pieces/Knight.cpp"
	"src/Pieces/Pawn.cpp"
	"src/Pieces/Queen.cpp"
	"src/Pieces/Rook.cpp"
	)
	
set(GAMEMODES_SOURCE
	"src/GameModes/Game.cpp"
	"src/GameModes/Chess.cpp"
	"src/GameModes/SwapChess.cpp"
	"src/GameModes/FogOfWarChess.cpp"
	)

set(CHESS_SOURCE
	"src/GameLogic.cpp"
	"src/Board.cpp"
	"src/Utility.cpp"
	"src/Renderer.cpp"
	)
	
set(HEADER
	${PIECES_HEADER}
	${SDL_HEADER}
	${CHESS_HEADER}
	${GAMEMODES_HEADER}
	${GAMEMODES_SOURCE}
	)
	
set(SOURCE
	${PIECES_SOURCE}
	${SDL_SOURCE}
	${CHESS_SOURCE}
	)
	
source_group("\\Chess\\Pieces" FILES ${PIECES_HEADER} ${PIECES_SOURCE})
source_group("\\SDL2" FILES ${SDL_HEADER} ${SDL_SOURCE})
source_group("\\Chess" FILES ${CHESS_HEADER} ${CHESS_SOURCE})
source_group("\\Chess\\GameModes" FILES ${GAMEMODES_HEADER} ${GAMEMODES_SOURCE})

add_library(Chess STATIC 
	${HEADER}
	${SOURCE}
	)

if (WIN32)
	set(SDL2_INCLUDE_DIRS $ENV{SDL2_INCLUDE})
	set(SDL2_LIBRARIES "$ENV{SDL2_BIN}/SDL2.lib")
	set(SDL2IMAGE_INCLUDE_DIRS $ENV{SDL2IMAGE_INCLUDE})
	set(SDL2IMAGE_LIBRARIES "$ENV{SDL2IMAGE_BIN}/SDL2_image.lib")
endif(WIN32)

target_include_directories(Chess PUBLIC "include" ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries(Chess ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})

if (MSVC)
    target_compile_options(Chess PRIVATE "/MP") # Enable multi threaded compilation
endif (MSVC)