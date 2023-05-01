file(GLOB MAIN_HEADER "../include/GetGudSdk.h")
file(GLOB MAIN_SOURCE "GetGudSdk.cpp")
file(GLOB ACTIONS_HEADERS "../include/actions/*.h")
file(GLOB ACTIONS_SOURCES "actions/*.cpp")
file(GLOB CONFIG_HEADERS "../include/config/*.h")
file(GLOB CONFIG_SOURCES "config/*.cpp")
file(GLOB ACTIONS_BUFFER_HEADERS "actions_buffer/*.h")
file(GLOB ACTIONS_BUFFER_SOURCES "actions_buffer/*.cpp")
file(GLOB GAME_CONTAINER_HEADERS "game_container/*.h")
file(GLOB GAME_CONTAINER_SOURCES "game_container/*.cpp")
file(GLOB GAME_SENDER_HEADERS "game_sender/*.h")
file(GLOB GAME_SENDER_SOURCES "game_sender/*.cpp")
file(GLOB LOGGER_HEADERS "logger/*.h")
file(GLOB LOGGER_SOURCES "logger/*.cpp")
file(GLOB UTILS_HEADERS "utils/*.h")
file(GLOB UTILS_SOURCES "utils/*.cpp")

add_library(GetGudSdk	${MAIN_HEADER} ${MAIN_SOURCE}
						${ACTIONS_HEADERS} ${ACTIONS_SOURCES}
						${CONFIG_HEADERS} ${CONFIG_SOURCES}
						${ACTIONS_BUFFER_HEADERS} ${ACTIONS_BUFFER_SOURCES}
						${LOGGER_HEADERS} ${LOGGER_SOURCES}
						${GAME_CONTAINER_HEADERS} ${GAME_CONTAINER_SOURCES}
						${GAME_SENDER_HEADERS} ${GAME_SENDER_SOURCES}
						${UTILS_HEADERS} ${UTILS_SOURCES}
)

source_group("Header Files\\" FILES ${MAIN_HEADER})
source_group("Source Files\\" FILES ${MAIN_SOURCE})
source_group("Header Files\\actions" FILES ${ACTIONS_HEADERS})
source_group("Source Files\\actions" FILES ${ACTIONS_SOURCES})
source_group("Header Files\\config" FILES ${CONFIG_HEADERS})
source_group("Source Files\\config" FILES ${CONFIG_SOURCES})
source_group("Header Files\\actions_buffer" FILES ${ACTIONS_BUFFER_HEADERS})
source_group("Source Files\\actions_buffer" FILES ${ACTIONS_BUFFER_SOURCES})
source_group("Header Files\\game_container" FILES ${GAME_CONTAINER_HEADERS})
source_group("Source Files\\game_container" FILES ${GAME_CONTAINER_SOURCES})
source_group("Header Files\\game_sender" FILES ${GAME_SENDER_HEADERS})
source_group("Source Files\\game_sender" FILES ${GAME_SENDER_SOURCES})
source_group("Header Files\\logger" FILES ${LOGGER_HEADERS})
source_group("Source Files\\logger" FILES ${LOGGER_SOURCES})
source_group("Header Files\\utils" FILES ${UTILS_HEADERS})
source_group("Source Files\\utils" FILES ${UTILS_SOURCES})

