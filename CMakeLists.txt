cmake_minimum_required(VERSION 3.16)
project(senkaid
    VERSION 0.0.1
    DESCRIPTION "A C++ library for linear algebra"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
set(CMAKE_COLOR_MAKEFILE ON)

add_library(senkaid 
        interface/matrix.cpp
)

target_include_directories(senkaid 
PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/senkaid>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/senkaid>
)

add_executable(senkaid-run src/main.cpp)
target_link_libraries(senkaid-run PRIVATE senkaid)

target_include_directories(senkaid 
PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

target_compile_features(senkaid PUBLIC cxx_std_20)

target_compile_options(senkaid PRIVATE
-Wall -Wextra -Wpedantic -mavx -mavx2
)

target_compile_options(senkaid-run PRIVATE -mavx -mavx2)

include(GNUInstallDirs)

install(TARGETS senkaid
    EXPORT senkaidTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT senkaidTargets
    FILE senkaidTargets.cmake
    NAMESPACE senkaid::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/senkaid
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/senkaidConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/senkaidConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/senkaidConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/senkaid
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/senkaidConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/senkaidConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/senkaid
)
