
set(acapela_client_SRCS
PlaybackAcapela.cpp
PlaybackAcapela.h
)

add_library(08_acapela_client ${acapela_client_SRCS})

install(TARGETS 08_acapela_client DESTINATION lib)
