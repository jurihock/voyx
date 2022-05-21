add_executable(voyx)

file(GLOB_RECURSE
  HDR "${CMAKE_CURRENT_LIST_DIR}/*.h"
  CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}"
  FILES ${HDR} ${CPP}
  PREFIX voyx)

target_sources(voyx
  PRIVATE ${HDR} ${CPP})

target_include_directories(voyx
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")

target_link_libraries(voyx
  PRIVATE cxxopts
          dr_libs
          easyloggingpp
          fmt
          metal
          mlinterp
          openmp
          pocketfft
          qcustomplot
          qt
          readerwriterqueue
          rtaudio
          rtmidi
          thread_pool)

target_compile_features(voyx
  PRIVATE cxx_std_20)

if (UNIX)

  target_link_libraries(voyx
    PRIVATE pthread)

endif()

if (UI)

  target_compile_definitions(voyx
    PRIVATE VOYXUI)

endif()

if (METAL)

  target_compile_definitions(voyx
    PRIVATE VOYXMETAL)

  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default.air"
    COMMAND xcrun -sdk macosx metal -c "${CMAKE_CURRENT_LIST_DIR}/METAL/Voyx.metal" -o "${CMAKE_CURRENT_BINARY_DIR}/default.air"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${CMAKE_CURRENT_LIST_DIR}/METAL/Voyx.metal")

  add_custom_target(xcrun_default_air
    ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/default.air")

  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default.metallib"
    COMMAND xcrun -sdk macosx metallib "${CMAKE_CURRENT_BINARY_DIR}/default.air" -o "${CMAKE_CURRENT_BINARY_DIR}/default.metallib"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/default.air")

  add_custom_target(xcrun_default_metallib
    ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/default.metallib")

  add_dependencies(xcrun_default_metallib xcrun_default_air)

endif()
