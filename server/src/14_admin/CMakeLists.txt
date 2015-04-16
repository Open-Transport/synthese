set_source_groups()

set(admin_SRCS
AdminActionFunctionRequest.hpp
AdminFunctionRequest.hpp
AdminInterfaceElement.cpp
AdminInterfaceElement.h
AdminInterfaceElementTemplate.h
AdminModule.cpp
AdminModule.gen.cpp
AdminModuleRegister.cpp
AdminModule.h
AdminModule.inc.cpp
AdminPageDisplayService.cpp
AdminPageDisplayService.hpp
AdminParametersException.cpp
AdminParametersException.h
BaseAdminFunctionRequest.cpp
BaseAdminFunctionRequest.hpp
HomeAdmin.cpp
HomeAdmin.h
ModuleAdmin.cpp
ModuleAdmin.h
)

set_source_files_properties(AdminModule.gen.cpp AdminModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(14_admin ${admin_SRCS})

target_link_libraries(14_admin
  05_html
  10_db
  12_security
  # Forward dependencies:
  #15_server
  #11_cms
)

install(TARGETS 14_admin DESTINATION lib)
