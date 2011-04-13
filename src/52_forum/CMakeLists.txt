
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
ForumModule.hpp
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

add_library(52_forum ${forum_SRCS})

target_link_libraries(52_forum
  01_util
  10_db
  11_interfaces
  12_security
  36_cms
)

install(TARGETS 52_forum DESTINATION lib)
