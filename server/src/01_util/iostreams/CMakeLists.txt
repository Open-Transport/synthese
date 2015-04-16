set_source_groups()

set(util_iostreams_SRCS
Archive.cpp
Archive.h
Compression.cpp
Compression.h
)

add_library(01_util_iostreams ${util_iostreams_SRCS})

target_link_libraries(01_util_iostreams
  ${Boost_LIBRARIES}
  01_util
)

install(TARGETS 01_util_iostreams DESTINATION lib)
