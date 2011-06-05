
macro(set_source_groups)
  # default group
  source_group("Sources" REGULAR_EXPRESSION "\\.(cpp|h|hpp)$")
  source_group("01 Module" REGULAR_EXPRESSION ".*Module\\..*|.*\\gen\\.cpp|.*Types\\.h.*|.*Constants\\.h.*")
  source_group("01 Exceptions" REGULAR_EXPRESSION ".*Exception\\..*")
  source_group("10 Table Syncs" REGULAR_EXPRESSION ".*TableSync\\..*")
  source_group("11 Interface Elements" REGULAR_EXPRESSION ".*InterfaceElement\\..*")
  source_group("11 Interface Pages" REGULAR_EXPRESSION ".*InterfacePage\\..*")
  source_group("12 Rights" REGULAR_EXPRESSION ".*Right\\..*|RightTemplate\\.h")
  source_group("13 DB Logs" REGULAR_EXPRESSION ".*Log\\..*")
  source_group("14 Admin" REGULAR_EXPRESSION ".*Admin\\..*")
  source_group("15 Actions" REGULAR_EXPRESSION ".*Action\\..*")
  source_group("15 Services" REGULAR_EXPRESSION ".*Service\\..*|.*Function\\..*|.*CMSExporters\\..*")
  source_group("16 File Formats" REGULAR_EXPRESSION ".*FileFormat\\..*")
  source_group("17 Alarm recipients" REGULAR_EXPRESSION ".*AlarmRecipient\\..*")
  source_group("39 WFS Types" REGULAR_EXPRESSION ".*WFSType\\..*|WFSTypeTemplate\\.hpp")
endmacro(set_source_groups)

set(MODULES
00_framework
01_util
05_html
06_openstreetmap
07_lexical_matcher
#08_acapela_client
#09_agi_client
10_db
10_db/101_sqlite
11_interfaces
12_security
13_dblog
14_admin
15_server
16_impex
17_messages
18_graph
20_tree
31_calendar
32_geography
33_algorithm
34_road
35_pt
36_cms
37_pt_operation
39_map
51_resa
52_forum
53_pt_journey_planner
54_departure_boards
55_timetables
56_pt_website
#57_accounting
58_hiking
59_road_journey_planner)

if(WITH_MYSQL)
  list(APPEND MODULES 10_db/102_mysql)
endif()

include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src")

foreach(module ${MODULES})
  # TODO: each module should only have an include path with the current module group or its dependencies.
  include_directories("${PROJECT_SOURCE_DIR}/src/${module}")
endforeach(module)

if(UNIX)
  # TODO: only add to places where required.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

  # TODO: is this symbol required? (it is present with scons)
  # BOOST_ALL_DYN_LINK

  add_definitions(-DUNIX)
endif()

foreach(module ${MODULES})
  # Don't include submodules
  if(NOT ${module} MATCHES ".*/.*")
    add_subdirectory(${module})
  endif()
endforeach(module)

add_subdirectory(bin)
