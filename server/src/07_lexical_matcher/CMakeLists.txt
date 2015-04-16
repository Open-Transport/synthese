set_source_groups()

set(lexical_matcher_SRCS
FrenchPhoneticString.cpp
FrenchPhoneticString.h
FrenchSentence.cpp
FrenchSentence.h
LexicalMatcher.h
#old_main.cpp
)

add_library(07_lexical_matcher ${lexical_matcher_SRCS})

target_link_libraries(07_lexical_matcher
  01_util
)

install(TARGETS 07_lexical_matcher DESTINATION lib)
