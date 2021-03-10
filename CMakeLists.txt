cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(DANAS-VC)

file(GLOB SOURCE
	${PROJECT_SOURCE_DIR}/src/*.cpp
	${PROJECT_SOURCE_DIR}/src/*.h
)

add_executable(${PROJECT_NAME} ${SOURCE})

if(WIN32)

elseif(UNIX)
	target_compile_options(${PROJECT_NAME} PRIVATE -llua -lSDL2 -Wall -Wextra)

	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIRS})

	find_package(Lua 5.3 REQUIRED)
	include_directories(${LUA_INCLUDE_DIRS})

	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${LUA_LIBRARIES})
endif()
