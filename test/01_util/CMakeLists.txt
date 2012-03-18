include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")

set(DEPS
  00_framework
  01_util
)

boost_test(Log "${DEPS}")
boost_test(Registrable "${DEPS}")
boost_test(UId "${DEPS}")

add_subdirectory(iostreams)
add_subdirectory(threads)
