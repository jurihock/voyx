add_library(pocketfft INTERFACE)

target_sources(pocketfft
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/pocketfft_hdronly.h"
)

target_include_directories(pocketfft
  INTERFACE "${CMAKE_CURRENT_LIST_DIR}/.."
)

target_compile_definitions(pocketfft
  INTERFACE -DPOCKETFFT_NO_MULTITHREADING
)

target_compile_definitions(pocketfft
  INTERFACE -DPOCKETFFT_CACHE_SIZE=10
)

if (UNIX)
  target_link_libraries(pocketfft
    INTERFACE pthread)
endif()
