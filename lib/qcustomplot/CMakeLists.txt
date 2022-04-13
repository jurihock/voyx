add_library(qcustomplot)

target_sources(qcustomplot
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/qcustomplot.h"
         "${CMAKE_CURRENT_LIST_DIR}/qcustomplot.cpp"
)

target_include_directories(qcustomplot
  PUBLIC  "${CMAKE_CURRENT_LIST_DIR}/.."
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(qcustomplot PUBLIC
  PUBLIC qt
)
