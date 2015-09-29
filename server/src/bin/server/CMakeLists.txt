set_source_groups()
# Build the generated.cpp.inc and includes.cpp.inc files.

set(gen_cpp_content "")
set(inc_cpp_content "")
foreach(module ${MODULES})
  file(GLOB gen_cpp_filename ${PROJECT_SOURCE_DIR}/src/${module}/*.gen.cpp)
  if(NOT ${gen_cpp_filename})
    file(READ ${gen_cpp_filename} content)
    set(gen_cpp_content "${gen_cpp_content}\n// ${gen_cpp_filename}\n${content}")
  endif()
  file(GLOB inc_cpp_filename ${PROJECT_SOURCE_DIR}/src/${module}/*.inc.cpp)
  if(NOT ${inc_cpp_filename})
    file(READ ${inc_cpp_filename} content)
    set(inc_cpp_content "${inc_cpp_content}\n// ${inc_cpp_filename}\n${content}")
  endif()
endforeach(module)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated.cpp.inc" "${gen_cpp_content}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/includes.cpp.inc" "${inc_cpp_content}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# include directories needed by the generated factory includes.
# Could be removed once each modules exports a single register entry point.
include_directories(${SPATIALITE_INCLUDE_DIRS})
include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${EXPAT_INCLUDE_DIRS})


add_executable(s3-server main.cpp)

if(WIN32)
  # To prevent "LINK : fatal error LNK1210: exceeded internal ILK size limit; link with /INCREMENTAL:NO"
  set_target_properties(s3-server PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif()

foreach(module ${MODULES})
  # Don't include submodules
  if(NOT ${module} MATCHES ".*/.*")
    #target_link_libraries(s3-server ${module})
  endif()
endforeach(module)

target_link_libraries(s3-server 00_framework)
target_link_libraries(s3-server 01_util)
target_link_libraries(s3-server 05_html)
target_link_libraries(s3-server 07_lexical_matcher)
target_link_libraries(s3-server 10_db)
target_link_libraries(s3-server 11_cms)
target_link_libraries(s3-server 12_security)
target_link_libraries(s3-server 13_dblog)
target_link_libraries(s3-server 14_admin)
target_link_libraries(s3-server 15_server)
target_link_libraries(s3-server 16_impex)
target_link_libraries(s3-server 18_graph)
target_link_libraries(s3-server 20_tree)
target_link_libraries(s3-server 31_calendar)
target_link_libraries(s3-server 32_geography)
target_link_libraries(s3-server 33_algorithm)
target_link_libraries(s3-server 34_road)
target_link_libraries(s3-server 35_pt)
target_link_libraries(s3-server 37_pt_operation)
target_link_libraries(s3-server 38_vehicle)
target_link_libraries(s3-server 39_map)
target_link_libraries(s3-server 51_resa)
target_link_libraries(s3-server 53_pt_journey_planner)
target_link_libraries(s3-server 56_pt_website)
target_link_libraries(s3-server 59_road_journey_planner)
target_link_libraries(s3-server 63_settings)
target_link_libraries(s3-server 06_openstreetmap)
target_link_libraries(s3-server 17_messages)
target_link_libraries(s3-server 19_inter_synthese)
target_link_libraries(s3-server 30_fare)
target_link_libraries(s3-server 40_pdf_cms)
target_link_libraries(s3-server 54_departure_boards)
target_link_libraries(s3-server 55_timetables)
target_link_libraries(s3-server 62_regulation)

target_link_libraries(s3-server ${Boost_LIBRARIES})

if(UNIX)
  target_link_libraries(s3-server pthread dl)
endif()

install(TARGETS s3-server DESTINATION bin)
