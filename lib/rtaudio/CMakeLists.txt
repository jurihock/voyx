add_library(rtaudio)

target_sources(rtaudio
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/RtAudio.h"
         "${CMAKE_CURRENT_LIST_DIR}/RtAudio.cpp"
)

target_include_directories(rtaudio
  PUBLIC  "${CMAKE_CURRENT_LIST_DIR}/.."
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
)

target_compile_features(rtaudio
  PRIVATE cxx_std_11
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")

  find_package(PkgConfig QUIET)

  if (PKG_CONFIG_FOUND)

    pkg_search_module(JACK QUIET jack)

  endif()

  if (NOT JACK_FOUND)

    message(WARNING "Continuing without jack!")

  else()

    target_compile_definitions(rtaudio
      PUBLIC -D__UNIX_JACK__
    )

    target_link_libraries(rtaudio
      PUBLIC jack
    )

  endif()

endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

  target_compile_definitions(rtaudio
    PUBLIC -D__MACOSX_CORE__
  )

  target_link_libraries(rtaudio
    PUBLIC "-framework CoreFoundation"
           "-framework CoreAudio"
  )

endif()

#####################
# DISABLED WARNINGS #
#####################

if (UNIX)

  target_compile_options(rtaudio
    PUBLIC -Wno-deprecated-declarations
  )

  if (APPLE)

    target_compile_options(rtaudio
      PUBLIC -Wno-undef-prefix
    )

  endif()

endif()
