cmake_minimum_required(VERSION 3.15)
project(Chess)
set(CMAKE_CXX_STANDARD 17)

set(HEADER
	"include/GameLogic.hpp"
	"include/Board.hpp"
	"include/Utility.hpp"
	"include/SDL/Mouse.hpp"
	"include/SDL/SDLHandler.hpp"
	"include/Pieces/Piece.hpp"
	"include/Pieces/Bishop.hpp"
	"include/Pieces/King.hpp"
	"include/Pieces/Knight.hpp"
	"include/Pieces/Pawn.hpp"
	"include/Pieces/Queen.hpp"
	"include/Pieces/Rook.hpp"
	)

set(SOURCE
	"src/GameLogic.cpp"
	"src/Board.cpp"
	"src/Utility.cpp"
	"src/SDL/Mouse.cpp"
	"src/SDL/SDLHandler.cpp"
	"src/Pieces/Piece.cpp"
	"src/Pieces/Bishop.cpp"
	"src/Pieces/King.cpp"
	"src/Pieces/Knight.cpp"
	"src/Pieces/Pawn.cpp"
	"src/Pieces/Queen.cpp"
	"src/Pieces/Rook.cpp"
	)

#source_group("\\Pieces" FILES ${Pieces})

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