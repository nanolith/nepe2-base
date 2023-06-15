cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0048 NEW)
PROJECT(nepe2base VERSION 0.1.0)

string(REPLACE "." ";" NEPE2BASE_VERSION_LIST "${CMAKE_PROJECT_VERSION}")
list(GET NEPE2BASE_VERSION_LIST 0 NEPE2BASE_VERSION_MAJOR)
list(GET NEPE2BASE_VERSION_LIST 1 NEPE2BASE_VERSION_MINOR)
list(GET NEPE2BASE_VERSION_LIST 2 NEPE2BASE_VERSION_REL)

cmake_minimum_required(VERSION 3.10)

INCLUDE(CheckSymbolExists)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

option(MODELCHECK_REQUIRED "Require Model Checking")

#minunit package
find_package(minunit REQUIRED)
#rcpr package
find_package(rcpr REQUIRED)
#threads package
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

#Build config.h
configure_file(config.h.cmake include/nepe2/config.h)

#includes
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include)

#source files
AUX_SOURCE_DIRECTORY(src/secure_buffer NEPE2BASE_SECURE_BUFFER_SOURCES)
SET(NEPE2BASE_SOURCES
    ${NEPE2BASE_SECURE_BUFFER_SOURCES})

#test source files
AUX_SOURCE_DIRECTORY(test/secure_buffer NEPE2BASE_TEST_SECURE_BUFFER_SOURCES)
SET(NEPE2BASE_TEST_SOURCES 
    ${NEPE2BASE_TEST_SECURE_BUFFER_SOURCES})

ADD_LIBRARY(nepe2base STATIC
    ${NEPE2BASE_SOURCES})
TARGET_COMPILE_OPTIONS(
    nepe2base PRIVATE -fPIC -O2 ${RCPR_CFLAGS}
    -Wall -Werror -Wextra -Wpedantic -Wno-unused-command-line-argument)
TARGET_LINK_LIBRARIES(
    nepe2base PRIVATE ${RCPR_LDFLAGS})

ADD_LIBRARY(nepe2base-${CMAKE_PROJECT_VERSION} SHARED
    ${NEPE2BASE_SOURCES})
TARGET_COMPILE_OPTIONS(
    nepe2base-${CMAKE_PROJECT_VERSION} PRIVATE -fPIC -O2 ${RCPR_CFLAGS}
    -Wall -Werror -Wextra -Wpedantic -Wno-unused-command-line-argument
    ${USE_EXTERN_ASSEMBLER})
TARGET_LINK_LIBRARIES(
    nepe2base-${CMAKE_PROJECT_VERSION} PRIVATE ${RCPR_LDFLAGS})

ADD_EXECUTABLE(testnepe2base
    ${NEPE2BASE_SOURCES} ${NEPE2BASE_TEST_SOURCES})

TARGET_COMPILE_OPTIONS(
    testnepe2base PRIVATE -g -O0 --coverage ${MINUNIT_CFLAGS} ${RCPR_CFLAGS}
                     -Wall -Werror -Wextra -Wpedantic
                     -Wno-unused-command-line-argument ${USE_EXTERN_ASSEMBLER})
TARGET_LINK_LIBRARIES(
    testnepe2base PRIVATE -g -O0 --coverage ${MINUNIT_LDFLAGS} ${RCPR_LDFLAGS})
set_source_files_properties(
    ${NEPE2BASE_TEST_SOURCES} PROPERTIES
    COMPILE_FLAGS "${STD_CXX_20} ${USE_INTERN_ASSEMBLER}")

ADD_CUSTOM_TARGET(
    test
    COMMAND testnepe2base
    DEPENDS testnepe2base)

ADD_CUSTOM_TARGET(
    model_checks
    COMMAND make
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/models)

if (MODELCHECK_REQUIRED)
    ADD_DEPENDENCIES(testnepe2base model_checks)
endif()

#Build a pkg-config file
SET(NEPE2BASE_PC "${CMAKE_BINARY_DIR}/nepe2base.pc")
FILE(WRITE  ${NEPE2BASE_PC} "Name: nepe2base")
FILE(APPEND ${NEPE2BASE_PC} "\nDescription: nepephemeral 2 base library")
FILE(APPEND ${NEPE2BASE_PC} "\nVersion: ${CMAKE_PROJECT_VERSION}")
FILE(APPEND ${NEPE2BASE_PC} "\nURL: https://github.com/nanolith/nepe2-base")
FILE(APPEND ${NEPE2BASE_PC} "\nprefix=${CMAKE_INSTALL_PREFIX}")
FILE(APPEND ${NEPE2BASE_PC} "\nlibdir=\${prefix}/lib")
FILE(APPEND ${NEPE2BASE_PC} "\nincludedir=\${prefix}/include")
FILE(APPEND ${NEPE2BASE_PC} "\nLibs: -L\${libdir} -lnepe2base")
FILE(APPEND ${NEPE2BASE_PC} "\nCflags: -I\${includedir}")
INSTALL(FILES ${NEPE2BASE_PC} DESTINATION lib/pkgconfig)

#Install headers
FILE(GLOB NEPE2BASE_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/nepe2/*.h")
INSTALL(FILES ${NEPE2BASE_INCLUDES} DESTINATION include/nepe2)
INSTALL(
    FILES ${CMAKE_BINARY_DIR}/include/nepe2/config.h DESTINATION include/nepe2)

#Install library
INSTALL(TARGETS nepe2base nepe2base-${CMAKE_PROJECT_VERSION}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
