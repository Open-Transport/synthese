include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")

set(DEPS
  00_framework
  01_util
)

boost_test(Conversion "${DEPS}")

boost_test(Log "${DEPS}")
# TODO: fails
boost_test(LowerCaseFilter "${DEPS}")
# TODO: fails
boost_test(PlainCharFilter "${DEPS}")

boost_test(Registrable "${DEPS}")
# TODO: doesn't compile
#boost_test(SqlFilter "${DEPS}")
boost_test(Uid "${DEPS}")

add_subdirectory(iostreams)
add_subdirectory(threads)
