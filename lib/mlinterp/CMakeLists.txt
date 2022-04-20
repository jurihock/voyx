add_library(mlinterp INTERFACE)

target_sources(mlinterp
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/mlinterp.hpp"
)

target_include_directories(mlinterp
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/.."
)
