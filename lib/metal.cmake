# https://developer.apple.com/metal/cpp

CPMAddPackage(
  NAME metal
  VERSION 2021.11.09
  URL https://developer.apple.com/metal/cpp/files/metal-cpp_macOS12_iOS15.zip
  DOWNLOAD_ONLY YES)

if(metal_ADDED)

  add_library(metal INTERFACE)

  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

    target_include_directories(metal
      INTERFACE "${metal_SOURCE_DIR}")

    target_link_libraries(metal
      INTERFACE "-framework Foundation"
                "-framework Metal"
                "-framework MetalKit")

    # Foundation/Foundation.hpp
    target_compile_definitions(metal
      INTERFACE -DNS_PRIVATE_IMPLEMENTATION)

    # Metal/Metal.hpp
    target_compile_definitions(metal
      INTERFACE -DMTL_PRIVATE_IMPLEMENTATION)

    # QuartzCore/QuartzCore.hpp
    target_compile_definitions(metal
      INTERFACE -DCA_PRIVATE_IMPLEMENTATION)

  endif()

endif()
