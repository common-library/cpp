set(BUILD_TARGET "file")
set(ALIAS_NAME "lib::${BUILD_TARGET}")
set(TEST_BUILD_TARGET ${BUILD_TARGET}${POSTFIX_TEST_NAME})

set(HEADER
		FileManager.h
	)

set(SOURCE
		FileManager.cpp
	)

set(SOURCE_TEST
		test/main.cpp
		test/FileManagerTest.cpp
	)

set(DEPENDENCIES
		stdc++fs
	)

set(CONFIGURE_FILE_TEST
	)

func_build_library(${ALIAS_NAME} ${BUILD_TARGET} "${HEADER}" "${SOURCE}" "${DEPENDENCIES}")

func_build_unit_test(${TEST_BUILD_TARGET} "${SOURCE_TEST}" "${SOURCE}" "${DEPENDENCIES}" "${CONFIGURE_FILE_TEST}")
