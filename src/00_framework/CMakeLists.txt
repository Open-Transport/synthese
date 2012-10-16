set_source_groups()

set(framework_SRCS
ComplexObjectFieldDefinition.hpp
CoordinatesSystem.cpp
CoordinatesSystem.hpp
DateField.cpp
DateField.hpp
DaysField.hpp
EnumObjectField.hpp
Exception.cpp
Exception.h
Field.cpp
Field.hpp
FilesMap.cpp
FilesMap.hpp
FrameworkTypes.hpp
GeometryField.cpp
GeometryField.hpp
Language.cpp
Language.hpp
MinutesField.hpp
NumericField.cpp
NumericField.hpp
Object.hpp
ObjectBase.cpp
ObjectBase.hpp
PointerField.hpp
PtimeField.cpp
PtimeField.hpp
Record.cpp
Record.hpp
SchemaMacros.hpp
SimpleMapField.hpp
SimpleObjectFieldDefinition.hpp
StringField.cpp
StringField.hpp
PointersVectorField.hpp
WithGeometry.hpp
)

add_library(00_framework ${framework_SRCS})

target_link_libraries(00_framework
  ${Boost_LIBRARIES}
  geos
  proj
)

install(TARGETS 00_framework DESTINATION lib)
