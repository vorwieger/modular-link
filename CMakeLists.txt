cmake_minimum_required(VERSION 3.0)

# Toolchain
include_directories(/usr/include)
set(CMAKE_FIND_ROOT_PATH ${TARGET_DIR})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project
project(ModularLink)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Os -Wno-psabi")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(modules/link/AbletonLinkConfig.cmake)
include_directories(${link_HEADERS})

set(SRC
  src/main.cpp
  src/Settings.cpp
  src/Engine.cpp
  src/Max7219.cpp
  src/Display.cpp
  src/Input.cpp
  src/Output.cpp
)
add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} atomic pthread config++)
target_link_libraries(${PROJECT_NAME} Ableton::Link)
target_link_libraries(${PROJECT_NAME} wiringPi)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "modlink")
