if(WITH_55_TIMETABLES)

  set_source_groups()

  set(timetables_SRCS
    MultipleTimetableGenerateFunction.cpp
    MultipleTimetableGenerateFunction.hpp
    Timetable.cpp
    Timetable.h
    TimetableAddAction.cpp
    TimetableAddAction.h
    TimetableAdmin.cpp
    TimetableAdmin.h
    TimetableBuildService.cpp
    TimetableBuildService.hpp
    TimetableColumn.cpp
    TimetableColumn.h
    TimetableGenerateFunction.cpp
    TimetableGenerateFunction.h
    TimetableGenerator.cpp
    TimetableGenerator.h
    TimetableModule.cpp
    TimetableModule.gen.cpp
    TimetableModuleRegister.cpp
    TimetableModule.h
    TimetableModule.inc.cpp
    TimetableResult.cpp
    TimetableResult.hpp
    TimetableRight.cpp
    TimetableRight.h
    TimetableRow.cpp
    TimetableRow.h
    TimetableRowAddAction.cpp
    TimetableRowAddAction.h
    TimetableRowGroup.cpp
    TimetableRowGroup.hpp
    TimetableRowGroupItem.cpp
    TimetableRowGroupItem.hpp
    TimetableRowGroupItemAddAction.cpp
    TimetableRowGroupItemAddAction.hpp
    TimetableRowGroupItemTableSync.cpp
    TimetableRowGroupItemTableSync.hpp
    TimetableRowGroupTableSync.cpp
    TimetableRowGroupTableSync.hpp
    TimetableRowTableSync.cpp
    TimetableRowTableSync.h
    TimetableServiceRowInterfacePage.cpp
    TimetableServiceRowInterfacePage.h
    TimetableSetLineAction.cpp
    TimetableSetLineAction.h
    TimetableSetPhysicalStopAction.cpp
    TimetableSetPhysicalStopAction.h
    TimetableTableSync.cpp
    TimetableTableSync.h
    TimetableTransferUpdateAction.cpp
    TimetableTransferUpdateAction.hpp
    TimetableUpdateAction.cpp
    TimetableUpdateAction.h
    TimetableWarning.cpp
    TimetableWarning.h
  )

  set_source_files_properties(TimetableModule.gen.cpp TimetableModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

  add_library(55_timetables ${timetables_SRCS})

  target_link_libraries(55_timetables
    ${Boost_LIBRARIES}
    01_util
    10_db
    35_pt
  )

  install(TARGETS 55_timetables DESTINATION lib)

endif(WITH_55_TIMETABLES)
