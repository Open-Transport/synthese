file(GLOB png_SRCS "src/*.c" "src/*.h")

#find_package(zlib REQUIRED)

include_directories(${PNG_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})

add_library(png ${png_SRCS})

install(TARGETS png DESTINATION lib)

target_link_libraries(png zlib)
