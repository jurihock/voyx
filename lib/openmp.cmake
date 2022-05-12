find_package(OpenMP)

add_library(openmp INTERFACE)

if(OpenMP_CXX_FOUND)

  target_link_libraries(openmp
    INTERFACE OpenMP::OpenMP_CXX)

endif()
