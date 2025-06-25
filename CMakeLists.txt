cmake_minimum_required (VERSION 3.31)

project("RocketSim" VERSION 2.2.6 LANGUAGES CXX)

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

# Maximum performance optimizations for shared library
target_compile_options(RocketSim PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:
        # Core optimizations
        -O3                             # Maximum optimization
        -march=native                   # CPU-specific optimizations
        -mtune=native                   # Tune for current CPU
        -funroll-loops                  # Aggressive loop unrolling
        -ffast-math                     # Fast math (breaks IEEE)
        -ffinite-math-only             # Assume finite math
        -fno-math-errno                # Don't set errno for math
        -fno-signed-zeros              # Assume no signed zeros
        -fno-trapping-math             # No math exceptions
        -fassociative-math             # Allow math reassociation
        -freciprocal-math              # Allow reciprocal approximations
        
        # Link-time optimizations
        -flto                          # Link-time optimization
        -fwhole-program               # Whole program optimization
        
        # Code generation
        -fomit-frame-pointer          # Don't keep frame pointer
        -fstrict-aliasing             # Strict aliasing rules
        -fprefetch-loop-arrays        # Prefetch loop arrays
        
        # Shared library specific
        -fPIC                         # Position independent code
        -fvisibility=hidden           # Hide symbols by default
        -fvisibility-inlines-hidden   # Hide inline symbols
        -fno-plt                      # Avoid PLT overhead
        -fno-semantic-interposition   # No symbol interposition
        
        # Advanced optimizations
        -ftree-vectorize              # Auto-vectorization
        -fvect-cost-model=unlimited   # Unlimited vectorization
        -funswitch-loops              # Loop unswitching
        -fpredictive-commoning        # Predictive commoning
        -fgcse-after-reload           # GCSE after reload
        -ftree-loop-distribute-patterns # Loop distribution
        -fsplit-wide-types            # Split wide types
        -ftree-loop-vectorize         # Loop vectorization
        -ftree-slp-vectorize          # SLP vectorization
    >
    
    $<$<CXX_COMPILER_ID:MSVC>:
        # MSVC optimizations
        /O2                           # Maximum optimization
        /Ox                           # Full optimization
        /Ob3                          # Aggressive inlining
        /Oi                           # Enable intrinsics
        /Ot                           # Favor speed over size
        /Oy                           # Omit frame pointer
        /GT                           # Fiber-safe TLS
        /GL                           # Whole program optimization
        /fp:fast                      # Fast floating point
        /Qpar                         # Auto-parallelization
        /arch:AVX2                    # Use AVX2 instructions
        # /arch:AVX512                # Use AVX512 if available
        /favor:INTEL64                # Optimize for Intel64
    >
)

# Link-time optimizations
target_link_options(RocketSim PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:
        -flto                         # Link-time optimization
        -fuse-linker-plugin          # Use linker plugin for LTO
        -Wl,-O3                      # Linker optimization level
        -Wl,--as-needed              # Only link needed libraries
        -Wl,--gc-sections            # Remove unused sections
        -Wl,-z,now                   # Resolve symbols at load time
        -Wl,-z,relro                 # Read-only relocations
        -Wl,--hash-style=gnu         # Use GNU hash style
        -Wl,--sort-common            # Sort common symbols
        $<$<CONFIG:Release>:-Wl,--strip-all>  # Strip symbols in release
    >
    
    $<$<CXX_COMPILER_ID:MSVC>:
        /LTCG                        # Link-time code generation
        /OPT:REF                     # Remove unreferenced functions
        /OPT:ICF                     # Identical COMDAT folding
        /INCREMENTAL:NO              # Disable incremental linking
    >
)

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