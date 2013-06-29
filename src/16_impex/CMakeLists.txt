set_source_groups()

set(impex_SRCS
BaseImportableUpdateAction.cpp
BaseImportableUpdateAction.hpp
CleanObsoleteDataAction.cpp
CleanObsoleteDataAction.hpp
ConnectionImporter.hpp
DatabaseReadImporter.hpp
DataSource.cpp
DataSource.h
DataSourceAdmin.cpp
DataSourceAdmin.h
DataSourcesAdmin.cpp
DataSourcesAdmin.h
DataSourcesService.cpp
DataSourcesService.hpp
DataSourceLinksField.cpp
DataSourceLinksField.hpp
DataSourceTableSync.cpp
DataSourceTableSync.h
Exporter.hpp
ExportFunction.cpp
ExportFunction.hpp
FileFormat.cpp
FileFormat.h
FileFormatsService.cpp
FileFormatsService.hpp
FileFormatTemplate.h
ImpExModule.cpp
ImpExModule.gen.cpp
ImpExModuleRegister.cpp
ImpExModule.h
ImpExModule.inc.cpp
ImpExTypes.hpp
Import.cpp
Import.hpp
Importable.cpp
Importable.h
ImportableAdmin.cpp
ImportableAdmin.hpp
ImportableTableSync.cpp
ImportableTableSync.hpp
ImportableTemplate.hpp
Importer.cpp
Importer.hpp
ImportFunction.cpp
ImportFunction.h
ImportsService.cpp
ImportsService.hpp
ImportTableSync.cpp
ImportTableSync.hpp
MultipleFileTypesImporter.hpp
NoExportPolicy.hpp
NoImportPolicy.hpp
OneFileExporter.hpp
OneFileTypeImporter.hpp
)

set_source_files_properties(ImpExModule.gen.cpp ImpExModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(16_impex ${impex_SRCS})

install(TARGETS 16_impex DESTINATION lib)
