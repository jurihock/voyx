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

target_compile_features(voyx
  PRIVATE cxx_std_11
)
