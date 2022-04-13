add_library(rtmidi)

target_sources(rtmidi
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/RtMidi.h"
         "${CMAKE_CURRENT_LIST_DIR}/RtMidi.cpp"
)

target_include_directories(rtmidi
  PUBLIC  "${CMAKE_CURRENT_LIST_DIR}/.."
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
)

target_compile_features(rtmidi
  PRIVATE cxx_std_11
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")

  target_compile_definitions(rtmidi
    PRIVATE -D__UNIX_JACK__
  )

  target_link_libraries(rtmidi
    INTERFACE jack
  )

endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

  target_compile_definitions(rtmidi
    PRIVATE -D__MACOSX_CORE__
  )

  target_link_libraries(rtmidi
    INTERFACE "-framework CoreMIDI"
  )

endif()
