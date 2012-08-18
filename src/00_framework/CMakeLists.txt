set_source_groups()

set(framework_SRCS
ComplexObjectField.hpp
ComplexObjectFieldDefinition.hpp
CoordinatesSystem.cpp
CoordinatesSystem.hpp
EnumObjectField.hpp
Exception.cpp
Exception.h
Field.cpp
Field.hpp
FilesMap.cpp
FilesMap.hpp
FrameworkTypes.hpp	
Language.cpp
Language.hpp
Object.hpp
ObjectBase.cpp
ObjectBase.hpp
Record.cpp
Record.hpp
SchemaMacros.hpp
SimpleObjectField.hpp
SimpleObjectFieldDefinition.hpp
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
