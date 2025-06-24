cmake_minimum_required(VERSION 3.29)

add_executable(RocketSimIntegration "main.cpp")

find_package(RocketSim)

function(print_status TARGET_NAME)
	if(${TARGET_NAME}_FOUND)
		message(STATUS "============ ${TARGET_NAME} ============")
		message(STATUS "${TARGET_NAME} found at ${${TARGET_NAME}_DIR}")
		message(STATUS "${TARGET_NAME} include dirs at ${${TARGET_NAME}_INCLUDE_DIRS}")
		message(STATUS "${TARGET_NAME} library dirs at ${${TARGET_NAME}_LIB_DIR}")
		if(${TARGET_NAME}_BIN_DIR)
			message(STATUS "${TARGET_NAME} binary dirs at ${${TARGET_NAME}_BIN_DIR}")
		else()
			message(STATUS "No binary")
		endif()
		message(STATUS "${TARGET_NAME} libraires are ${${TARGET_NAME}_LIBRARIES}")
		message(STATUS "============ ${TARGET_NAME} ============")
	else()
		message(FATAL_ERROR "${TARGET_NAME} has not been found")
	endif()
endfunction()

function(copy_collision_meshes_if_needed TARGET_NAME)
    set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/collision_meshes")
    set(DST_DIR "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources/collision_meshes")  # <<== Next to the binary

    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${SRC_DIR}" "${DST_DIR}"
        COMMENT "Copying resources to ${DST_DIR}"
    )
endfunction()

COPY_COLLISION_MESHES_IF_NEEDED(RocketSimIntegration)

print_status(RocketSim)

target_link_libraries(RocketSimIntegration PUBLIC RocketSim::RocketSim)

set_target_properties(RocketSimIntegration PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(RocketSimIntegration PROPERTIES CXX_STANDARD 20)

enable_testing()

add_test(NAME integration_test COMMAND RocketSimIntegration)