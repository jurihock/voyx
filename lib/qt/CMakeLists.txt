if (UI)

  set(CMAKE_AUTOMOC ON)

  find_package(Qt5 REQUIRED COMPONENTS
    Core
    PrintSupport
    Widgets
  )

  add_library(qt INTERFACE)

  target_link_libraries(qt
    INTERFACE Qt5::Core
              Qt5::PrintSupport
              Qt5::Widgets
  )

else()

  add_library(qt INTERFACE)

endif()
