add_library(fmt)

target_sources(fmt
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/core.h"
         "${CMAKE_CURRENT_LIST_DIR}/format.h"
         "${CMAKE_CURRENT_LIST_DIR}/format-inl.h"
         "${CMAKE_CURRENT_LIST_DIR}/format.cc"
)

target_include_directories(fmt
  PUBLIC  "${CMAKE_CURRENT_LIST_DIR}/.."
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
)

target_compile_features(fmt
  PRIVATE cxx_std_11
)
