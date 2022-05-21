option(UI "Enable Qt5 based user interface" ON)

find_package(Qt5 COMPONENTS Core QUIET)

if (UI AND NOT Qt5_FOUND)

  message(WARNING "Continuing without user interface!")
  set(UI OFF CACHE BOOL "" FORCE)

endif()

add_library(qt INTERFACE)

if (UI)

  set(CMAKE_AUTOMOC ON)

  find_package(Qt5 REQUIRED COMPONENTS
    Core
    PrintSupport
    Widgets)

  target_link_libraries(qt
    INTERFACE Qt5::Core
              Qt5::PrintSupport
              Qt5::Widgets)

endif()
