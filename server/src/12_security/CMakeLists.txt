set_source_groups()

set(security_SRCS
AddProfileAction.cpp
AddProfileAction.h
AddRightAction.cpp
AddRightAction.h
AddUserAction.cpp
AddUserAction.h
CheckRightService.cpp
CheckRightService.hpp
DefaultRight.h
DeleteRightAction.cpp
DeleteRightAction.h
GlobalRight.cpp
GlobalRight.h
Profile.cpp
Profile.h
ProfileAdmin.cpp
ProfileAdmin.h
ProfilesAdmin.cpp
ProfilesAdmin.h
ProfileTableSync.cpp
ProfileTableSync.h
Right.cpp
Right.h
RightTemplate.h
SecurityConstants.hpp
SecurityLog.cpp
SecurityLog.h
SecurityModule.cpp
SecurityModule.gen.cpp
SecurityModuleRegister.cpp
SecurityModule.h
SecurityModule.inc.cpp
SecurityRight.cpp
SecurityRight.h
SecurityTypes.hpp
UpdateProfileAction.cpp
UpdateProfileAction.h
UpdateRightAction.cpp
UpdateRightAction.h
User.cpp
User.h
UserAdmin.cpp
UserAdmin.h
UserException.cpp
UserException.h
UserPasswordUpdateAction.cpp
UserPasswordUpdateAction.h
UserPasswordRecoveryAction.cpp
UserPasswordRecoveryAction.hpp
UserPasswordChangeAction.cpp
UserPasswordChangeAction.hpp
UserPrefPanel.h
UserPrefPanelTemplate.h
UsersAdmin.cpp
UsersAdmin.h
UsersListService.cpp
UsersListService.hpp
UserTableSync.cpp
UserTableSync.h
UserUpdateAction.cpp
UserUpdateAction.h
)

set_source_files_properties(SecurityModule.gen.cpp SecurityModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(12_security ${security_SRCS})

target_link_libraries(12_security
  01_util
  05_html
  10_db
  16_impex
  # Forward dependencies:
  #13_dblog
  #14_admin
  #15_server
)

install(TARGETS 12_security DESTINATION lib)
