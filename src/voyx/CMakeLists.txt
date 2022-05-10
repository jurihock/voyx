add_executable(voyx)

file(GLOB_RECURSE
  HDR "${CMAKE_CURRENT_LIST_DIR}/*.h"
)

file(GLOB_RECURSE
  CPP "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)

target_sources(voyx
  PRIVATE ${HDR} ${CPP}
)

target_include_directories(voyx
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
)

target_link_libraries(voyx
  PRIVATE cxxopts
          dr_libs
          easyloggingpp
          fmt
          mlinterp
          openmp
          pocketfft
          qcustomplot
          qt
          readerwriterqueue
          rtaudio
          rtmidi
          thread_pool
)

target_compile_features(voyx
  PRIVATE cxx_std_20
)

if (UNIX)
  target_link_libraries(voyx
    PRIVATE pthread)
endif()

if (UI)
  target_compile_definitions(voyx
    PRIVATE VOYXUI)
endif()
