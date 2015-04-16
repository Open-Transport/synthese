set_source_groups()

set(tree_SRCS
AdminPageTreeTemplate.hpp
TreeAlphabeticalOrderingPolicy.hpp
TreeFolder.cpp
TreeFolder.hpp
TreeFolderAdmin.hpp
TreeFolderDownNode.hpp
TreeFolderDownNodeInterface.cpp
TreeFolderDownNodeInterface.hpp
TreeFolderRoot.hpp
TreeFoldersService.cpp
TreeFoldersService.hpp
TreeFolderTableSync.cpp
TreeFolderTableSync.hpp
TreeFolderUpNode.cpp
TreeFolderUpNode.hpp
TreeModule.cpp
TreeModule.gen.cpp
TreeModuleRegister.cpp
TreeModule.h
TreeModule.inc.cpp
TreeMultiClassRootPolicy.hpp
TreeNode.hpp
TreeNodeField.hpp
TreeOtherClassRootPolicy.hpp
TreeRankOrderingPolicy.hpp
TreeRoot.hpp
TreeUniqueRootPolicy.hpp
)

set_source_files_properties(TreeModule.gen.cpp TreeModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(20_tree ${tree_SRCS})

target_link_libraries(20_tree
  10_db
  14_admin
)

install(TARGETS 20_tree DESTINATION lib)
