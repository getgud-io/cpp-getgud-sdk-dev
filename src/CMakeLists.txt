file(GLOB MAIN_HEADER "../include/GetGudSdk.h")
file(GLOB MAIN_SOURCE "GetGudSdk.cpp")
file(GLOB ACTIONS_HEADERS "../include/actions/*.h")
file(GLOB ACTIONS_SOURCES "actions/*.cpp")
file(GLOB BUFFER_HEADERS "shared_buffer/*.h")
file(GLOB BUFFER_SOURCES "shared_buffer/*.cpp")
file(GLOB UTILS_HEADERS "utils/*.h")
file(GLOB UTILS_SOURCES "utils/*.cpp")

add_library(GetGudSdk	${MAIN_HEADER} ${MAIN_SOURCE}
						${ACTIONS_HEADERS} ${ACTIONS_SOURCES}
						${BUFFER_HEADERS} ${BUFFER_SOURCES}
						${UTILS_HEADERS} ${UTILS_SOURCES}
)

source_group("Header Files\\" FILES ${MAIN_HEADER})
source_group("Source Files\\" FILES ${MAIN_SOURCE})
source_group("Header Files\\actions" FILES ${ACTIONS_HEADERS})
source_group("Source Files\\actions" FILES ${ACTIONS_SOURCES})
source_group("Header Files\\shared_buffer" FILES ${BUFFER_HEADERS})
source_group("Source Files\\shared_buffer" FILES ${BUFFER_SOURCES})
source_group("Header Files\\utils" FILES ${UTILS_HEADERS})
source_group("Source Files\\utils" FILES ${UTILS_SOURCES})