add_definitions(-DBOOST_TEST_NO_LIB -DBOOST_AUTO_TEST_MAIN)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/test/common")

macro(boost_test name deps)
  string(REPLACE "${PROJECT_SOURCE_DIR}/test/" "" prefix ${CMAKE_CURRENT_SOURCE_DIR})
  string(REPLACE "/" "_" prefix ${prefix})
  set(target "test_${prefix}_${name}Test")
  add_executable(${target} ${name}Test.cpp ${ARGN})
  target_link_libraries(${target}
    ${Boost_LIBRARIES} ${deps})
  add_test(${target} ${target})
  if(DEFINED TESTS_ENV)
    set_tests_properties(${target} PROPERTIES ENVIRONMENT ${TESTS_ENV})
  endif()
endmacro(boost_test)


add_subdirectory(01_util)
add_subdirectory(05_html)
add_subdirectory(07_lex_matcher)
add_subdirectory(10_db)
add_subdirectory(14_geography)
add_subdirectory(18_graph)
add_subdirectory(34_road)
add_subdirectory(35_pt)
add_subdirectory(53_pt_routeplanner)
add_subdirectory(54_departure_boards)
