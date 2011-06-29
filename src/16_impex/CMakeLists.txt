set_source_groups()

set(impex_SRCS
DataSource.cpp
DataSource.h
DataSourceAdmin.cpp
DataSourceAdmin.h
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
ImpExModule.gen.cpp
ImpExModuleRegister.cpp
ImpExModule.h
ImpExModule.inc.cpp
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

set_source_files_properties(ImpExModule.gen.cpp ImpExModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

if(WIN32)
  include_directories("${ICONV_INCLUDE_DIRS}")
endif()

add_library(16_impex ${impex_SRCS})

if(WIN32)
  target_link_libraries(16_impex iconv)
endif()

install(TARGETS 16_impex DESTINATION lib)
