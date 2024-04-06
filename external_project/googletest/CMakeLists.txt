include(FetchContent)

set(TARGET_NAME "googletest")
set(ALIAS_NAME "lib::${TARGET_NAME}")

FetchContent_Declare(
	${TARGET_NAME}
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v1.13.0
)

FetchContent_MakeAvailable(${TARGET_NAME})

add_library(${ALIAS_NAME} ALIAS gtest)
