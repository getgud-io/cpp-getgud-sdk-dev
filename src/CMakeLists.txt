file(GLOB MAIN_HEADER "../include/GetGudSdk.h")
file(GLOB MAIN_SOURCE "GetGudSdk.cpp")
file(GLOB C_HEADER "../include/GetGudSdk_C.h")
file(GLOB C_SOURCE "GetGudSdk_C.cpp")
file(GLOB DLLMAIN_SOURCE "dllMain.cpp")
file(GLOB ACTIONS_HEADERS "../include/actions/*.h")
file(GLOB ACTIONS_SOURCES "actions/*.cpp")
file(GLOB CONFIG_HEADERS "config/*.h")
file(GLOB CONFIG_SOURCES "config/*.cpp")
file(GLOB ACTIONS_BUFFER_HEADERS "actions_buffer/*.h")
file(GLOB ACTIONS_BUFFER_SOURCES "actions_buffer/*.cpp")
file(GLOB GAME_CONTAINER_HEADERS "game_container/*.h")
file(GLOB GAME_CONTAINER_SOURCES "game_container/*.cpp")
file(GLOB GAME_SENDER_HEADERS "senders/game_sender/*.h")
file(GLOB GAME_SENDER_SOURCES "senders/game_sender/*.cpp")
file(GLOB PLAYER_UPDATER_HEADERS "senders/player_updater/*.h")
file(GLOB PLAYER_UPDATER_SOURCES "senders/player_updater/*.cpp")
file(GLOB REPORT_SENDER_HEADERS "senders/report_sender/*.h")
file(GLOB REPORT_SENDER_SOURCES "senders/report_sender/*.cpp")
file(GLOB SENDERS_DATA_HEADERS "senders/*.h")
file(GLOB SENDERS_DATA_SOURCES "senders/*.cpp")
file(GLOB ZIP_HEADERS "game_container/zip/*.h")
file(GLOB ZIP_SOURCES "game_container/zip/*.cpp")
file(GLOB LOGGER_HEADERS "logger/*.h")
file(GLOB LOGGER_SOURCES "logger/*.cpp")
file(GLOB UTILS_HEADERS "utils/*.h")
file(GLOB UTILS_SOURCES "utils/*.cpp")

set(project_files ${MAIN_HEADER} ${MAIN_SOURCE}
						${C_HEADER} ${C_SOURCE}
						${ACTIONS_HEADERS} ${ACTIONS_SOURCES}
						${CONFIG_HEADERS} ${CONFIG_SOURCES}
						${ACTIONS_BUFFER_HEADERS} ${ACTIONS_BUFFER_SOURCES}
						${LOGGER_HEADERS} ${LOGGER_SOURCES}
						${PLAYER_UPDATER_HEADERS} ${PLAYER_UPDATER_SOURCES}
						${REPORT_SENDER_HEADERS} ${REPORT_SENDER_SOURCES}
						${ZIP_HEADERS} ${ZIP_SOURCES}
						${GAME_CONTAINER_HEADERS} ${GAME_CONTAINER_SOURCES}
						${GAME_SENDER_HEADERS} ${GAME_SENDER_SOURCES}
						${UTILS_HEADERS} ${UTILS_SOURCES}
						${SENDERS_DATA_SOURCES} ${SENDERS_DATA_HEADERS}
						)

add_compile_options(-fPIC)

if ((UNIX AND LIBCURL) OR NOT UNIX)
file(GLOB LIBCURL_DEBUG "../libs/libcurl/builds/*debug*/")
list(FILTER LIBCURL_DEBUG EXCLUDE REGEX ".*-obj-.*")
file(GLOB LIBCURL_RELEASE "../libs/libcurl/builds/*release*/")
list(FILTER LIBCURL_RELEASE EXCLUDE REGEX ".*-obj-.*")
endif()

if (UNIX)
file(GLOB ZLIB_LIB "../libs/*zlib/")
else()
file(GLOB ZLIB_LIB "../libs/*zlib/")
endif()

if (DLL_BUILD)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_library(GetGudSdk SHARED ${project_files} ${DLLMAIN_SOURCE})
source_group("Source Files\\C" FILES ${DLLMAIN_SOURCE})
else() 
if (SO_BUILD)
link_directories(
	/usr/lib/x86_64-linux-gnu/
	$<$<CONFIG:Debug>:${LIBCURL_DEBUG}/lib/>
	$<$<CONFIG:Release>:${LIBCURL_RELEASE}/lib/>
	${ZLIB_LIB}
)
add_library(GetGudSdk SHARED ${project_files})
target_precompile_headers(GetGudSdk PRIVATE pch.h)
set(CMAKE_SHARED_LINKER_FLAGS "-fPIC")
set_target_properties(GetGudSdk PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
else()
if (UNIX)
link_directories(
	/usr/lib/x86_64-linux-gnu/
	$<$<CONFIG:Debug>:${LIBCURL_DEBUG}/lib/>
	$<$<CONFIG:Release>:${LIBCURL_RELEASE}/lib/>
	${ZLIB_LIB}
)
endif()
add_library(GetGudSdk STATIC ${project_files})
endif()
endif()

target_include_directories(GetGudSdk PRIVATE source)
set_target_properties(GetGudSdk PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(GetGudSdk PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(GetGudSdk PROPERTIES PUBLIC_HEADER ../include/GetGudSdk.h PUBLIC_HEADER ../include/GetGudSdk_C.h)


source_group("Header Files\\" FILES ${MAIN_HEADER})
source_group("Source Files\\" FILES ${MAIN_SOURCE})
source_group("Header Files\\C" FILES ${C_HEADER})
source_group("Source Files\\C" FILES ${C_SOURCE})
source_group("Header Files\\actions" FILES ${ACTIONS_HEADERS})
source_group("Source Files\\actions" FILES ${ACTIONS_SOURCES})
source_group("Header Files\\config" FILES ${CONFIG_HEADERS})
source_group("Source Files\\config" FILES ${CONFIG_SOURCES})
source_group("Header Files\\actions_buffer" FILES ${ACTIONS_BUFFER_HEADERS})
source_group("Source Files\\actions_buffer" FILES ${ACTIONS_BUFFER_SOURCES})
source_group("Header Files\\senders\\player_updater" FILES ${PLAYER_UPDATER_HEADERS})
source_group("Source Files\\senders\\player_updater" FILES ${PLAYER_UPDATER_SOURCES})
source_group("Header Files\\senders\\report_sender" FILES ${REPORT_SENDER_HEADERS})
source_group("Source Files\\senders\\report_sender" FILES ${REPORT_SENDER_SOURCES})
source_group("Header Files\\senders\\game_sender" FILES ${GAME_SENDER_HEADERS})
source_group("Source Files\\senders\\game_sender" FILES ${GAME_SENDER_SOURCES})
source_group("Header Files\\senders" FILES ${SENDERS_DATA_HEADERS})
source_group("Source Files\\senders" FILES ${SENDERS_DATA_SOURCES})
source_group("Header Files\\game_container" FILES ${GAME_CONTAINER_HEADERS})
source_group("Source Files\\game_container" FILES ${GAME_CONTAINER_SOURCES})
source_group("Header Files\\game_container\\zip" FILES ${ZIP_HEADERS})
source_group("Source Files\\game_container\\zip" FILES ${ZIP_SOURCES})
source_group("Header Files\\logger" FILES ${LOGGER_HEADERS})
source_group("Source Files\\logger" FILES ${LOGGER_SOURCES})
source_group("Header Files\\utils" FILES ${UTILS_HEADERS})
source_group("Source Files\\utils" FILES ${UTILS_SOURCES})

target_include_directories(GetGudSdk
    PRIVATE
        $<$<CONFIG:Debug>:${LIBCURL_DEBUG}/include/>
        $<$<CONFIG:Release>:${LIBCURL_RELEASE}/include/>
		../libs/ #zlib builds into root
		../src/
)

if (SO_BUILD OR UNIX)
set_property(TARGET GetGudSdk PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(GetGudSdk
    PRIVATE
        libcurl.so
		libz.a
		libssl.a
		libcrypto.a
)
else()
message(${ZLIB_LIB}/zlib.lib)
message(${LIBCURL_RELEASE})
target_link_libraries(GetGudSdk
    PRIVATE
        $<$<CONFIG:Debug>:${LIBCURL_DEBUG}/lib/libcurl_a_debug.lib>
        $<$<CONFIG:Release>:${LIBCURL_RELEASE}/lib/libcurl_a.lib>
		${ZLIB_LIB}/zlib.lib
)
endif()
if (DLL_BUILD)
	target_link_libraries(GetGudSdk
		PRIVATE
		Normaliz.lib
		Ws2_32.lib
		Crypt32.lib
		wldap32.lib
	)
	target_precompile_headers(GetGudSdk PRIVATE pch.h)
	set_target_properties(GetGudSdk PROPERTIES 
	VS_DEBUGGER_COMMAND "$<TARGET_FILE_DIR:GetGudSdk>/C_Starter.exe")

endif()
