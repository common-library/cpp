include(FetchContent)

set(TARGET_NAME "googletest")
set(ALIAS_NAME "lib::${TARGET_NAME}")

FetchContent_Declare(
	${TARGET_NAME}
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG release-1.11.0
)

FetchContent_GetProperties(${TARGET_NAME})
if(NOT ${TARGET_NAME}_POPULATED)
	FetchContent_Populate(${TARGET_NAME})
	add_subdirectory(${${TARGET_NAME}_SOURCE_DIR} ${${TARGET_NAME}_BINARY_DIR})

	add_library(${ALIAS_NAME} ALIAS gtest)
endif()
