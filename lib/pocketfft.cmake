# https://gitlab.mpcdf.mpg.de/mtr/pocketfft

CPMAddPackage(
  NAME pocketfft
  VERSION 2021.11.23
  GIT_TAG daa8bb18327bc5c7d22c69428c25cf5dc64167d3
  GIT_REPOSITORY https://gitlab.mpcdf.mpg.de/mtr/pocketfft
  DOWNLOAD_ONLY)

if(pocketfft_ADDED)

  add_library(pocketfft INTERFACE)

  target_include_directories(pocketfft
    INTERFACE "${pocketfft_SOURCE_DIR}")

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

endif()
