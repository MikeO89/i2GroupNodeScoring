cmake_minimum_required(VERSION 3.16)
project(i2GroupTechTest VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_VS_JUST_MY_CODE_DEBUGGING ON)

find_package(Boost REQUIRED COMPONENTS program_options)

find_package(Doxygen REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(JsonCpp CONFIG REQUIRED)

if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")
else()
    message(FATAL_ERROR "Doxygen not found")
endif()

set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOC_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/html)

configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)

set(I2_HEADERS
    include/i2/directives.hpp
    include/i2/io.hpp
    include/i2/node.hpp
    include/i2/nodeLoader.hpp
)

set(I2_SOURCES
    ${CMAKE_SOURCE_DIR}/source/i2/io.cpp
    ${CMAKE_SOURCE_DIR}/source/i2/node.cpp
    ${CMAKE_SOURCE_DIR}/source/i2/nodeLoader.cpp
)

set(I2_DATA_FILES
    ${CMAKE_SOURCE_DIR}/resources/data.json
    ${CMAKE_SOURCE_DIR}/resources/invalidNodeIndex.json
    ${CMAKE_SOURCE_DIR}/resources/nodeSelfReference.json
    )

# I2 Library - making a shared library as the code is used in two executables (i2GroupTechTest and i2GroupUnitTest) and a shared library will keep the executable sizes to a minimum
add_library(i2Lib SHARED ${I2_SOURCES})
target_compile_definitions(i2Lib PRIVATE BUILDING_I2LIB)  # Export symbols from this library

if(UNIX)
    target_compile_options(i2Lib PRIVATE "-fvisibility=hidden")
endif()

# Add include directories for i2Lib (so the headers are available)
target_include_directories(i2Lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(i2Lib PRIVATE JsonCpp::JsonCpp)

# Tech Test Executable
add_executable(i2GroupTechTest ${CMAKE_SOURCE_DIR}/source/main.cpp)
target_include_directories(i2GroupTechTest PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(i2GroupTechTest PRIVATE i2Lib JsonCpp::JsonCpp Boost::program_options)

set(APP_ARGUMENTS "" CACHE STRING "Command-line arguments for i2GroupTechTest")

# Define a target to run the executable with arguments
add_custom_target(run_i2GroupTechTest
    COMMAND i2GroupTechTest ${APP_ARGUMENTS}
    DEPENDS i2GroupTechTest
    COMMENT "Running i2GroupTechTest with arguments"
)

# Unit Test Executable
add_executable(i2GroupUnitTest ${CMAKE_SOURCE_DIR}/tests/nodeLinkingTest.cpp)
target_include_directories(i2GroupUnitTest PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(i2GroupUnitTest PUBLIC i2Lib GTest::gtest GTest::gtest_main JsonCpp::JsonCpp)

# Copy the sample JSON data file to the test output directory
#configure_file(${CMAKE_SOURCE_DIR}/resources/data.json
              # ${CMAKE_BINARY_DIR}/resources/data.json COPYONLY)

set(TEST_RESOURCES_DIR ${CMAKE_BINARY_DIR}/resources)

# Ensure the test resources directory exists and copy files
file(MAKE_DIRECTORY ${TEST_RESOURCES_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${TEST_RESOURCES_DIR})

enable_testing()

#add_test(i2GroupUnitTest i2GroupUnitTest)
include(GoogleTest)
gtest_discover_tests(i2GroupUnitTest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Ensure the headers are copied across for the install targets
install(FILES ${I2_HEADERS} DESTINATION  include/i2)

# Ensure the sample JSON data file is available for the test install target
install(FILES ${I2_DATA_FILES} DESTINATION resources)

# Ensure the targets are installed
install(TARGETS i2Lib i2GroupTechTest i2GroupUnitTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "GTest DLL path: ${VCPKG_INSTALLED_DIR}/x64-windows/bin/gtest_main.dll")

# Determine the architecture (x86 or x64)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCH "x86")
else()
    message(FATAL_ERROR "Unknown architecture!")
endif()

# Boost DLLs: Dynamically locate the correct version based on the compiler and architecture
file(GLOB BOOST_DLLS "${CMAKE_BINARY_DIR}/vcpkg_installed/${ARCH}-windows/bin/boost_program_options-*-mt-${ARCH}-*.dll")

# GTest DLLs: Dynamically find gtest DLLs
file(GLOB GTEST_DLLS 
    "${CMAKE_BINARY_DIR}/vcpkg_installed/${ARCH}-windows/bin/gtest_main.dll"
    "${CMAKE_BINARY_DIR}/vcpkg_installed/${ARCH}-windows/bin/gtest.dll"
)

# JSONCpp DLLs: Dynamically find jsoncpp DLL
file(GLOB JSONCPP_DLLS 
    "${CMAKE_BINARY_DIR}/vcpkg_installed/${ARCH}-windows/bin/jsoncpp.dll"
)

# Copy gtest_main.dll to the same folder as the executable during installation
set(EXTERN_DLLS_TO_INSTALL
    ${BOOST_DLLS}
    ${GTEST_DLLS}
    ${JSONCPP_DLLS}
)

install(FILES ${EXTERN_DLLS_TO_INSTALL}
        DESTINATION bin)

execute_process(COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                RESULT_VARIABLE DOXYGEN_RESULT)
if(DOXYGEN_RESULT)
    message(FATAL_ERROR "Doxygen generation failed!")
endif()


# Ensure documentation is generated during installation
#install(CODE "execute_process(COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT})")

# Install the generated documentation
install(DIRECTORY ${DOC_OUTPUT_DIR} DESTINATION docs)