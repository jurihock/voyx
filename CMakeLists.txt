cmake_minimum_required(VERSION 3.12...3.18)

project(voyx)

if (MSVC)
  # add_compile_options(/W3 /WX)
else()
  # add_compile_options(-Wall -Werror)
endif()

option(UI "Enable Qt5 based user interface" ON)
find_package(Qt5 COMPONENTS Core QUIET)
if (UI AND NOT Qt5_FOUND)
  message(WARNING "Continuing without user interface!")
  set(UI OFF CACHE BOOL "" FORCE)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/lib/cpm.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/lib/cxxopts.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/lib/dr_libs/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/easyloggingpp/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/fmt/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/mlinterp/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/openmp/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/pocketfft/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/qcustomplot/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/qt/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/readerwriterqueue/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/rtaudio/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/rtmidi/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lib/thread_pool/CMakeLists.txt")

include("${CMAKE_CURRENT_LIST_DIR}/src/voyx/CMakeLists.txt")

set(FOLDERS "lib;src")

foreach(FOLDER ${FOLDERS})

  file(GLOB_RECURSE FILES
    "${CMAKE_CURRENT_LIST_DIR}/${FOLDER}/*.h*"
    "${CMAKE_CURRENT_LIST_DIR}/${FOLDER}/*.c*"
  )

  source_group(
    TREE "${CMAKE_CURRENT_LIST_DIR}/${FOLDER}"
    PREFIX "${FOLDER}"
    FILES ${FILES}
  )

endforeach()
