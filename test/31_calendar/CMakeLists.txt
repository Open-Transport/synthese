include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/20_tree")
include_directories("${PROJECT_SOURCE_DIR}/src/31_calendar")

set(DEPS
  15_server # from db
  31_calendar
  36_cms # from admin
  56_pt_website # from cms
  59_road_journey_planner # from 56_pt_website
)

boost_test(Calendar "${DEPS}")
