
set(impex_SRCS
DataSource.cpp
DataSource.h
DataSourceAdmin.cpp
DataSourceAdmin.h
DataSourceRemoveAction.cpp
DataSourceRemoveAction.hpp
DataSourcesAdmin.cpp
DataSourcesAdmin.h
DataSourceTableSync.cpp
DataSourceTableSync.h
DataSourceUpdateAction.cpp
DataSourceUpdateAction.hpp
Exporter.hpp
ExportFunction.cpp
ExportFunction.hpp
FileFormat.h
FileFormatTemplate.h
ImpExModule.cpp
ImpExModule.h
Importable.cpp
Importable.h
ImportableAdmin.cpp
ImportableAdmin.hpp
ImportableTableSync.cpp
ImportableTableSync.hpp
Importer.hpp
ImportFunction.cpp
ImportFunction.h
MultipleFileTypesImporter.hpp
NoExportPolicy.hpp
NoImportPolicy.hpp
OneFileExporter.hpp
OneFileTypeImporter.hpp
)

if(WIN32)
  include_directories("${ICONV_INCLUDE_DIRS}")
endif()

add_library(16_impex ${impex_SRCS})

if(WIN32)
  target_link_libraries(16_impex iconv)
endif()

install(TARGETS 16_impex DESTINATION lib)
