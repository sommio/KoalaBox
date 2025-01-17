cmake_minimum_required(VERSION 3.24)

project(KoalaBox
    LANGUAGES CXX
    DESCRIPTION "A box of koality tools"
    )

# Define options

option(KOALABOX_HTTP_SERVER "Include HTTP server" OFF)

# Configure variables

include(GNUInstallDirs)

set(KOALABOX_HEADERS
    include/koalabox/cache.hpp
    include/koalabox/config.hpp
    include/koalabox/core.hpp
    include/koalabox/crypto.hpp
    include/koalabox/dll_monitor.hpp
    include/koalabox/globals.hpp
    include/koalabox/logger.hpp
    include/koalabox/hook.hpp
    include/koalabox/http_client.hpp
    include/koalabox/io.hpp
    include/koalabox/ipc.hpp
    include/koalabox/loader.hpp
    include/koalabox/patcher.hpp
    include/koalabox/paths.hpp
    include/koalabox/util.hpp
    include/koalabox/win_util.hpp
    )

set(KOALABOX_SOURCES
    src/koalabox/cache.cpp
    src/koalabox/core.cpp
    src/koalabox/crypto.cpp
    src/koalabox/dll_monitor.cpp
    src/koalabox/globals.cpp
    src/koalabox/logger.cpp
    src/koalabox/hook.cpp
    src/koalabox/http_client.cpp
    src/koalabox/io.cpp
    src/koalabox/ipc.cpp
    src/koalabox/loader.cpp
    src/koalabox/ntapi.hpp
    src/koalabox/patcher.cpp
    src/koalabox/paths.cpp
    src/koalabox/util.cpp
    src/koalabox/win_util.cpp
    )

if (KOALABOX_HTTP_SERVER)
    set(KOALABOX_HEADERS ${KOALABOX_HEADERS} include/koalabox/http_server.hpp)
    set(KOALABOX_SOURCES ${KOALABOX_SOURCES} src/koalabox/http_server.cpp)
endif ()

add_library(KoalaBox OBJECT ${KOALABOX_HEADERS} ${KOALABOX_SOURCES})

# Configure dependencies

## Cpr - https://github.com/libcpr/cpr/
add_subdirectory(dependencies/cpr EXCLUDE_FROM_ALL)
target_link_libraries(KoalaBox PUBLIC cpr::cpr)

## Json - https://github.com/nlohmann/json
add_subdirectory(dependencies/json EXCLUDE_FROM_ALL)
target_link_libraries(KoalaBox PUBLIC nlohmann_json::nlohmann_json)

## Polyhook2 - https://github.com/stevemk14ebr/PolyHook_2_0
set(POLYHOOK_FEATURE_DETOURS ON)
set(POLYHOOK_FEATURE_INLINENTD OFF)
set(POLYHOOK_FEATURE_EXCEPTION OFF)
set(POLYHOOK_BUILD_DLL ON)
add_subdirectory(dependencies/PolyHook_2_0 EXCLUDE_FROM_ALL)
target_link_libraries(KoalaBox PUBLIC PolyHook_2)

## Spdlog - https://github.com/gabime/spdlog
add_subdirectory(dependencies/spdlog EXCLUDE_FROM_ALL)
target_link_libraries(KoalaBox PUBLIC spdlog::spdlog)

if (KOALABOX_HTTP_SERVER)
    ## OpenSSL - https://github.com/janbar/openssl-cmake
    add_subdirectory(dependencies/openssl-cmake)
    target_link_libraries(KoalaBox PUBLIC ssl)
    target_link_libraries(KoalaBox PUBLIC crypto)

    ## cpp-httplib - https://github.com/yhirose/cpp-httplib
    add_subdirectory(dependencies/cpp-httplib EXCLUDE_FROM_ALL)
    target_link_libraries(KoalaBox PUBLIC httplib::httplib)
endif ()

# Setup KoalaBox exports

target_include_directories(KoalaBox PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
    "$<INSTALL_INTERFACE:include>"
    )

set(PCH_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/koalabox/pch.hpp)
target_precompile_headers(KoalaBox PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${PCH_PATH}>")

install(
    TARGETS KoalaBox DESTINATION ${LIB_INSTALL_DIR}
    INCLUDES ${KOALABOX_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}
)

# Exports Generator

add_executable(exports_generator src/exports_generator/exports_generator.cpp)

target_link_libraries(exports_generator PUBLIC KoalaBox)
