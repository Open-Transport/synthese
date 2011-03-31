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

install(TARGETS 01_util_threads DESTINATION lib)
