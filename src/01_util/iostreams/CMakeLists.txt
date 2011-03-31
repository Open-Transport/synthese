set(util_iostreams_SRCS
Archive.cpp
Archive.h
Compression.cpp
Compression.h
)

add_library(01_util_iostreams ${util_iostreams_SRCS})

install(TARGETS 01_util_iostreams DESTINATION lib)
