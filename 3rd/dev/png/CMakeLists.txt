file(GLOB png_SRCS "src/*.c" "src/*.h")

if(not WIN32)
  add_definitions(-DPNG_USE_PNGGCCRD)
  add_definitions(-funroll-loops)
  add_definitions(-fomit-frame-pointer)
endif()

include_directories(${PNG_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})


add_library(png ${png_SRCS})

install(TARGETS png DESTINATION lib)

target_link_libraries(png zlib)
