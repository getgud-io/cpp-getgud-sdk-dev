file(GLOB MAIN_HEADER "../include/GetGudSdk.h")
file(GLOB MAIN_SOURCE "GetGudSdk.cpp")
file(GLOB ACTIONS_HEADERS "../include/actions/*.h")
file(GLOB ACTIONS_SOURCES "actions/*.cpp")
file(GLOB ACTIONS_BUFFER_HEADERS "actions_buffer/*.h")
file(GLOB ACTIONS_BUFFER_SOURCES "actions_buffer/*.cpp")
file(GLOB PACKETS_BUFFER_HEADERS "packets_buffer/*.h")
file(GLOB PACKETS_BUFFER_SOURCES "packets_buffer/*.cpp")
file(GLOB UTILS_HEADERS "utils/*.h")
file(GLOB UTILS_SOURCES "utils/*.cpp")

add_library(GetGudSdk	${MAIN_HEADER} ${MAIN_SOURCE}
						${ACTIONS_HEADERS} ${ACTIONS_SOURCES}
						${ACTIONS_BUFFER_HEADERS} ${ACTIONS_BUFFER_SOURCES}
						${PACKETS_BUFFER_HEADERS} ${PACKETS_BUFFER_SOURCES}
						${UTILS_HEADERS} ${UTILS_SOURCES}
)

source_group("Header Files\\" FILES ${MAIN_HEADER})
source_group("Source Files\\" FILES ${MAIN_SOURCE})
source_group("Header Files\\actions" FILES ${ACTIONS_HEADERS})
source_group("Source Files\\actions" FILES ${ACTIONS_SOURCES})
source_group("Header Files\\actions_buffer" FILES ${ACTIONS_BUFFER_HEADERS})
source_group("Source Files\\actions_buffer" FILES ${ACTIONS_BUFFER_SOURCES})
source_group("Header Files\\packets_buffer" FILES ${PACKETS_BUFFER_HEADERS})
source_group("Source Files\\packets_buffer" FILES ${PACKETS_BUFFER_SOURCES})
source_group("Header Files\\utils" FILES ${UTILS_HEADERS})
source_group("Source Files\\utils" FILES ${UTILS_SOURCES})