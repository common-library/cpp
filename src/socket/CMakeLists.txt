set(BUILD_TARGET "socket")
set(ALIAS_NAME "lib::${BUILD_TARGET}")
set(TEST_BUILD_TARGET ${BUILD_TARGET}${POSTFIX_TEST_NAME})

set(HEADER
		SocketServer.h
		SocketClient.h
	)

set(SOURCE
		DualIp.cpp

		SocketServer.cpp
		SocketClient.cpp
	)

set(SOURCE_TEST
		test/main.cpp
		test/SocketServerTest.cpp
		test/SocketClientTest.cpp
	)

set(DEPENDENCIES
		lib::thread
	)

set(CONFIGURE_FILE_TEST
	)

func_build_library(${ALIAS_NAME} ${BUILD_TARGET} "${HEADER}" "${SOURCE}" "${DEPENDENCIES}")

func_build_unit_test(${TEST_BUILD_TARGET} "${SOURCE_TEST}" "${SOURCE}" "${DEPENDENCIES}" "${CONFIGURE_FILE_TEST}")
