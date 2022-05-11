# https://github.com/parsiad/mlinterp

CPMAddPackage(
  NAME mlinterp
  VERSION 0.1.1
  GITHUB_REPOSITORY parsiad/mlinterp
  DOWNLOAD_ONLY)

if(mlinterp_ADDED)

  add_library(mlinterp INTERFACE)

  target_include_directories(mlinterp
    INTERFACE "${mlinterp_SOURCE_DIR}/mlinterp")

endif()
