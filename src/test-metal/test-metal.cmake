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
