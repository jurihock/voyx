if (UI)

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

else()

  add_library(qcustomplot INTERFACE)

endif()

#####################
# DISABLED WARNINGS #
#####################

if (UI AND UNIX)

  target_compile_options(qcustomplot
    PUBLIC -Wno-deprecated-declarations
  )

endif()
