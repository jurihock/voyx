add_library(thread_pool INTERFACE)

target_sources(thread_pool
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/thread_pool.hpp"
)

target_include_directories(thread_pool
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/.."
)

if (UNIX)
  target_link_libraries(thread_pool
    INTERFACE pthread)
endif()
