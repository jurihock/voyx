add_library(wildcards INTERFACE)

target_sources(wildcards
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/cx.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/wildcards.hpp"
)

target_include_directories(wildcards
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/.."
)
