set_source_groups()

set(framework_SRCS
CallableByThread.hpp
CastPointersVectorField.hpp
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
KillProcessException.h
KillProcessException.cpp
Language.cpp
Language.hpp
MinutesField.hpp
NumericField.cpp
NumericField.hpp
Object.hpp
ObjectBase.cpp
ObjectBase.hpp
ParametersMapField.cpp
ParametersMapField.hpp
PointerField.hpp
PointerField2.hpp
PointersSetField.hpp
PointersVectorField.hpp
PtimeField.cpp
PtimeField.hpp
PtreeRecord.cpp
PtreeRecord.hpp
Record.cpp
Record.hpp
SchemaMacros.hpp
SecondsField.hpp
SimpleMapField.hpp
SimpleObjectFieldDefinition.hpp
StringField.cpp
StringField.hpp
StringVectorField.hpp
TimeField.cpp
TimeField.hpp
WithGeometry.hpp
)

add_library(00_framework ${framework_SRCS})

target_link_libraries(
  00_framework
  ${Boost_LIBRARIES}
  ${GEOS_LIBRARIES}
  ${GEOSC_LIBRARIES}
  ${PROJ_LIBRARIES}
)

install(TARGETS 00_framework DESTINATION lib)
