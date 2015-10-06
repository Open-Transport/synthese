# gcc warning settings
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Wextra -Wno-unused-parameter -Winit-self)
endif ()

macro(set_source_groups)
  # default group
  source_group("Sources" REGULAR_EXPRESSION "\\.(cpp|h|hpp)$")
  source_group("Fields" REGULAR_EXPRESSION ".*Field\\..*")
  source_group("01 Module" REGULAR_EXPRESSION ".*Module\\..*|.*ModuleRegister\\..*|.*\\gen\\.cpp|.*Types\\.(cpp|h|hpp)|.*Constants\\.(cpp|h|hpp).*")
  source_group("01 Exceptions" REGULAR_EXPRESSION ".*Exception\\..*")
  source_group("10 Table Syncs" REGULAR_EXPRESSION ".*TableSync\\..*")
  source_group("12 Rights" REGULAR_EXPRESSION ".*Right\\..*|RightTemplate\\.h")
  source_group("13 DB Logs" REGULAR_EXPRESSION ".*Log\\..*")
  source_group("14 Admin" REGULAR_EXPRESSION ".*Admin\\..*")
  source_group("15 Actions" REGULAR_EXPRESSION ".*Action\\..*")
  source_group("15 Services" REGULAR_EXPRESSION ".*Service\\..*|.*Function\\..*|.*CMSExporters\\..*")
  source_group("16 File Formats" REGULAR_EXPRESSION ".*FileFormat\\..*")
  source_group("17 Alarm recipients" REGULAR_EXPRESSION ".*AlarmRecipient\\..*")
  source_group("19 Inter-SYNTHESE" REGULAR_EXPRESSION ".*InterSYNTHESE\\..*")
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
10_db/103_svn
11_cms
12_security
13_dblog
14_admin
15_server
16_impex
17_messages
18_graph
19_inter_synthese
20_tree
30_fare
31_calendar
32_geography
33_algorithm
34_road
35_pt
37_pt_operation
38_vehicle
39_map
40_pdf_cms
51_resa
53_pt_journey_planner
54_departure_boards
55_timetables
56_pt_website
59_road_journey_planner
60_analysis
61_data_exchange
63_settings)

if(WITH_MYSQL)
  list(APPEND MODULES 10_db/102_mysql)
endif()

if(WITH_SCOM)
  list(APPEND MODULES 62_scom)
  add_definitions(-DWITH_SCOM)
endif()

if (WITH_INEO_TERMINUS)
  list(APPEND MODULES 61_ineo_terminus)
endif()

if(WITH_REGULATION)
  list(APPEND MODULES 62_regulation)
endif()

include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories(${CMAKE_CURRENT_BINARY_DIR})


foreach(module ${MODULES})
  # TODO: each module should only have an include path with the current module group or its dependencies.
  include_directories("${PROJECT_SOURCE_DIR}/src/${module}")
endforeach(module)

if(UNIX)
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
