# TODO: GLOB shouldn't be used. 
file(GLOB proj_SRCS "src/src/*.c")

foreach(exclude jniproj.c  nad2nad.c nad2bin.c geod.c cs2cs.c proj.c)
  list(REMOVE_ITEM proj_SRCS "${PROJECT_SOURCE_DIR}/3rd/dev/proj/src/src/${exclude}")
endforeach(exclude)

include_directories(${PROJ_INCLUDE_DIRS})

add_library(proj ${proj_SRCS})

install(TARGETS proj DESTINATION lib)

add_executable(proj-bin "${CMAKE_CURRENT_SOURCE_DIR}/src/src/proj.c")
target_link_libraries(proj-bin proj)
if(UNIX)
  target_link_libraries(proj-bin m)
endif(UNIX)
