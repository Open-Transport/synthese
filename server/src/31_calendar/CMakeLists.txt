set_source_groups()

set(calendar_SRCS
BaseCalendarAdmin.cpp
BaseCalendarAdmin.hpp
BaseCalendarUpdateAction.cpp
BaseCalendarUpdateAction.hpp
Calendar.cpp
Calendar.h
CalendarDateInterfacePage.cpp
CalendarDateInterfacePage.hpp
CalendarField.cpp
CalendarField.hpp
CalendarLink.cpp
CalendarLink.hpp
CalendarLinkTableSync.cpp
CalendarLinkTableSync.hpp
CalendarLinkUpdateAction.cpp
CalendarLinkUpdateAction.hpp
CalendarModule.cpp
CalendarModule.gen.cpp
CalendarModuleRegister.cpp
CalendarModule.h
CalendarModule.inc.cpp
CalendarRight.cpp
CalendarRight.h
CalendarTypes.h
CalendarsListService.cpp
CalendarsListService.hpp
CalendarTemplate.cpp
CalendarTemplate.h
CalendarTemplateAdmin.cpp
CalendarTemplateAdmin.h
CalendarTemplateCleanAction.cpp
CalendarTemplateCleanAction.hpp
CalendarTemplateElement.cpp
CalendarTemplateElement.h
CalendarTemplateElementAddAction.cpp
CalendarTemplateElementAddAction.h
CalendarTemplateElementTableSync.cpp
CalendarTemplateElementTableSync.h
CalendarTemplatePropertiesUpdateAction.cpp
CalendarTemplatePropertiesUpdateAction.h
CalendarTemplatesAdmin.cpp
CalendarTemplatesAdmin.h
CalendarTemplatesListFunction.cpp
CalendarTemplatesListFunction.hpp
CalendarTemplateTableSync.cpp
CalendarTemplateTableSync.h
)

set_source_files_properties(CalendarModule.gen.cpp CalendarModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(31_calendar ${calendar_SRCS})

target_link_libraries(31_calendar
  01_util
  10_db
  14_admin
  15_server
  # Forward dependencies:
  #11_cms
)

install(TARGETS 31_calendar DESTINATION lib)
