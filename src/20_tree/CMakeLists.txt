
set(tree_SRCS
AdminPageTreeTemplate.hpp
TreeAlphabeticalOrderingPolicy.hpp
TreeFolder.hpp
TreeFolderAddAction.cpp
TreeFolderAddAction.hpp
TreeFolderAdmin.cpp
TreeFolderAdmin.hpp
TreeFolderChild.hpp
TreeFolderRemoveAction.cpp
TreeFolderRemoveAction.hpp
TreeFolderRoot.cpp
TreeFolderRoot.hpp
TreeFolderTableSync.cpp
TreeFolderTableSync.hpp
TreeFolderUpdateAction.cpp
TreeFolderUpdateAction.hpp
TreeModule.cpp
TreeModule.h
TreeMultiClassRootPolicy.hpp
TreeNode.hpp
TreeOtherClassRootPolicy.hpp
TreeRankOrderingPolicy.hpp
TreeUniqueRootPolicy.hpp
)

add_library(20_tree ${tree_SRCS})

install(TARGETS 20_tree DESTINATION lib)
