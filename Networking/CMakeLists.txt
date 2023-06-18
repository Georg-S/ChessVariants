cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
project(Networking)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)


set(HEADERS 
	"include/TCPServer.hpp"
	"include/TCPClient.hpp"
	"include/Message.hpp"
	"include/Utility.hpp"
	"include/ThreadSafeQueue.hpp"
	)

set(SOURCES 
	"src/TCPServer.cpp"
	"src/TCPClient.cpp"
	"src/Utility.cpp"
	)

if (MSVC)
	set(BOOST_ROOT $ENV{BOOST_ROOT})
endif (MSVC)

find_package(Boost REQUIRED ${BOOST_MIN_VERSION})

#source_group("" FILES ${miscFiles})

add_library(Networking STATIC ${SOURCES} ${HEADERS})
set_target_properties(Networking PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Networking PUBLIC "include" ${Boost_INCLUDE_DIRS})
target_link_libraries(Networking ${Boost_LIBRARIES})