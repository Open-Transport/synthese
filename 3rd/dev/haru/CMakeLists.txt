file(GLOB haru_SRCS "src/*.c" "include/*.h")

find_package(png REQUIRED)
find_package(zlib REQUIRED)

# create hpdf_config.h
CONFIGURE_FILE(
  ${HARU_INCLUDE_DIRS}/hpdf_config.h.cmake
  ${HARU_INCLUDE_DIRS}/hpdf_config.h
)

add_definitions(-D_USE_MATH_DEFINES)

include_directories(${HARU_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})

add_library(haru ${haru_SRCS})

install(TARGETS haru DESTINATION lib)

target_link_libraries(haru zlib png)