include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/20_tree")

set(DEPS
  20_tree
  11_cms # from admin
)

boost_test(Tree "${DEPS}")
