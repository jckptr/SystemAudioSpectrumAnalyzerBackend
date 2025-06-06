cmake_minimum_required(VERSION 3.10.0)
project(SystemAudioSpectrumAnalyzerBackend VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME "sasa-backend")

message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
if(NOT (CMAKE_SYSTEM_NAME MATCHES "Windows"
         OR CMAKE_SYSTEM_NAME MATCHES "Linux"
         OR CMAKE_SYSTEM_NAME MATCHES "Darwin"))
    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()

# extra directories if auto find_package fails
# especially for Windows platform
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(FFTW3_DIR "C:/Program Files/fftw/lib/cmake/fftw3"
        CACHE PATH "Search path for FFTW3Config.cmake for find_package")
    set(RtAudio_DIR "C:/Program Files (x86)/RtAudio/share/rtaudio3"
        CACHE PATH "Search path for RtAudioConfig.cmake for find_package")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # set(FFTW3_DIR "/usr/local/lib/cmake/fftw3")
    # set(RtAudio_DIR "/usr/local/share/rtaudio")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin") # auto generated by copilot, i do not know if it is correct
    # set(FFTW3_DIR "/usr/local/Cellar/fftw/3.3.10_1/lib/cmake/fftw3")
    # set(RtAudio_DIR "/usr/local/Cellar/rtaudio/5.1.0/lib/cmake/RtAudio")
endif()

message(STATUS "FFTW3_DIR: ${FFTW3_DIR}")
message(STATUS "RtAudio_DIR: ${RtAudio_DIR}")


message(STATUS "Looking for FFTW3")
find_package(FFTW3 REQUIRED CONFIG)
if(FFTW3_FOUND)
    get_target_property(FFTW3_INCLUDE_DIRS FFTW3::fftw3 INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(FFTW3_IMPLIB_DEBUG FFTW3::fftw3 IMPORTED_IMPLIB_DEBUG)
    get_target_property(FFTW3_IMPLIB_RELEASE FFTW3::fftw3 IMPORTED_IMPLIB_RELEASE)
    get_target_property(FFTW3_LOCATION_DEBUG FFTW3::fftw3 IMPORTED_LOCATION_DEBUG)
    get_target_property(FFTW3_LOCATION_RELEASE FFTW3::fftw3 IMPORTED_LOCATION_RELEASE)
    message(STATUS "INCLUDE_DIRS: ${FFTW3_INCLUDE_DIRS}")
    message(STATUS "IMPLIB_DEBUG: ${FFTW3_IMPLIB_DEBUG}")
    message(STATUS "LOCATION_DEBUG: ${FFTW3_LOCATION_DEBUG}")
    message(STATUS "IMPLIB_RELEASE: ${FFTW3_IMPLIB_RELEASE}")
    message(STATUS "LOCATION_RELEASE: ${FFTW3_LOCATION_RELEASE}")
    message(STATUS "FFTW3 is found.")
endif()

message(STATUS "Looking for RtAudio")
find_package(RtAudio REQUIRED CONFIG)
if(RtAudio_FOUND)
    get_target_property(RtAudio_INCLUDE_DIRS RtAudio::rtaudio INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(RtAudio_IMPLIB_DEBUG RtAudio::rtaudio IMPORTED_IMPLIB_DEBUG)
    get_target_property(RtAudio_IMPLIB_RELEASE RtAudio::rtaudio IMPORTED_IMPLIB_RELEASE)
    get_target_property(RtAudio_LOCATION_DEBUG RtAudio::rtaudio IMPORTED_LOCATION_DEBUG)
    get_target_property(RtAudio_LOCATION_RELEASE RtAudio::rtaudio IMPORTED_LOCATION_RELEASE)
    message(STATUS "INCLUDE_DIRS: ${RtAudio_INCLUDE_DIRS}")
    message(STATUS "IMPLIB_DEBUG: ${RtAudio_IMPLIB_DEBUG}")
    message(STATUS "LOCATION_DEBUG: ${RtAudio_LOCATION_DEBUG}")
    message(STATUS "IMPLIB_RELEASE: ${RtAudio_IMPLIB_RELEASE}")
    message(STATUS "LOCATION_RELEASE: ${RtAudio_LOCATION_RELEASE}")
    message(STATUS "RtAudio is found.")
endif()


add_executable(${TARGET_NAME} ./src/main.cpp) # AudioSpectrumAnalyzer.cpp)

target_link_libraries(${TARGET_NAME} PRIVATE FFTW3::fftw3)
target_link_libraries(${TARGET_NAME} PRIVATE RtAudio::rtaudio)


# TODO: check Linux
# copy dlls for Windows
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<$<CONFIG:Debug>:${RtAudio_LOCATION_DEBUG}>
        $<$<CONFIG:Debug>:${FFTW3_LOCATION_DEBUG}>
        $<$<NOT:$<CONFIG:Debug>>:${RtAudio_LOCATION_RELEASE}>
        $<$<NOT:$<CONFIG:Debug>>:${FFTW3_LOCATION_RELEASE}>
        $<TARGET_FILE_DIR:${TARGET_NAME}>
    )
endif()
