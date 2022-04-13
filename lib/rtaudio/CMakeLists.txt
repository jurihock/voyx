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

  target_compile_definitions(rtaudio
    PRIVATE -D__UNIX_JACK__
  )

  target_link_libraries(rtaudio
    INTERFACE jack
  )

endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

  target_compile_definitions(rtaudio
    PRIVATE -D__MACOSX_CORE__
  )

  target_link_libraries(rtaudio
    INTERFACE "-framework CoreAudio"
  )

endif()
