file(GLOB zlib_SRCS "src/*.c" "src/*.h")
list(REMOVE_ITEM zlib_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/zlib/src/example.c")

include_directories(${ZLIB_INCLUDE_DIRS})

add_library(zlib ${zlib_SRCS})

install(TARGETS zlib DESTINATION lib)