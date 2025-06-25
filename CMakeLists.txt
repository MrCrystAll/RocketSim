cmake_minimum_required (VERSION 3.31)

project("RocketSim" VERSION 2.2.5 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(CheckIPOSupported)
include(GNUInstallDirs)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define path variables
set(${PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(${PROJECT_NAME}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(${PROJECT_NAME}_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# For installed package
set(${PROJECT_NAME}_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(${PROJECT_NAME}_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}")
set(${PROJECT_NAME}_INSTALL_BIN_DIR "${CMAKE_INSTALL_BINDIR}")
set(${PROJECT_NAME}_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Optional: version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKE_DIR}
)

# Add all headers and code files
file(GLOB_RECURSE FILES_ROCKETSIM "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Only include bullet headers when using MSVC, otherwise just code files
file(GLOB_RECURSE FILES_BULLET "${PROJECT_SOURCE_DIR}/libsrc/bullet3-3.24/*.cpp" "${PROJECT_SOURCE_DIR}/libsrc/bullet3-3.24/*.h")

add_library(RocketSim SHARED ${FILES_ROCKETSIM} ${FILES_BULLET})

check_ipo_supported(RESULT result OUTPUT output)

if(result)
    message(STATUS "IPO / LTO supported")
    set_target_properties(RocketSim PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: ${output}")
endif()

# Output to build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/RocketSim/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/RocketSim/version.h
    @ONLY
)

target_include_directories(RocketSim
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libsrc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(RocketSim PRIVATE ROCKETSIM_EXPORTS)

set_target_properties(RocketSim PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(RocketSim PROPERTIES CXX_STANDARD 20)

install(TARGETS RocketSim
    EXPORT RocketSimTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKE_DIR}
)

# Install headers
install(DIRECTORY ${${PROJECT_NAME}_INCLUDE_DIR}/  "${CMAKE_CURRENT_SOURCE_DIR}/libsrc/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/generated/RocketSim/version.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RocketSim
)

# Configure package config file
configure_package_config_file(
    ${${PROJECT_NAME}_CMAKE_DIR}/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKE_DIR}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_BINDIR
)

# Create version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKE_DIR}
)

install(FILES
    ${CMAKE_SOURCE_DIR}/LICENSE
    ${CMAKE_SOURCE_DIR}/README.md
    DESTINATION .
)