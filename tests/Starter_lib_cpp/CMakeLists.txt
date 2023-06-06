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

file(GLOB LIBCURL_DEBUG "../../libs/libcurl/builds/*x64-debug*/")
list(FILTER LIBCURL_DEBUG EXCLUDE REGEX ".*-obj-.*")
file(GLOB LIBCURL_RELEASE "../../libs/libcurl/builds/*x64-release*/")
list(FILTER LIBCURL_RELEASE EXCLUDE REGEX ".*-obj-.*")
file(GLOB zlib_lib "../../libs/zlib/zlib.lib")

target_include_directories(GetGudSdk_Starter
		PRIVATE
        $<$<CONFIG:Debug>:${LIBCURL_DEBUG}/include/>
        $<$<CONFIG:Release>:${LIBCURL_RELEASE}/include/>
		../../libs/ #zlib builds into root
		../../include/
)

if (WIN32)
	target_link_libraries(GetGudSdk_Starter
        $<$<CONFIG:Debug>:${LIBCURL_DEBUG}/lib/libcurl_a_debug.lib>
        $<$<CONFIG:Release>:${LIBCURL_RELEASE}/lib/libcurl_a.lib>
		$<$<CONFIG:Debug>:../../../build/_build/Debug/GetGudSdk>
		$<$<CONFIG:Release>:../../../build/_build/Release/GetGudSdk>
		${zlib_lib}
	)
	target_link_libraries(GetGudSdk_Starter
		Normaliz.lib
		Ws2_32.lib
		Crypt32.lib
		wldap32.lib
	)
else()
target_link_libraries(GetGudSdk_Starter
        $<$<CONFIG:Debug>:${LIBCURL_DEBUG}/lib/libcurl.a>
        $<$<CONFIG:Release>:${LIBCURL_RELEASE}/lib/libcurl.a>
		$<$<CONFIG:Debug>:../../../build/_build/Debug/GetGudSdk>
		$<$<CONFIG:Release>:../../../build/_build/Release/GetGudSdk>
		${zlib_lib}
)
endif()