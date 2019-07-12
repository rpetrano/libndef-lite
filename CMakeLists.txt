cmake_minimum_required(VERSION 3.0)
project(ndef-lite VERSION 0.0.2)

# Enable building tests by default
option(NDEF_LITE__DISABLE_TESTS "If tests should be compiled or not" OFF)

set(source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/src/encoding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ndef-record.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/record-header.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/record-type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/text-record.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uri-record.cpp
)

set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ndef-lite/encoding.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ndef-lite/exceptions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ndef-lite/message.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ndef-lite/record-header.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ndef-lite/record.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ndef-lite/record-type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ndef-lite/util.hpp
)

add_library(${PROJECT_NAME}
    SHARED
        ${source_files}
        ${header_files}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        DESCRIPTION "NDEF (NFC Data Exchange Format) Message library written in pure C++"
        VERSION "${PROJECT_VERSION}"
)

target_compile_options(${PROJECT_NAME} PRIVATE -Werror)

set_target_properties(${PROJECT_NAME}
    PROPERTIES PUBLIC_HEADER
        "${header_files}"
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

include(GNUInstallDirs)

if (NOT NDEF_LITE__DISABLE_TESTS)
    # Handle automatic Catch2 testing
    enable_testing()
    include(CTest)
    add_subdirectory(test)
endif()

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ndef-lite
)