add_library(readerwriterqueue INTERFACE)

target_sources(readerwriterqueue
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/atomicops.h"
            "${CMAKE_CURRENT_LIST_DIR}/readerwritercircularbuffer.h"
            "${CMAKE_CURRENT_LIST_DIR}/readerwriterqueue.h"
)

target_include_directories(readerwriterqueue
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/.."
)
