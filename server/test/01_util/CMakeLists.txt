include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")

set(DEPS
  00_framework
  01_util
  10_db
  11_cms
  12_security
  15_server
  16_impex
)

boost_test(Log "${DEPS}")
boost_test(ParametersMap "${DEPS}")
boost_test(Registrable "${DEPS}")
boost_test(UId "${DEPS}")

add_subdirectory(iostreams)
add_subdirectory(threads)
