# TODO: rename module to 53_pt_journey_planner

include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/05_html")
include_directories("${PROJECT_SOURCE_DIR}/src/07_lexical_matcher")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")
include_directories("${PROJECT_SOURCE_DIR}/src/11_interfaces")
include_directories("${PROJECT_SOURCE_DIR}/src/12_security")
include_directories("${PROJECT_SOURCE_DIR}/src/14_admin")
include_directories("${PROJECT_SOURCE_DIR}/src/15_server")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/18_graph")
include_directories("${PROJECT_SOURCE_DIR}/src/31_calendar")
include_directories("${PROJECT_SOURCE_DIR}/src/32_geography")
include_directories("${PROJECT_SOURCE_DIR}/src/33_algorithm")
include_directories("${PROJECT_SOURCE_DIR}/src/34_road")
include_directories("${PROJECT_SOURCE_DIR}/src/35_pt")
include_directories("${PROJECT_SOURCE_DIR}/src/51_resa")
include_directories("${PROJECT_SOURCE_DIR}/src/53_pt_journey_planner")

set(DEPS
  59_road_journey_planner # from 53_pt_journey_planner > 56_pt_website
  56_pt_website # from 53_pt_journey_planner
  53_pt_journey_planner
  51_resa # no dependency, missing?
  36_cms # from 53_pt_journey_planner > pt
)

# FIXME: tests don't compile since since rev 5043 (4/26/11)
#boost_test(IntegralSearch "${DEPS}")
#boost_test(PTRoutePlannerResult "${DEPS}")
#boost_test(RoutePlanner "${DEPS}")
