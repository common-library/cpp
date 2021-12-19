cmake_minimum_required(VERSION 3.12)

project(common-library-cpp VERSION 1.0.0 LANGUAGES CXX)

include(./cmake/define.cmake)
include(./cmake/function.cmake)

add_subdirectory(src)
add_subdirectory(external_project)

set(LIBRARY_LIST
		config
		file
		json
		log
		process
		queue
		socket
		thread
		util
)
func_make_integrate_library("common" "${LIBRARY_LIST}")

if(ENABLE_TEST)
	func_make_coverage()
endif(ENABLE_TEST)
