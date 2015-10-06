set_source_groups()

set(agi_client_SRCS
cagi.c
cagi.h
)

add_library(09_agi_client ${agi_client_SRCS})

install(TARGETS 09_agi_client DESTINATION lib)
