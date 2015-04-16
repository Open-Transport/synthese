set(spatialite_SRCS
src/spatialite.c
src/sqlite3.c
)

include_directories(${SPATIALITE_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})
include_directories(${GEOSC_INCLUDE_DIRS})
include_directories(${PROJ_INCLUDE_DIRS})

if(WIN32)
  include_directories("${ICONV_INCLUDE_DIRS}")
endif()

add_definitions(-DSQLITE_ENABLE_RTREE)
add_definitions(-DSPATIALITE_AMALGAMATION)
add_definitions(-DSQLITE_THREADSAFE=1)

add_library(spatialite ${spatialite_SRCS})

install(TARGETS spatialite DESTINATION lib)

target_link_libraries(spatialite proj)
target_link_libraries(spatialite geosc)

if(UNIX)
  target_link_libraries(spatialite dl)
endif(UNIX)

if(WIN32)
  target_link_libraries(spatialite iconv)
endif()

add_executable(spatialite-bin cli-src/shapefiles.c cli-src/shell.c)
target_link_libraries(spatialite-bin spatialite)
if(UNIX)
  target_link_libraries(spatialite-bin m)
  target_link_libraries(spatialite-bin pthread)
endif(UNIX)

install(TARGETS spatialite-bin DESTINATION bin)
