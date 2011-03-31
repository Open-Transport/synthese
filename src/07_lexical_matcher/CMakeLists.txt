
set(lexical_matcher_SRCS
FrenchPhoneticString.cpp
FrenchPhoneticString.h
FrenchSentence.cpp
FrenchSentence.h
LexicalMatcher.h
#old_main.cpp
)

add_library(07_lexical_matcher ${lexical_matcher_SRCS})

install(TARGETS 07_lexical_matcher DESTINATION lib)
