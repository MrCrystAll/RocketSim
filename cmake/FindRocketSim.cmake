# FindRocketSim.cmake

# This module defines:
#   RocketSim_FOUND
#   RocketSim_INCLUDE_DIRS
#   RocketSim_LIBRARIES

find_path(RocketSim_INCLUDE_DIR
    NAMES RocketSim/Framework.h  # Replace with a known header file
    PATH_SUFFIXES RocketSim
)

find_library(RocketSim_LIBRARY
    NAMES RocketSim
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RocketSim
    REQUIRED_VARS RocketSim_LIBRARY RocketSim_INCLUDE_DIR
    VERSION_VAR RocketSim_VERSION
)

if(RocketSim_FOUND)
    set(RocketSim_LIBRARIES ${RocketSim_LIBRARY})
    set(RocketSim_INCLUDE_DIRS ${RocketSim_INCLUDE_DIR})
endif()
