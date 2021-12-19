set(BUILD_TARGET "log")
set(ALIAS_NAME "lib::${BUILD_TARGET}")
set(TEST_BUILD_TARGET ${BUILD_TARGET}${POSTFIX_TEST_NAME})

set(HEADER
		FileLog.h
	)

set(SOURCE
		FileLog.cpp
	)

set(SOURCE_TEST
		test/main.cpp
		test/FileLogTest.cpp
	)

set(DEPENDENCIES
		lib::design-pattern
		lib::file
		lib::thread
	)

func_build_library(${ALIAS_NAME} ${BUILD_TARGET} "${HEADER}" "${SOURCE}" "${DEPENDENCIES}")

func_build_unit_test(${TEST_BUILD_TARGET} "${SOURCE_TEST}" "${SOURCE}" "${DEPENDENCIES}")
