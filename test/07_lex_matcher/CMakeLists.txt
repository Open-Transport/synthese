
include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")

set(DEPS
  07_lexical_matcher
)

boost_test(LexicalMatcher "${DEPS}")
