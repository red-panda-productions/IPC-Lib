project(IPCTests)

set(INSTALL_GTEST OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

SET(IPCTESTS_SOURCES SocketTest.cpp)
SET(IPCTESTS_HEADERS Utils.h)

add_executable(
  IPCTests ${IPCTESTS_SOURCES} ${IPCTESTS_HEADERS})
target_link_libraries(
  IPCTests
  IPCLib
  gtest_main
)
target_include_directories(IPCTests PUBLIC "../IPCLib")

IF(WIN32)
    add_custom_command(TARGET IPCTests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:IPCTests> $<TARGET_FILE_DIR:IPCTests>
            COMMAND_EXPAND_LISTS
    )
ENDIF(WIN32)

IF(OPEN_CPP_COVERAGE OR GCOVR)
    SET(COV_EXCLUDE_REGEX "\"[ \\t]*(([THROW_]?IPCLIB_ERROR\\(SOCKET_LIBRARY_NAME.*)|(\\}.*))\"")
ENDIF(OPEN_CPP_COVERAGE OR GCOVR)

if(OPEN_CPP_COVERAGE)
    STRING(REGEX REPLACE "/" "\\\\" SOURCEDIR ${CMAKE_SOURCE_DIR})
    STRING(REGEX REPLACE "/" "\\\\" BINARYDIR ${CMAKE_CURRENT_BINARY_DIR})

    IF(OPTION_GITHUB_ACTIONS)
        MESSAGE(STATUS "Creating Github actions coverage file")
        STRING(CONCAT BINARYDIR "${BINARYDIR}" "\\Debug" )
        SET(COV_EXPORT_TYPE "cobertura:${SOURCEDIR}\\coverage.xml")
        SET(COV_EXTRA_COMMANDS "")
    ELSE(OPTION_GITHUB_ACTIONS)
        SET(COV_EXPORT_TYPE "html:${BINARYDIR}\\coverage")
        SET(COV_EXTRA_COMMANDS "@start ${BINARYDIR}\\coverage\\index.html")
    ENDIF(OPTION_GITHUB_ACTIONS)

    FILE(GENERATE OUTPUT ${CMAKE_SOURCE_DIR}/coverage.bat
        CONTENT 
            "@echo off
            \"${OPEN_CPP_COVERAGE}\" --sources=${SOURCEDIR}\\IPCLib\\ --export_type=${COV_EXPORT_TYPE} --excluded_line_regex ${COV_EXCLUDE_REGEX} -- ${BINARYDIR}\\IPCTests.exe
            ${COV_EXTRA_COMMANDS}")

    MESSAGE(STATUS "Created coverage.bat")
endif(OPEN_CPP_COVERAGE)

IF(GCOVR)

    IF(OPTION_GITHUB_ACTIONS)
        MESSAGE(STATUS "Creating Github actions coverage file")
        SET(COV_EXPORT_TYPE "--cobertura")
        SET(COV_EXTRA_COMMANDS "")
        SET(COV_OUTPUT "${CMAKE_SOURCE_DIR}/coverage.xml")
    ELSE(OPTION_GITHUB_ACTIONS)
        SET(COV_EXPORT_TYPE "--html --html-details")
        SET(COV_EXTRA_COMMANDS "bash -c \"xdg-open ${CMAKE_BINARY_DIR}/coverage.html &\" 2> /dev/null")
        SET(COV_OUTPUT "${CMAKE_BINARY_DIR}/coverage.html")
    ENDIF(OPTION_GITHUB_ACTIONS)

    FILE(GENERATE OUTPUT ${CMAKE_SOURCE_DIR}/coverage.sh
            CONTENT
            "${CMAKE_CURRENT_BINARY_DIR}/IPCTests
            gcovr --gcov-ignore-parse-errors --root=\"${CMAKE_SOURCE_DIR}/IPCLib/.\" --object-directory=\"${CMAKE_BINARY_DIR}/IPCLib/CMakeFiles/IPCLib.dir/\" --exclude-lines-by-pattern ${COV_EXCLUDE_REGEX} ${COV_EXPORT_TYPE} -o ${COV_OUTPUT}
            ${COV_EXTRA_COMMANDS}"
            FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ WORLD_EXECUTE WORLD_WRITE WORLD_READ)

    MESSAGE(STATUS "Created coverage.sh at ${CMAKE_SOURCE_DIR}/coverage.sh")
ENDIF(GCOVR)


include(GoogleTest)
gtest_discover_tests(IPCTests DISCOVERY_TIMEOUT 600)