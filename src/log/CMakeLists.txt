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
		lib::file
	)

set(CONFIGURE_FILE_TEST
		"test/test.h.in"
	)

func_build_library(${ALIAS_NAME} ${BUILD_TARGET} "${HEADER}" "${SOURCE}" "${DEPENDENCIES}")

func_build_unit_test(${TEST_BUILD_TARGET} "${SOURCE_TEST}" "${SOURCE}" "${DEPENDENCIES}" "${CONFIGURE_FILE_TEST}")
