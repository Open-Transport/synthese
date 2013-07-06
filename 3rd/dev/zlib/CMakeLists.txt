file(GLOB zlib_SRCS "src/*.c" "src/*.h")

include_directories(${ZLIB_INCLUDE_DIRS})

add_library(zlib ${zlib_SRCS})

install(TARGETS zlib DESTINATION lib)