set_source_groups()

set(framework_SRCS
ComplexObjectField.hpp
CoordinatesSystem.cpp
CoordinatesSystem.hpp
Exception.cpp
Exception.h
Field.cpp
Field.hpp
FrameworkTypes.hpp	
Language.cpp
Language.hpp
Object.hpp
ObjectBase.hpp
ObjectField.hpp
Record.cpp
Record.hpp
SchemaMacros.hpp
StandardFields.cpp
StandardFields.hpp
WithGeometry.hpp
)

add_library(00_framework ${framework_SRCS})

target_link_libraries(00_framework
  ${Boost_LIBRARIES}
  geos
  proj
)

install(TARGETS 00_framework DESTINATION lib)
