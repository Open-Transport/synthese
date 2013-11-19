project(gps)

link_libraries(/usr/lib/libboost_system.so)

add_executable(gps gps.cpp main.cpp)
