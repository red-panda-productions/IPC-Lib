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

if(CODE_COVERAGE AND WIN32)
    STRING(REGEX REPLACE "/" "\\\\" SOURCEDIR ${CMAKE_SOURCE_DIR})
    STRING(REGEX REPLACE "/" "\\\\" BINARYDIR ${CMAKE_CURRENT_BINARY_DIR})
    add_custom_command(TARGET IPCTests POST_BUILD
        COMMAND OpenCppCoverage.exe --sources=${SOURCEDIR}\\IPCLib\\ --export_type=html:coverage -- "${BINARYDIR}\\IPCTests.exe"
        COMMAND echo "-- Source diretory: ${CMAKE_SOURCE_DIR}/IPCLib/"
        COMMAND echo "-- Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
        COMMAND start ${BINARYDIR}\\coverage\\index.html
    )
endif(CODE_COVERAGE AND WIN32)

include(GoogleTest)
gtest_discover_tests(IPCTests)