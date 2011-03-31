
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
