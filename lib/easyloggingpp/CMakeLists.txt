add_library(easyloggingpp)

target_sources(easyloggingpp
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/easylogging++.h"
         "${CMAKE_CURRENT_LIST_DIR}/easylogging++.cc"
)

target_include_directories(easyloggingpp
  PUBLIC  "${CMAKE_CURRENT_LIST_DIR}/.."
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
)

target_compile_features(easyloggingpp
  PRIVATE cxx_std_11
)
