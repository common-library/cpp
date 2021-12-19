option(ENABLE_TEST "-DENABLE_TEST=on" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS_COVERAGE " --coverage")

if("${CMAKE_INSTALL_PREFIX}" MATCHES "/usr/local")
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
endif()

set(INTEGRATE_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/integrate)
set(INDIVIDUAL_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/individual)
set(EXTERNAL_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/external)

set(INCLUDE_FOLDER_NAME "include")
set(LIBRARY_FOLDER_NAME "lib")

set(POSTFIX_OBJECT_NAME "_o")
set(POSTFIX_TEST_NAME "_test")

if(ENABLE_TEST)
	enable_testing()
endif(ENABLE_TEST)
