if(WITH_52_FORUM)

  set_source_groups()

  set(forum_SRCS
    ForumInterfacePage.cpp
    ForumInterfacePage.hpp
    ForumMessage.cpp
    ForumMessage.hpp
    ForumMessagePostAction.cpp
    ForumMessagePostAction.hpp
    ForumMessagesAdmin.cpp
    ForumMessagesAdmin.hpp
    ForumMessagesFunction.cpp
    ForumMessagesFunction.hpp
    ForumMessageTableSync.cpp
    ForumMessageTableSync.hpp
    ForumModule.cpp
    ForumModule.gen.cpp
    ForumModuleRegister.cpp
    ForumModule.hpp
    ForumModule.inc.cpp
    ForumRight.cpp
    ForumRight.hpp
    ForumTopic.cpp
    ForumTopic.hpp
    ForumTopicPostAction.cpp
    ForumTopicPostAction.hpp
    ForumTopicsAdmin.cpp
    ForumTopicsAdmin.hpp
    ForumTopicsFunction.cpp
    ForumTopicsFunction.hpp
    ForumTopicTableSync.cpp
    ForumTopicTableSync.hpp
  )

  set_source_files_properties(ForumModule.gen.cpp ForumModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

  add_library(52_forum ${forum_SRCS})

  target_link_libraries(52_forum
    01_util
    10_db
    11_cms
    12_security
)

install(TARGETS 52_forum DESTINATION lib)

endif(WITH_52_FORUM)
