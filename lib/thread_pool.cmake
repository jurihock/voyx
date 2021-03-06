# https://github.com/bshoshany/thread-pool

CPMAddPackage(
  NAME thread_pool
  VERSION 2.0.0
  GITHUB_REPOSITORY bshoshany/thread-pool
  DOWNLOAD_ONLY YES)

if(thread_pool_ADDED)

  add_library(thread_pool INTERFACE)

  target_include_directories(thread_pool
    INTERFACE "${thread_pool_SOURCE_DIR}")

  if (UNIX)
    target_link_libraries(thread_pool
      INTERFACE pthread)
  endif()

endif()
