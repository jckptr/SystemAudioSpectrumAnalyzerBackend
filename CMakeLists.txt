cmake_minimum_required(VERSION 3.10.0)
project(SystemAudioSpectrumAnalyzerBackend VERSION 0.1.0 LANGUAGES C CXX)

set(TARGET_NAME "sasa-backend")



include_directories(
    ${PATH_TO_FFTW}/include
    ${PATH_TO_RTAUDIO}/include
)


set(FFTW3_DIR "C:/Program Filesaaa/fftw/lib/cmake/fftw3")
set(FFTW3_INCLUDE_DIR "C:/Program Files/fftw/include")
set(FFTW3_LIBRARY_DIR "C:/Program Files/fftw/lib")

set(RtAudio_DIR "C:/Program Files (x86)/RtAudio/share/rtaudio3")
set(RTAUDIO_INCLUDE_DIR "C:/Program Files (x86)/RtAudio/include")
set(RTAUDIO_LIBRARY_DIR "C:/Program Files (x86)/RtAudio/lib")


set(FFTW3_MANUALLY OFF)
message(STATUS "Looking for FFTW3")
find_package(FFTW3 QUIET)
if(FFTW3_FOUND)
    message(STATUS "FFTW3 is found via find_package.")
else()
    message(STATUS "FFTW3 is not found via find_package. Try it manually.")

    message(STATUS "find_path(FFTW3_INCLUDE_DIR)")
    find_path(FFTW3_INCLUDE
        NAMES fftw3.h
        PATHS ${FFTW3_INCLUDE_DIR}
    )
    if(NOT FFTW3_INCLUDE OR FFTW3_INCLUDE STREQUAL "FFTW3_INCLUDE-NOTFOUND")
        message(FATAL_ERROR "FFTW3 includes is not found.")
    endif()
    message(STATUS "Include directory: ${FFTW3_INCLUDE}")

    message(STATUS "find_library(FFTW3_LIBRARY_DIR)")
    find_library(FFTW3_LIBRARY
        NAMES fftw3
        PATHS ${FFTW3_LIBRARY_DIR})
    if(NOT FFTW3_LIBRARY)
        message(FATAL_ERROR "FFTW3 library is not found.")
    endif()
    message(STATUS "Library: ${FFTW3_LIBRARY}")
    message(STATUS "FFTW3 is found.")
    set(FFTW3_MANUALLY ON)
endif()


set(RTAUDIO_MANUALLY OFF)
message(STATUS "Looking for RtAudio")
find_package(RtAudio QUIET)
if(RtAudio_FOUND)
    message(STATUS "RtAudio is found via find_package.")
else()
    message(STATUS "RtAudio is not found via find_package. Try it manually.")

    message(STATUS "find_path(RTAUDIO_INCLUDE_DIR)")
    find_path(RTAUDIO_INCLUDE
        NAMES RtAudio.h
        PATHS ${RTAUDIO_INCLUDE_DIR}/rtaudio
    )
    if(NOT RTAUDIO_INCLUDE OR RTAUDIO_INCLUDE STREQUAL "RTAUDIO_INCLUDE-NOTFOUND")
        message(FATAL_ERROR "RtAudio includes is not found.")
    endif()
    message(STATUS "Include directory: ${RTAUDIO_INCLUDE}")

    message(STATUS "find_library(RTAUDIO_LIBRARY_DIR)")
    find_library(RTAUDIO_LIBRARY
        NAMES rtaudio
        PATHS ${RTAUDIO_LIBRARY_DIR})
    if(NOT RTAUDIO_LIBRARY)
        message(FATAL_ERROR "RtAudio library is not found.")
    endif()
    message(STATUS "Library: ${RTAUDIO_LIBRARY}")
    message(STATUS "RtAudio is found.")
    set(RTAUDIO_MANUALLY ON)
endif()



add_executable(${TARGET_NAME} main.cpp) # AudioSpectrumAnalyzer.cpp)

if(FFTW3_MANUALLY)
    target_include_directories(${TARGET_NAME} PRIVATE ${FFTW3_INCLUDE})
    target_link_libraries(${TARGET_NAME} PRIVATE ${FFTW3_LIBRARY})
endif()

if(RTAUDIO_MANUALLY)
    target_include_directories(${TARGET_NAME} PRIVATE ${RTAUDIO_INCLUDE})
    target_link_libraries(${TARGET_NAME} PRIVATE ${RTAUDIO_LIBRARY})
endif()
