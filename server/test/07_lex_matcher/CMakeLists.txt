
include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")

set(DEPS
  00_framework
  07_lexical_matcher
  10_db
  15_server
  10_db
  11_cms
)

boost_test(LexicalMatcher "${DEPS}")
