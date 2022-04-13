set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
  Core
  PrintSupport
  Widgets
)

add_library(qt INTERFACE)

target_link_libraries(qt
  INTERFACE Qt6::Core
            Qt6::PrintSupport
            Qt6::Widgets
)
