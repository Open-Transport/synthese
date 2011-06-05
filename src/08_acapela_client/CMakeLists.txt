set_source_groups()

set(acapela_client_SRCS
PlaybackAcapela.cpp
PlaybackAcapela.h
)

add_library(08_acapela_client ${acapela_client_SRCS})

target_link_libraries(08_acapela_client
)

install(TARGETS 08_acapela_client DESTINATION lib)
