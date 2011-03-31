add_definitions(-DBOOST_TEST_NO_LIB -DBOOST_AUTO_TEST_MAIN)

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
add_subdirectory(10_db)
