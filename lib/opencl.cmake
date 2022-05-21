option(OPENCL "Enable OpenCL support" ON)

find_package(OpenCL QUIET)

if(OPENCL AND NOT OpenCL_FOUND)

  message(WARNING "Continuing without OpenCL support!")
  set(OPENCL OFF CACHE BOOL "" FORCE)

endif()

add_library(opencl INTERFACE)

if (OPENCL)

  target_link_libraries(opencl
    INTERFACE "${OpenCL_LIBRARIES}")

  # https://github.com/KhronosGroup/OpenCL-CLHPP

  CPMAddPackage(
    NAME khronos
    VERSION 2.0.16
    GITHUB_REPOSITORY KhronosGroup/OpenCL-CLHPP
    DOWNLOAD_ONLY YES)

  if(khronos_ADDED)

    target_include_directories(opencl
      INTERFACE "${khronos_SOURCE_DIR}/include")

    target_compile_definitions(opencl
      INTERFACE -DCL_HPP_ENABLE_EXCEPTIONS)

    target_compile_definitions(opencl
      INTERFACE -DCL_HPP_MINIMUM_OPENCL_VERSION=120)

    target_compile_definitions(opencl
      INTERFACE -DCL_HPP_TARGET_OPENCL_VERSION=120)

  endif()

endif()
