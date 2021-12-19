include(FetchContent)

set(TARGET_NAME "lcov")

FetchContent_Declare(
	${TARGET_NAME}
	GIT_REPOSITORY https://github.com/linux-test-project/lcov.git
	GIT_TAG master
)

FetchContent_GetProperties(${TARGET_NAME})
if(NOT ${TARGET_NAME}_POPULATED)
	FetchContent_Populate(${TARGET_NAME})

	set_property(GLOBAL PROPERTY LCOV_SOURCE_DIR "${${TARGET_NAME}_SOURCE_DIR}")
endif()
