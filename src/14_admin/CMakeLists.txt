
set(admin_SRCS
AdminActionFunctionRequest.hpp
AdminFunction.cpp
AdminFunction.h
AdminFunctionRequest.hpp
AdminInterfaceElement.cpp
AdminInterfaceElement.h
AdminInterfaceElementTemplate.h
AdminModule.cpp
AdminModule.h
AdminParametersException.cpp
AdminParametersException.h
HomeAdmin.cpp
HomeAdmin.h
LoginAdmin.cpp
LoginAdmin.hpp
ModuleAdmin.cpp
ModuleAdmin.h
)

add_library(14_admin ${admin_SRCS})

target_link_libraries(14_admin
  05_html
  10_db
  12_security
  # Forward dependencies:
  #15_server
  #36_cms
)

install(TARGETS 14_admin DESTINATION lib)
