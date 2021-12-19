function(func_make_integrate_library BUILD_TARGET OBJECT_LIST)
	foreach(ITEM ${OBJECT_LIST})
		set(TARGET_OBJECT_LIST ${TARGET_OBJECT_LIST} $<TARGET_OBJECTS:${ITEM}${POSTFIX_OBJECT_NAME}>)
	endforeach()

	add_library(${BUILD_TARGET} STATIC ${TARGET_OBJECT_LIST})

	install(TARGETS ${BUILD_TARGET} DESTINATION ${INTEGRATE_INSTALL_PATH}/${LIBRARY_FOLDER_NAME})

	install_pkg_config(true ${BUILD_TARGET} "" "")
endfunction()

function(func_build_library ALIAS_NAME BUILD_TARGET HEADER SOURCE DEPENDENCIES)
	if(NOT "${SOURCE}" STREQUAL "")
		add_library(${BUILD_TARGET} STATIC ${SOURCE})
		add_library(${BUILD_TARGET}${POSTFIX_OBJECT_NAME} OBJECT ${SOURCE})
	else()
		add_library(${BUILD_TARGET} INTERFACE)
	endif()

	add_library(${ALIAS_NAME} ALIAS ${BUILD_TARGET})

	if(NOT "${HEADER}" STREQUAL "")
		install(FILES ${HEADER} DESTINATION ${INTEGRATE_INSTALL_PATH}/${INCLUDE_FOLDER_NAME})
		install(FILES ${HEADER} DESTINATION ${INDIVIDUAL_INSTALL_PATH}/${BUILD_TARGET}/${INCLUDE_FOLDER_NAME})
	endif()

	if(TARGET ${BUILD_TARGET})
		get_target_property(TARGET_TYPE ${BUILD_TARGET} TYPE)
		if("${TARGET_TYPE}" STREQUAL "STATIC_LIBRARY")
			target_include_directories(${BUILD_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
		elseif("${TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
			target_include_directories(${BUILD_TARGET} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
		endif()

		foreach(ITEM ${DEPENDENCIES})
			target_link_libraries(${BUILD_TARGET} ${ITEM})
			target_link_libraries(${BUILD_TARGET}${POSTFIX_OBJECT_NAME} ${ITEM})
		endforeach()

		install(TARGETS ${BUILD_TARGET} DESTINATION ${INDIVIDUAL_INSTALL_PATH}/${BUILD_TARGET}/${LIBRARY_FOLDER_NAME})
	endif()

	install_pkg_config(false ${BUILD_TARGET} "${HEADER}" "${DEPENDENCIES}")
endfunction()

function(func_build_unit_test TEST_BUILD_TARGET SOURCE_TEST SOURCE DEPENDENCIES CONFIGURE_FILE)
	if(ENABLE_TEST)
		foreach(ITEM ${CONFIGURE_FILE})
			string(LENGTH "${ITEM}" ITEM_SIZE)

			math(EXPR DESTINATION_FILE_SIZE "${ITEM_SIZE} - 3")

			string(SUBSTRING "${ITEM}" 0 ${DESTINATION_FILE_SIZE} DESTINATION_FILE)

			configure_file("${ITEM}" "${CMAKE_CURRENT_SOURCE_DIR}/${DESTINATION_FILE}")
		endforeach()

		add_executable(${TEST_BUILD_TARGET} ${SOURCE_TEST} ${SOURCE})

		set_target_properties(${TEST_BUILD_TARGET} PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS_COVERAGE})

		target_link_libraries(${TEST_BUILD_TARGET} gcov lib::googletest)
		foreach(ITEM ${DEPENDENCIES})
			target_link_libraries(${TEST_BUILD_TARGET} ${ITEM})
		endforeach()

		add_test(NAME ${TEST_BUILD_TARGET} COMMAND ${TEST_BUILD_TARGET})
	endif(ENABLE_TEST)
endfunction()

function(func_make_coverage)
	get_property(LCOV_SOURCE_DIR GLOBAL PROPERTY LCOV_SOURCE_DIR)
	get_property(LCOV_TO_COBERTURA_XML_SOURCE_DIR GLOBAL PROPERTY LCOV_TO_COBERTURA_XML_SOURCE_DIR)

	set(BINARY_LCOV ${LCOV_SOURCE_DIR}/bin/lcov)
	set(BINARY_GENHTML ${LCOV_SOURCE_DIR}/bin/genhtml)

	set(LCOV_FOLDER ${CMAKE_BINARY_DIR}/coverage/lcov)
	set(COVERAGE_RESULT_FOLDER ${CMAKE_BINARY_DIR}/coverage/html)

	set(LCOV_FILE_BASE ${LCOV_FOLDER}/coverage.base)
	set(LCOV_FILE_TEST ${LCOV_FOLDER}/coverage.test)
	set(LCOV_FILE_TOTAL ${LCOV_FOLDER}/coverage.total)
	set(LCOV_EXCEPT_PATH "*/usr/include/*" "*.h" "*/rabbit.hpp" "*/library/*" "*/test/main.cpp" "*Test.cpp")

	add_custom_target(lcov_run
			COMMAND rm -rf ${LCOV_FOLDER} ${COVERAGE_RESULT_FOLDER}
			COMMAND mkdir -p ${LCOV_FOLDER} ${COVERAGE_RESULT_FOLDER}
			COMMAND ${BINARY_LCOV} --capture --initial --directory ${CMAKE_BINARY_DIR} --output-file ${LCOV_FILE_BASE}
			COMMAND ${BINARY_LCOV} --capture --directory ${CMAKE_BINARY_DIR} --output-file ${LCOV_FILE_TEST}
			COMMAND ${BINARY_LCOV} --add-tracefile ${LCOV_FILE_BASE} --add-tracefile ${LCOV_FILE_TEST} --output-file ${LCOV_FILE_TOTAL}
			COMMAND ${BINARY_LCOV} --remove ${LCOV_FILE_TOTAL} ${LCOV_EXCEPT_PATH} --output-file ${LCOV_FILE_TOTAL}
			COMMAND ${BINARY_GENHTML} ${LCOV_FILE_TOTAL} --output-directory ${COVERAGE_RESULT_FOLDER}
			COMMAND python3 ${LCOV_TO_COBERTURA_XML_SOURCE_DIR}/lcov_cobertura/lcov_cobertura.py ${LCOV_FILE_TOTAL} -o ${COVERAGE_RESULT_FOLDER}/coverage.xml
		)
endfunction()

function(install_pkg_config IS_INTEGRATE BUILD_TARGET HEADER DEPENDENCIES)
	if(IS_INTEGRATE)
		set(PKG_CONFIG_CFLAGS "-I${INTEGRATE_INSTALL_PATH}/${INCLUDE_FOLDER_NAME}")
		set(PKG_CONFIG_LIBS "-L${INTEGRATE_INSTALL_PATH}/${LIBRARY_FOLDER_NAME} -l${BUILD_TARGET}")

		get_property(INTEGRATE_LIBS GLOBAL PROPERTY INTEGRATE_LINKER_FLAGS)
		string(CONCAT PKG_CONFIG_LIBS ${PKG_CONFIG_LIBS} " ${INTEGRATE_LIBS}")

		configure_file("${PROJECT_SOURCE_DIR}/cmake/pkg-config-template.pc.in" "${INTEGRATE_INSTALL_PATH}/${LIBRARY_FOLDER_NAME}/pkgconfig/${BUILD_TARGET}.pc" @ONLY)
	else()
		set(PKG_CONFIG_CFLAGS "")
		set(PKG_CONFIG_LIBS "")

		if(NOT "${HEADER}" STREQUAL "")
			string(CONCAT PKG_CONFIG_CFLAGS "-I${INDIVIDUAL_INSTALL_PATH}/${BUILD_TARGET}/${INCLUDE_FOLDER_NAME}")
		endif()

		if(TARGET ${BUILD_TARGET})
			string(CONCAT PKG_CONFIG_LIBS "-L${INDIVIDUAL_INSTALL_PATH}/${BUILD_TARGET}/${LIBRARY_FOLDER_NAME} -l${BUILD_TARGET}")

			foreach(ITEM ${DEPENDENCIES})
				if(NOT "${ITEM}" MATCHES "^lib::")
					string(CONCAT PKG_CONFIG_LIBS ${PKG_CONFIG_LIBS} " -l${ITEM}")

					get_property(INTEGRATE_LIBS GLOBAL PROPERTY INTEGRATE_LINKER_FLAGS)
					if("${INTEGRATE_LIBS}" STREQUAL "")
						string(CONCAT INTEGRATE_LIBS -l${ITEM})
					else()
						string(CONCAT INTEGRATE_LIBS ${INTEGRATE_LIBS} " -l${ITEM}")
					endif()
					set_property(GLOBAL PROPERTY INTEGRATE_LINKER_FLAGS "${INTEGRATE_LIBS}")
				elseif(NOT "${ITEM}" STREQUAL "lib::rabbit" AND NOT "${ITEM}" STREQUAL "lib::rapidjson")
					string(SUBSTRING ${ITEM} 5 -1 LIBRARY_NAME)

					string(CONCAT PKG_CONFIG_CFLAGS ${PKG_CONFIG_CFLAGS} " -I${INDIVIDUAL_INSTALL_PATH}/${LIBRARY_NAME}/${INCLUDE_FOLDER_NAME}")

					string(CONCAT PKG_CONFIG_LIBS ${PKG_CONFIG_LIBS} " -L${INDIVIDUAL_INSTALL_PATH}/${LIBRARY_NAME}/${LIBRARY_FOLDER_NAME} -l${LIBRARY_NAME}")
				endif()
			endforeach()
		endif()

		configure_file("${PROJECT_SOURCE_DIR}/cmake/pkg-config-template.pc.in" "${INDIVIDUAL_INSTALL_PATH}/${BUILD_TARGET}/${LIBRARY_FOLDER_NAME}/pkgconfig/${BUILD_TARGET}.pc" @ONLY)
	endif()
endfunction()
