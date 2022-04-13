cmake_minimum_required(VERSION 3.18)

project(voyx)

if (MSVC)
  # add_compile_options(/W3 /WX)
else()
  # add_compile_options(-Wall -Werror)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/lib/cxxopts/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/dr_libs/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/pocketfft/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/readerwriterqueue/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/rtaudio/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/rtmidi/CMakeLists.txt")

include("${CMAKE_CURRENT_LIST_DIR}/src/voyx/CMakeLists.txt")
