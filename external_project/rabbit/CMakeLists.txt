include(FetchContent)

set(TARGET_NAME "rabbit")
set(ALIAS_NAME "lib::${TARGET_NAME}")

FetchContent_Declare(
	${TARGET_NAME}
	GIT_REPOSITORY https://github.com/mashiro/rabbit.git
	GIT_TAG master
)

FetchContent_GetProperties(${TARGET_NAME})
if(NOT ${TARGET_NAME}_POPULATED)
	FetchContent_Populate(${TARGET_NAME})

	add_library(${TARGET_NAME} INTERFACE)
	add_library(${ALIAS_NAME} ALIAS ${TARGET_NAME})

	set_target_properties(
			${TARGET_NAME} PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES ${${TARGET_NAME}_SOURCE_DIR}
		)
endif()
