# CMakeList.txt : CMake project for IPCLib, include source and define
# project specific logic here.

set(IPCLIB_HEADERS "ClientSocketAsync.h" "ServerSocketAsync.h" "SocketAsync.h")

# Add source to this project's executable.
add_library (IPCLib SHARED "ClientSocketAsync.cpp" "ServerSocketAsync.cpp" "SocketAsync.cpp" ${IPCLIB_HEADERS})

include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(IPCLib)

target_include_directories(IPCLib PUBLIC ${CMAKE_BINARY_DIR}/IPCLib)

install(TARGETS IPCLib)
install(FILES ${IPCLIB_HEADERS} ${CMAKE_BINARY_DIR}/IPCLib/ipclib_export.h DESTINATION include)