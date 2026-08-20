cmake_minimum_required(VERSION 3.16)
project(WormholeGateway LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WormholeGateway
	src/main.cpp
	src/Wormhole.cpp
	src/physics.c
	src/utils.c
	src/logger.c
	src/visualization.c
	gui/GuiManager.cpp
	gui/Renderer.cpp
)

target_include_directories(WormholeGateway PRIVATE include src gui)

if (WIN32)
	target_link_libraries(WormholeGateway PRIVATE user32 gdi32 winmm)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_link_options(WormholeGateway PRIVATE -mwindows)
	endif()
elseif (UNIX AND NOT APPLE)
	target_link_libraries(WormholeGateway PRIVATE m)
endif()
