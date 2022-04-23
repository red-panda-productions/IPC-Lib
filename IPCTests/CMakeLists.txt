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

add_executable(
  IPCTests "SocketTest.cpp" "Utils.h")
target_link_libraries(
  IPCTests
  IPCLib
  gtest_main
)
target_include_directories(IPCTests PUBLIC "../IPCLib")
add_custom_command(TARGET IPCTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:IPCTests> $<TARGET_FILE_DIR:IPCTests>
        COMMAND_EXPAND_LISTS
)

if(OPEN_CPP_COVERAGE)
    STRING(REGEX REPLACE "/" "\\\\" SOURCEDIR ${CMAKE_SOURCE_DIR})
    STRING(REGEX REPLACE "/" "\\\\" BINARYDIR ${CMAKE_CURRENT_BINARY_DIR})

    IF(OPTION_GITHUB_ACTIONS)
        MESSAGE(STATUS "Creating Github actions coverage file")
        LIST(APPEND BINARYDIR "\\\\Debug" )
        SET(COV_EXPORT_TYPE "cobertura:${SOURCEDIR}\\coverage")
        SET(COV_EXTRA_COMMANDS "")
    ELSE(OPTION_GITHUB_ACTIONS)
        SET(COV_EXPORT_TYPE "html:${BINARYDIR}\\coverage")
        SET(COV_EXTRA_COMMANDS "@start ${BINARYDIR}\\coverage\\index.html")
    ENDIF(OPTION_GITHUB_ACTIONS)

    FILE(GENERATE OUTPUT ${CMAKE_SOURCE_DIR}/coverage.bat
        CONTENT 
            "@echo off
            \"${OPEN_CPP_COVERAGE}\" --sources=${SOURCEDIR}\\IPCLib\\ --export_type=${COV_EXPORT_TYPE} --excluded_line_regex \"[ \\t]*(\"((\"THROW_IPCLIB|IPCLIB\")_ERROR\\(\\\"\\[WSA\\].*)|(\\}.*)\")\" -- ${BINARYDIR}\\IPCTests.exe
            ${COV_EXTRA_COMMANDS}")

    MESSAGE(STATUS "Created coverage.bat")
endif(OPEN_CPP_COVERAGE)

include(GoogleTest)
gtest_discover_tests(IPCTests DISCOVERY_TIMEOUT 600)