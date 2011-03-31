include_directories("${PROJECT_SOURCE_DIR}/src")

set(DEPS
  01_util
)

boost_test(Conversion "${DEPS}")

boost_test(Log "${DEPS}")
# TODO: fails
boost_test(LowerCaseFilter "${DEPS}")
# TODO: fails
boost_test(PlainCharFilter "${DEPS}")
# TODO: doesn't compile
#boost_test(Registrable "${DEPS}")
# TODO: doesn't compile
#boost_test(SqlFilter "${DEPS}")
# TODO: doesn't compile
#boost_test(Uid "${DEPS}")

add_subdirectory(iostreams)
add_subdirectory(threads)
