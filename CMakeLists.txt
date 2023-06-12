cmake_minimum_required (VERSION 3.8)
project(SpecialChess)
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CHESS_SERVER_HEADERS "")

set(CHESS_SERVER_SOURCES "src/server_main.cpp")

set(CHESS_CLIENT_HEADERS "")

set(CHESS_CLIENT_SOURCES "src/client_main.cpp")

#source_group("" FILES ${miscFiles})

add_subdirectory(Networking)
add_subdirectory(Chess)
#Server
add_executable(ChessServer ${CHESS_SERVER_HEADERS} ${CHESS_SERVER_SOURCES})
target_include_directories(ChessServer PUBLIC "include")
target_link_libraries(ChessServer Networking)
#Client
add_executable(ChessClient ${CHESS_CLIENT_HEADERS} ${CHESS_CLIENT_SOURCES})
target_include_directories(ChessClient PUBLIC "include")
target_link_libraries(ChessClient Networking Chess)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ChessServer)

if (MSVC)
    target_compile_options(Chess PRIVATE "/MP") # Enable multi threaded compilation
	#   copy dlls next to exe for windows (visual studio)
    add_custom_command(TARGET ChessServer
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "$ENV{SDL2_BIN}/SDL2.dll"
                        $<TARGET_FILE_DIR:ChessServer>)

    file(GLOB SDL2_IMAGE_DLLS "$ENV{SDL2IMAGE_BIN}/*.dll")
    add_custom_command(TARGET ChessServer
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL2_IMAGE_DLLS}
                        $<TARGET_FILE_DIR:ChessServer>)
						
	add_custom_command(TARGET ChessServer
						POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy_directory
						"${CMAKE_SOURCE_DIR}/Images"
						"$<TARGET_FILE_DIR:ChessServer>/Images")
		
	add_custom_command(TARGET ChessClient
					POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different
					"$ENV{SDL2_BIN}/SDL2.dll"
					$<TARGET_FILE_DIR:ChessClient>)

    file(GLOB SDL2_IMAGE_DLLS "$ENV{SDL2IMAGE_BIN}/*.dll")
    add_custom_command(TARGET ChessClient
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${SDL2_IMAGE_DLLS}
                        $<TARGET_FILE_DIR:ChessClient>)
	
	add_custom_command(TARGET ChessClient
						POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy_directory
						"${CMAKE_SOURCE_DIR}/Images"
						"$<TARGET_FILE_DIR:ChessClient>/Images")
endif (MSVC)