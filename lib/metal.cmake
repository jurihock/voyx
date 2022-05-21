option(METAL "Enable Apple Metal support" ON)

if(METAL AND NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin")

  message(WARNING "Continuing without metal support!")
  set(METAL OFF CACHE BOOL "" FORCE)

endif()

add_library(metal INTERFACE)

if (METAL)

  # https://developer.apple.com/metal/cpp

  CPMAddPackage(
    NAME metal
    VERSION 2021.11.09
    URL https://developer.apple.com/metal/cpp/files/metal-cpp_macOS12_iOS15.zip
    DOWNLOAD_ONLY YES)

  if(metal_ADDED)

    target_include_directories(metal
      INTERFACE "${metal_SOURCE_DIR}")

    target_link_libraries(metal
      INTERFACE "-framework Foundation"
                "-framework Metal"
                "-framework MetalKit")

  endif()

endif()
