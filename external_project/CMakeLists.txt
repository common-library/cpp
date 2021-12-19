add_subdirectory(rabbit)
add_subdirectory(rapidjson)

if(ENABLE_TEST)
	add_subdirectory(googletest)
	add_subdirectory(lcov)
	add_subdirectory(lcov-to-cobertura-xml)
endif(ENABLE_TEST)
