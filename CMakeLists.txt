cmake_minimum_required(VERSION 3.25) # or whatever is suitable/available for you

project(
  GetGudSdk
  VERSION 0.1
  DESCRIPTION "The GetGud REST API SDK"
  LANGUAGES CXX)

set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_build/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_build/)

add_subdirectory(src)

file(GLOB LIBCURL_DEBUG "./libs/libcurl/builds/*x64-debug*/")
file(GLOB LIBCURL_RELEASE "./libs/libcurl/builds/*x64-release*/")

target_include_directories(GetGudSdk
    PRIVATE
        $<$<CONFIG:Debug>:${LIBCURL_DEBUG}/include/>
        $<$<CONFIG:Release>:${LIBCURL_RELEASE}/include/>
)

target_link_libraries(GetGudSdk
    PRIVATE
        $<$<CONFIG:Debug>:${LIBCURL_DEBUG}/lib/libcurl_a_debug.lib>
        $<$<CONFIG:Release>:${LIBCURL_RELEASE}/lib/libcurl_a.lib>
)