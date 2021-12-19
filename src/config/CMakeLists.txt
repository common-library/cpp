set(BUILD_TARGET "config")
set(ALIAS_NAME "lib::${BUILD_TARGET}")
set(TEST_BUILD_TARGET ${BUILD_TARGET}${POSTFIX_TEST_NAME})

set(HEADER
		Config.h
		CommonConfig.h
	)

set(SOURCE
		Config.cpp
		CommonConfig.cpp
	)

set(SOURCE_TEST
		test/main.cpp
		test/CommonConfigTest.cpp
	)

set(DEPENDENCIES
		lib::log
		lib::json
	)

set(CONFIGURE_FILE_TEST
		"test/test.h.in"
	)

func_build_library(${ALIAS_NAME} ${BUILD_TARGET} "${HEADER}" "${SOURCE}" "${DEPENDENCIES}")

func_build_unit_test(${TEST_BUILD_TARGET} "${SOURCE_TEST}" "${SOURCE}" "${DEPENDENCIES}" "${CONFIGURE_FILE_TEST}")
