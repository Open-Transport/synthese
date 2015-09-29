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
07_lexical_matcher
#08_acapela_client
#09_agi_client
10_db
10_db/101_sqlite
11_cms
12_security
13_dblog
14_admin
15_server
16_impex
18_graph
20_tree
31_calendar
32_geography
33_algorithm
34_road
35_pt
37_pt_operation
38_vehicle
39_map
51_resa
53_pt_journey_planner
56_pt_website
#57_accounting
59_road_journey_planner
63_settings)

if(WITH_MYSQL)
  list(APPEND MODULES 10_db/102_mysql)
endif()

option(WITH_06_OPENSTREETMAP "Module 06_openstreetmap" ON)
if(WITH_06_OPENSTREETMAP)
  add_definitions(-DWITH_06_OPENSTREETMAP)
  list(APPEND MODULES 06_openstreetmap)
endif()

option(WITH_17_MESSAGES "Module 17_messages" ON)
if(WITH_17_MESSAGES)
  list(APPEND MODULES 17_messages)
endif()

option(WITH_19_INTER_SYNTHESE "Module 19_inter_synthese" ON)
if(WITH_19_INTER_SYNTHESE)
  list(APPEND MODULES 19_inter_synthese)
endif()

option(WITH_30_FARE "Module 30_fare" ON)
if(WITH_30_FARE)
  list(APPEND MODULES 30_fare)
endif()

option(WITH_40_PDF_CMS "Module 40_PDF_CMS" ON)
if(WITH_40_PDF_CMS)
  list(APPEND MODULES 40_pdf_cms)
endif()

option(WITH_52_FORUM "Module 52_FORUM" OFF)
if(WITH_52_FORUM)
  list(APPEND MODULES 52_forum)
endif()

option(WITH_54_DEPARTURE_BOARDS "Module 54_DEPARTURE_BOARDS" ON)
if(WITH_54_DEPARTURE_BOARDS)
  list(APPEND MODULES 54_departure_boards)
endif()

option(WITH_55_TIMETABLES "Module 55_TIMETABLES" ON)
if(WITH_55_TIMETABLES)
  list(APPEND MODULES 55_timetables)
endif()

option(WITH_58_HIKING "Module 58_HIKING" OFF)
if(WITH_58_HIKING)
  list(APPEND MODULES 58_hiking)
endif()

option(WITH_60_ANALYSIS "Module 60_ANALYSIS" OFF)
if(WITH_60_ANALYSIS)
  list(APPEND MODULES 60_analysis)
endif()

option(WITH_62_REGULATION "Module 62_REGULATION" ON)
if(WITH_62_REGULATION)
  list(APPEND MODULES 62_regulation)
endif()

if(WITH_SCOM)
  list(APPEND MODULES 62_scom)
  add_definitions(-DWITH_SCOM)
endif()

option(WITH_61_DATA_EXCHANGE "Module 61_DATE_EXCHANGE" OFF)
if (WITH_61_DATA_EXCHANGE)
  list(APPEND MODULES 61_data_exchange)
endif()

option(WITH_61_INEO_TERMINUS "Module 61_INEO_TERMINUS" OFF)
if (WITH_61_INEO_TERMINUS)
  list(APPEND MODULES 61_ineo_terminus)
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
