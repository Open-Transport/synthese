set_source_groups()

set(ineo_terminus_SRCS
  IneoNotificationChannel.cpp
  IneoNotificationChannel.hpp
  IneoTerminusModule.cpp
  IneoTerminusModule.gen.cpp
  IneoTerminusModule.hpp
  IneoTerminusModule.inc.cpp
  IneoTerminusModuleRegister.cpp
  IneoTerminusConnection.cpp
  IneoTerminusConnection.hpp
)

set_source_files_properties(IneoTerminusModule.gen.cpp IneoTerminusModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

include_directories(${EXPAT_INCLUDE_DIRS})

add_library(61_ineo_terminus ${ineo_terminus_SRCS})

target_link_libraries(61_ineo_terminus
  01_util
  17_messages
  expat
)

install(TARGETS 61_ineo_terminus DESTINATION lib)
