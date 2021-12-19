set(BUILD_TARGET "design-pattern")
set(ALIAS_NAME "lib::${BUILD_TARGET}")
set(TEST_BUILD_TARGET ${BUILD_TARGET}${POSTFIX_TEST_NAME})

set(HEADER
		Singleton.h
	)

set(SOURCE
	)

set(SOURCE_TEST
		test/main.cpp
		test/SingletonTest.cpp
	)

set(DEPENDENCIES
	)

func_build_library(${ALIAS_NAME} ${BUILD_TARGET} "${HEADER}" "${SOURCE}" "${DEPENDENCIES}")

func_build_unit_test(${TEST_BUILD_TARGET} "${SOURCE_TEST}" "${SOURCE}" "${DEPENDENCIES}")
