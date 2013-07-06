file(GLOB png_SRCS "src/*.c" "src/*.h")
list(REMOVE_ITEM png_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/png/src/pnggccrd.c")
list(REMOVE_ITEM png_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/png/src/example.c")

if(not WIN32)
  add_definitions(-funroll-loops)
endif()

include_directories(${PNG_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})


add_library(png ${png_SRCS})

install(TARGETS png DESTINATION lib)

target_link_libraries(png zlib)
