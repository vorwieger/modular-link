cmake_minimum_required(VERSION 3.0)
project(ModularLink)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(modules/link/AbletonLinkConfig.cmake)
include_directories(${link_HEADERS})

set(SRC
  src/main.cpp
  src/Engine.cpp
  src/Max7219.cpp
  src/Display.cpp
  src/Input.cpp
  src/Output.cpp
)
add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} pthread)
target_link_libraries(${PROJECT_NAME} Ableton::Link)
target_link_libraries(${PROJECT_NAME} wiringPi)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "modlink")
