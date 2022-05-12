# https://github.com/thestk/rtmidi

CPMAddPackage(
  NAME rtmidi
  VERSION 5.0.0
  GIT_TAG 5.0.0
  GITHUB_REPOSITORY thestk/rtmidi
  DOWNLOAD_ONLY YES)

if(rtmidi_ADDED)

  add_library(rtmidi)

  target_sources(rtmidi
    PUBLIC "${rtmidi_SOURCE_DIR}/RtMidi.h"
           "${rtmidi_SOURCE_DIR}/RtMidi.cpp")

  target_include_directories(rtmidi
    PUBLIC  "${rtmidi_SOURCE_DIR}")

  target_compile_features(rtmidi
    PRIVATE cxx_std_11)

  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")

    find_package(PkgConfig QUIET)

    if (PKG_CONFIG_FOUND)

      pkg_search_module(JACK QUIET jack)

    endif()

    if (NOT JACK_FOUND)

      message(WARNING "Continuing without jack!")

    else()

      target_compile_definitions(rtmidi
        PUBLIC -D__UNIX_JACK__)

      target_link_libraries(rtmidi
        PUBLIC jack)

    endif()

  endif()

  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

    target_compile_definitions(rtmidi
      PUBLIC -D__MACOSX_CORE__)

    target_link_libraries(rtmidi
      PUBLIC "-framework CoreFoundation"
             "-framework CoreMIDI")

  endif()

  #####################
  # DISABLED WARNINGS #
  #####################

  if (UNIX)

    target_compile_options(rtmidi
      PUBLIC -Wno-deprecated-declarations)

    if (APPLE)

      target_compile_options(rtmidi
        PUBLIC -Wno-undef-prefix)

    endif()

  endif()

endif()
