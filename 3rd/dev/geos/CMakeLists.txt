if(MSVC)
  set(input_suffix vc)
else()
  set(input_suffix in)
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/headers/geos/platform.h.${input_suffix}"
    "${PROJECT_BINARY_DIR}/3rd/dev/geos/src/headers/geos/platform.h"
    COPYONLY
)
configure_file(
    "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/headers/geos/version.h.${input_suffix}"
    "${PROJECT_BINARY_DIR}/3rd/dev/geos/src/headers/geos/version.h"
    COPYONLY
)

file(GLOB_RECURSE geos_SRCS "src/*.cpp")

file(GLOB_RECURSE excludes "src/examples/*.cpp")
list(REMOVE_ITEM geos_SRCS ${excludes})

list(REMOVE_ITEM geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/geom/GeometryFactory.cpp")
list(APPEND geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/geom/GeometryFactory.cpp")

list(REMOVE_ITEM geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/io/WKBReader.cpp")
list(APPEND geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/io/WKBReader.cpp")

include_directories(${GEOS_INCLUDE_DIRS})

add_library(geos ${geos_SRCS})

install(TARGETS geos DESTINATION lib)

# geosc

configure_file(
    "${PROJECT_SOURCE_DIR}/3rd/dev/geos/capi/geos_c.h.in"
    "${PROJECT_BINARY_DIR}/3rd/dev/geos/capi/geos_c.h"
    COPYONLY
)

include_directories(${GEOSC_INCLUDE_DIRS})

set(geosc_SRCS
capi/geos_c.cpp
capi/geos_c.h
capi/geos_ts_c.cpp
)

add_library(geosc ${geosc_SRCS})

install(TARGETS geosc DESTINATION lib)

target_link_libraries(geosc geos)
