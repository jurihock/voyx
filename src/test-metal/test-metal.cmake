add_executable(test-metal)

file(GLOB_RECURSE
  HDR "${CMAKE_CURRENT_LIST_DIR}/*.h"
  CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

target_sources(test-metal
  PRIVATE ${HDR} ${CPP})

target_include_directories(test-metal
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")

target_link_libraries(test-metal
  PRIVATE metal)

target_compile_features(test-metal
  PRIVATE cxx_std_20)

if (UNIX)
  target_link_libraries(test-metal
    PRIVATE pthread)
endif()

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test.air"
  COMMAND xcrun -sdk macosx metal -c "${CMAKE_CURRENT_LIST_DIR}/test.metal" -o "${CMAKE_CURRENT_BINARY_DIR}/test.air"
  WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
  DEPENDS "${CMAKE_CURRENT_LIST_DIR}/test.metal")

add_custom_target(xcrun_test_air
  ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test.air")

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test.metallib"
  COMMAND xcrun -sdk macosx metallib "${CMAKE_CURRENT_BINARY_DIR}/test.air" -o "${CMAKE_CURRENT_BINARY_DIR}/test.metallib"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test.air")

add_custom_target(xcrun_test_metallib
  ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test.metallib")
