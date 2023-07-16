cmake_minimum_required (VERSION 3.8)
project(GameReplay)
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(HEADERS
	"include/Button.hpp"
)

set(SOURCES
	"src/Button.cpp"
	"src/main.cpp"
)

add_executable(GameReplay ${HEADERS} ${SOURCES})
target_include_directories(GameReplay PUBLIC "include")
target_link_libraries(GameReplay Chess)

if (MSVC)
    target_compile_options(GameReplay PRIVATE "/MP") # Enable multi threaded compilation
	add_definitions(-D_WIN32_WINNT=0x0601)
	#   copy dlls next to exe for windows (visual studio)
    add_custom_command(TARGET GameReplay
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "$ENV{SDL2_BIN}/SDL2.dll"
                        $<TARGET_FILE_DIR:GameReplay>)

    file(GLOB SDL2_IMAGE_DLLS "$ENV{SDL2IMAGE_BIN}/*.dll")
    add_custom_command(TARGET GameReplay
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL2_IMAGE_DLLS}
                        $<TARGET_FILE_DIR:GameReplay>)
						
	add_custom_command(TARGET GameReplay
						POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy_directory
						"${CMAKE_SOURCE_DIR}/Images"
						"$<TARGET_FILE_DIR:GameReplay>/Images")
endif (MSVC)