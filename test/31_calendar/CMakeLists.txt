include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/20_tree")
include_directories("${PROJECT_SOURCE_DIR}/src/31_calendar")

set(DEPS
  31_calendar
)

boost_test(Calendar "${DEPS}")
