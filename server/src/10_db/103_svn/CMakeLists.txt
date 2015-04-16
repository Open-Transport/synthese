set_source_groups()

set(svn_mysql_SRCS
SVNCheckoutAction.cpp
SVNCheckoutAction.hpp
SVNCommands.cpp
SVNCommands.hpp
SVNCommitAction.cpp
SVNCommitAction.hpp
SVNModule.cpp
SVNModule.hpp
SVNModule.gen.cpp
SVNModule.inc.cpp
SVNModuleRegister.cpp
SVNRepository.cpp
SVNRepository.hpp
SVNUpdateAction.cpp
SVNUpdateAction.hpp
SVNWorkingCopy.cpp
SVNWorkingCopy.hpp
SVNWorkingCopyCreateAction.cpp
SVNWorkingCopyCreateAction.hpp
)

set_source_files_properties(SVNModule.gen.cpp SVNModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(103_db_svn ${svn_mysql_SRCS})

target_link_libraries(
  103_db_svn
)

install(TARGETS 103_db_svn DESTINATION lib)
