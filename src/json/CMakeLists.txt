set(BUILD_TARGET "json")
set(ALIAS_NAME "lib::${BUILD_TARGET}")
set(TEST_BUILD_TARGET ${BUILD_TARGET}${POSTFIX_TEST_NAME})

set(HEADER
		Json.h
		JsonFactory.h
	)

set(SOURCE
		Json.cpp
		RabbitJson.cpp
		RapidjsonJson.cpp

		JsonFactory.cpp
	)

set(SOURCE_TEST
		test/main.cpp

		test/RabbitJsonTest.cpp
		test/RapidjsonJsonTest.cpp

		test/JsonFactoryTest.cpp
	)

set(DEPENDENCIES
		lib::file
		lib::util
		lib::rabbit
		lib::rapidjson
	)

set(CONFIGURE_FILE_TEST
	)

func_build_library(${ALIAS_NAME} ${BUILD_TARGET} "${HEADER}" "${SOURCE}" "${DEPENDENCIES}")

func_build_unit_test(${TEST_BUILD_TARGET} "${SOURCE_TEST}" "${SOURCE}" "${DEPENDENCIES}" "${CONFIGURE_FILE_TEST}")
