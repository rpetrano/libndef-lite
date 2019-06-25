set(DOCTEST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
add_library(Doctest IMPORTED INTERFACE)
target_include_directories(Doctest INTERFACE ${DOCTEST_INCLUDE_DIR})

add_library(test-main OBJECT test-main.cpp)
target_link_libraries(test-main Doctest)

SET(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test-encoding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test-message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test-record.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test-recordHeader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test-recordType.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test-textRecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test-uriRecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test-util.cpp
)

foreach(TEST ${TEST_SOURCES})
    # Strip off test prefix
    string(REGEX MATCH "^.*/test-(.*)\\.[^.]*$" dummy ${TEST})
    set(testName ${CMAKE_MATCH_1})

    # Add individual test executables 
    add_executable("${testName}")
    target_sources("${testName}"
        PRIVATE
            "${TEST}"
            $<TARGET_OBJECTS:test-main>
    )
    target_link_libraries(${testName} Doctest ndef-lite)
    add_test(NAME ${testName} COMMAND ${testName})
endforeach()