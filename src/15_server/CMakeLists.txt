set(server_SRCS
Action.h
ActionException.cpp
ActionException.h
BasicClient.cpp
BasicClient.h
CleanerThreadExec.cpp
CleanerThreadExec.h
ClientException.cpp
ClientException.h
DbModuleConfigTableSync.cpp
DbModuleConfigTableSync.h
DynamicRequest.cpp
DynamicRequest.h
Function.h
HtmlFormCalendarFieldInterfaceElement.cpp
HtmlFormCalendarFieldInterfaceElement.h
HtmlFormFieldInterfaceElement.cpp
HtmlFormFieldInterfaceElement.h
HtmlFormInterfaceElement.cpp
HtmlFormInterfaceElement.h
HTTPConnection.cpp
HTTPConnection.hpp
HTTPReply.cpp
HTTPReply.hpp
HTTPRequest.hpp
HTTPRequestParser.cpp
HTTPRequestParser.hpp
LoginAction.cpp
LoginAction.h
LoginInterfacePage.cpp
LoginInterfacePage.h
LogoutAction.cpp
LogoutAction.h
ModuleClass.cpp
ModuleClass.h
ModuleClassTemplate.hpp
ModuleException.h
ParametersMap.cpp
ParametersMap.h
QuitAction.cpp
QuitAction.hpp
random_device.cpp
Request.cpp
Request.h
RequestException.cpp
RequestException.h
ServerAdminRight.cpp
ServerAdminRight.h
ServerConstants.h
ServerModule.cpp
ServerModule.gen.cpp
ServerModule.h
ServerModule.inc.cpp
ServerModuleException.cpp
ServerModuleException.h
ServerModuleVersionInterfaceElement.cpp
ServerModuleVersionInterfaceElement.h
ServerTypes.h
Session.cpp
Session.h
SessionException.cpp
SessionException.h
StaticActionFunctionRequest.h
StaticActionRequest.h
StaticFunctionRequest.h
ThreadKillAction.cpp
ThreadKillAction.h
ThreadsAdmin.cpp
ThreadsAdmin.h
ValidSessionInterfaceElement.cpp
ValidSessionInterfaceElement.h
XMLSessionFunction.cpp
XMLSessionFunction.h
XMLSessionIsValidFunction.cpp
XMLSessionIsValidFunction.h
)

set_source_files_properties(ServerModule.gen.cpp ServerModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

# For compiling Boost random_device.cpp, this define has to be set.
# To be removed once migrated to Boost 1.45.
add_definitions(-DBOOST_RANDOM_DECL=)

add_library(15_server ${server_SRCS})

target_link_libraries(15_server
  01_util
  05_html
  10_db
  11_interfaces
  12_security
  13_dblog
  # Forward dependencies:
  #36_cms
)

install(TARGETS 15_server DESTINATION lib)
