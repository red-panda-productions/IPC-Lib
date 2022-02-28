project(IPCTests)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
set_tests_properties(${Tests} PROPERTIES TIMEOUT 2)
add_executable(
  IPCTests "AsyncSocketTest.cpp" "Utils.h")
target_link_libraries(
  IPCTests #moet het zelfde zijn
  IPCLib
  gtest_main
)
target_include_directories(IPCTests PUBLIC "../IPCLib")
add_custom_command(TARGET IPCTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:IPCTests> $<TARGET_FILE_DIR:IPCTests>
        COMMAND_EXPAND_LISTS
)

include(GoogleTest)
gtest_discover_tests(IPCTests)
