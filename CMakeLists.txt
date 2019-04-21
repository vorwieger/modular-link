cmake_minimum_required(VERSION 3.0)
project(ModularLink)

set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(BUILD_DIR ${CMAKE_BINARY_DIR})
set(MODULES_DIR ${ROOT_DIR}/modules)

include(${MODULES_DIR}/link/AbletonLinkConfig.cmake)

set(HEADERS
  src/AudioEngine.hpp
  src/AudioPlatform_Portaudio.hpp
  ${link_HEADERS}
)

set(SOURCES
  src/AudioEngine.cpp
  src/AudioPlatform_Portaudio.cpp
  src/main.cpp
)

add_executable(ModularLink
  ${HEADERS}
  ${SOURCES}
)

target_compile_options(ModularLink PRIVATE -w)

target_link_libraries(ModularLink wiringPi)
target_link_libraries(ModularLink asound portaudio wiringPi)
target_link_libraries(ModularLink atomic pthread)
target_link_libraries(ModularLink Ableton::Link)

source_group("ModularLink" FILES ${HEADERS} ${SOURCES})

