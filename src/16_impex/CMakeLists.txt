set_source_groups()

set(impex_SRCS
CleanObsoleteDataAction.cpp
CleanObsoleteDataAction.hpp
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

add_library(16_impex ${impex_SRCS})

install(TARGETS 16_impex DESTINATION lib)
