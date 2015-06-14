include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIR})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/05_html")
include_directories("${PROJECT_SOURCE_DIR}/src/07_lexical_matcher")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db/103_svn")
include_directories("${PROJECT_SOURCE_DIR}/src/12_security")
include_directories("${PROJECT_SOURCE_DIR}/src/14_admin")
include_directories("${PROJECT_SOURCE_DIR}/src/15_server")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/20_tree")
include_directories("${PROJECT_SOURCE_DIR}/src/11_cms")

set(DEPS
  103_db_svn
  20_tree
  59_road_journey_planner # from 56_pt_website
  56_pt_website # from cms
  11_cms # from pt
  19_inter_synthese
  54_departure_boards
  61_data_exchange
  37_pt_operation
)

boost_test(WebpageContent "${DEPS}")
boost_test(Website "${DEPS}")
