
set(framework_SRCS
CoordinatesSystem.cpp
CoordinatesSystem.hpp
Exception.cpp
Exception.h
Language.cpp
Language.hpp
WithGeometry.hpp
)

add_library(00_framework ${framework_SRCS})

target_link_libraries(00_framework
  geos
  proj
)

install(TARGETS 00_framework DESTINATION lib)
