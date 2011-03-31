
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

install(TARGETS 52_forum DESTINATION lib)
