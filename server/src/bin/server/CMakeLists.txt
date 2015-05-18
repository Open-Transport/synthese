set_source_groups()
# Build the generated.cpp.inc and includes.cpp.inc files.

set(gen_cpp_content "")
set(inc_cpp_content "")
foreach(module ${MODULES})
  file(GLOB gen_cpp_filename ${PROJECT_SOURCE_DIR}/src/${module}/*.gen.cpp)
  if(NOT ${gen_cpp_filename})
    file(READ ${gen_cpp_filename} content)
    set(gen_cpp_content "${gen_cpp_content}\n// ${gen_cpp_filename}\n${content}")
  endif()
  file(GLOB inc_cpp_filename ${PROJECT_SOURCE_DIR}/src/${module}/*.inc.cpp)
  if(NOT ${inc_cpp_filename})
    file(READ ${inc_cpp_filename} content)
    set(inc_cpp_content "${inc_cpp_content}\n// ${inc_cpp_filename}\n${content}")
  endif()
endforeach(module)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated.cpp.inc" "${gen_cpp_content}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/includes.cpp.inc" "${inc_cpp_content}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# include directories needed by the generated factory includes.
# Could be removed once each modules exports a single register entry point.
include_directories(${SPATIALITE_INCLUDE_DIRS})
include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${EXPAT_INCLUDE_DIRS})


add_executable(s3-server main.cpp)

if(WIN32)
  # To prevent "LINK : fatal error LNK1210: exceeded internal ILK size limit; link with /INCREMENTAL:NO"
  set_target_properties(s3-server PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif()

foreach(module ${MODULES})
  # Don't include submodules
  if(NOT ${module} MATCHES ".*/.*")
    target_link_libraries(s3-server ${module})
  endif()
endforeach(module)

target_link_libraries(s3-server ${Boost_LIBRARIES})
target_link_libraries(s3-server ${PYTHON_LIBRARY})

if(UNIX)
  target_link_libraries(s3-server pthread)
endif()

install(TARGETS s3-server DESTINATION bin)
