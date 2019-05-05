cmake_minimum_required(VERSION 3.0)
project(ModularLink)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(modules/link/AbletonLinkConfig.cmake)
include_directories(${link_HEADERS})

set(SRC
  src/main.cpp
  src/Max7219.cpp
  src/Input.cpp
  src/Output.cpp
)
add_executable(ModularLink ${SRC})

target_link_libraries(ModularLink pthread)
target_link_libraries(ModularLink Ableton::Link)
target_link_libraries(ModularLink wiringPi)
