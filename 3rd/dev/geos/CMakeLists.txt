if(MSVC)
configure_file(
    "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/headers/geos/platform.h.vc"
    "${PROJECT_BINARY_DIR}/3rd/dev/geos/src/headers/geos/platform.h"
    COPYONLY
)
configure_file(
    "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/headers/geos/version.h.vc"
    "${PROJECT_BINARY_DIR}/3rd/dev/geos/src/headers/geos/version.h"
    COPYONLY
)
else()
# Use patched version instead.
configure_file(
    "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/headers/geos/platform.h.in"
    "${PROJECT_BINARY_DIR}/3rd/dev/geos/src/headers/geos/platform.h"
    COPYONLY
)
configure_file(
    "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/headers/geos/version.h.in"
    "${PROJECT_BINARY_DIR}/3rd/dev/geos/src/headers/geos/version.h"
    COPYONLY
)
endif()

file(GLOB_RECURSE geos_SRCS "src/*.cpp" "src/*.h")

file(GLOB_RECURSE excludes "src/examples/*.cpp" "src/examples/*.h")
list(REMOVE_ITEM geos_SRCS ${excludes})

# Patched files
list(REMOVE_ITEM geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/geom/GeometryFactory.cpp")
list(APPEND geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/geom/GeometryFactory.cpp")

list(REMOVE_ITEM geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/io/WKBReader.cpp")
list(APPEND geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/io/WKBReader.cpp")

list(REMOVE_ITEM geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/operation/distance/DistanceOp.cpp")
list(APPEND geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/operation/distance/DistanceOp.cpp")

list(REMOVE_ITEM geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/algorithm/LineIntersector.cpp")
list(APPEND geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/algorithm/LineIntersector.cpp")

list(REMOVE_ITEM geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/geom/LineSegment.cpp")
list(APPEND geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/geom/LineSegment.cpp")

configure_file(
    "${PROJECT_SOURCE_DIR}/3rd/dev/geos/patch/headers/geos/operation/overlay/OverlayNodeFactory.h"
    "${PROJECT_BINARY_DIR}/3rd/dev/geos/src/headers/geos/operation/overlay/OverlayNodeFactory.h"
    COPYONLY
)

list(REMOVE_ITEM geos_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/geos/src/headers/geos/operation/overlay/OverlayNodeFactory.h")
list(APPEND geos_SRCS "${PROJECT_BINARY_DIR}/3rd/dev/geos/src/headers/geos/operation/overlay/OverlayNodeFactory.h")

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
