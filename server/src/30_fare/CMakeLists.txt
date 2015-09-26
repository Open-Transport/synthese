if(WITH_30_FARE)

  set_source_groups()

  set(pt_SRCS
    Fare.cpp
    Fare.hpp
    FareAdmin.cpp
    FareAdmin.hpp
    FaresAdmin.cpp
    FaresAdmin.hpp
    FareTableSync.cpp
    FareTableSync.hpp
    FareTicket.cpp
    FareTicket.hpp
    FareType.cpp
    FareType.hpp
    FareTypeDistance.cpp
    FareTypeDistance.hpp
    FareTypeFlatRate.cpp
    FareTypeFlatRate.hpp
    FareUpdateAction.cpp
    FareUpdateAction.hpp
    FareModule.cpp
    FareModule.gen.cpp
    FareModule.hpp
    FareModule.inc.cpp
    FareModuleRegister.cpp
  )

  set_source_files_properties(FareModule.gen.cpp FareModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

  add_library(30_fare ${pt_SRCS})

  target_link_libraries(30_fare
    00_framework
    01_util
    10_db
    12_security
    16_impex
  )

  install(TARGETS 30_fare DESTINATION lib)

endif(WITH_30_FARE)
