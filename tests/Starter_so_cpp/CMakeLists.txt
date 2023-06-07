cmake_minimum_required(VERSION 3.18) # or whatever is suitable/available for you

project(
  GetGudSdk_Starter
  VERSION 0.1
  DESCRIPTION "The GetGud REST API SDK"
  LANGUAGES CXX)

set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/)

add_executable(GetGudSdk_Starter main.cpp)

target_include_directories(GetGudSdk_Starter
		PRIVATE
		../../include/
)

target_link_libraries(GetGudSdk_Starter
		$<$<CONFIG:Debug>:../../../build/_build/Debug/GetGudSdk>
		$<$<CONFIG:Release>:../../../build/_build/Release/GetGudSdk>
)