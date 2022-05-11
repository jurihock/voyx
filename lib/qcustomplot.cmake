# https://www.qcustomplot.com

if (UI)

  CPMAddPackage(
    NAME qcustomplot
    VERSION 2.1.0
    URL https://www.qcustomplot.com/release/2.1.0fixed/QCustomPlot-source.tar.gz
    DOWNLOAD_ONLY YES)

  if(qcustomplot_ADDED)

    add_library(qcustomplot)

    target_sources(qcustomplot
      PUBLIC "${qcustomplot_SOURCE_DIR}/qcustomplot.h"
             "${qcustomplot_SOURCE_DIR}/qcustomplot.cpp"
    )

    target_include_directories(qcustomplot
      PUBLIC  "${qcustomplot_SOURCE_DIR}"
    )

    target_link_libraries(qcustomplot
      PUBLIC qt
    )

  endif()

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
