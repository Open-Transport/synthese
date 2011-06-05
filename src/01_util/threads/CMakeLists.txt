set_source_groups()

set(util_threads_SRCS
ManagedThread.cpp
ManagedThread.h
Thread.cpp
Thread.h
ThreadAware.cpp
ThreadAware.h
ThreadException.cpp
ThreadException.h
ThreadExec.cpp
ThreadExec.h
ThreadGroup.cpp
ThreadGroup.h
ThreadManager.cpp
ThreadManager.h
)

add_library(01_util_threads ${util_threads_SRCS})

target_link_libraries(01_util_threads
  01_util
)

install(TARGETS 01_util_threads DESTINATION lib)
