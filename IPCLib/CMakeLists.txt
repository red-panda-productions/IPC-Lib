# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

# CMakeList.txt : CMake project for IPCLib, include source and define
# project specific logic here.

set(IPCLIB_HEADERS "ClientSocket.h" "ServerSocket.h" "Socket.h" "ipclib_portability.h" "ipclib_export.h")
set(IPCLIB_SOURCES "ClientSocket.cpp" "ServerSocket.cpp" "Socket.cpp" "ipclib_windows.cpp" "ipclib_linux.cpp")

# Add source to this project's executable.

IF(WIN32)
    add_library (IPCLib SHARED ${IPCLIB_SOURCES} ${IPCLIB_HEADERS})
ELSEIF(UNIX)
    add_library (IPCLib STATIC ${IPCLIB_SOURCES} ${IPCLIB_HEADERS})
ENDIF(WIN32)

install(TARGETS IPCLib)
install(FILES ${IPCLIB_HEADERS} DESTINATION include)
