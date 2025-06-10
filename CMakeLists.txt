cmake_minimum_required (VERSION 3.19)

include(CMakePackageConfigHelpers)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(ROCKETSIM_INSTALL_CMAKEDIR lib/cmake/RocketSim)
include(CheckIPOSupported)

project("RocketSim" VERSION 2.2.1)

# Optional: version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/RocketSimConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/RocketSimConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/RocketSimConfig.cmake"
    INSTALL_DESTINATION ${ROCKETSIM_INSTALL_CMAKEDIR}
)

# Add all headers and code files
file(GLOB_RECURSE FILES_ROCKETSIM "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Only include bullet headers when using MSVC, otherwise just code files
file(GLOB_RECURSE FILES_BULLET "${PROJECT_SOURCE_DIR}/libsrc/bullet3-3.24/*.cpp" "${PROJECT_SOURCE_DIR}/libsrc/bullet3-3.24/*.h")


add_library(RocketSim ${FILES_ROCKETSIM} ${FILES_BULLET})

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

set_target_properties(RocketSim PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(RocketSim PROPERTIES CXX_STANDARD 20)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/RocketSim
		DESTINATION include
		FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libsrc/bullet3-3.24
		DESTINATION include
		FILES_MATCHING PATTERN "*.h"
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/generated/RocketSim/version.h
    DESTINATION include/RocketSim
)

install(TARGETS RocketSim
    EXPORT RocketSimTargets
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(FILES cmake/FindRocketSim.cmake
        DESTINATION cmake)

# Export targets
install(EXPORT RocketSimTargets
    FILE RocketSimTargets.cmake
    NAMESPACE RocketSim::
    DESTINATION ${ROCKETSIM_INSTALL_CMAKEDIR}
)

# Install config and version files
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/RocketSimConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/RocketSimConfigVersion.cmake" DESTINATION ${ROCKETSIM_INSTALL_CMAKEDIR})

install(FILES
    ${CMAKE_SOURCE_DIR}/LICENSE
    ${CMAKE_SOURCE_DIR}/README.md
    DESTINATION .
)