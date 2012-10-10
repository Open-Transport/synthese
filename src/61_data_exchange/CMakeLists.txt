set_source_groups()

set(data_exchange_SRCS
  DataExchangeModule.cpp
  DataExchangeModule.gen.cpp
  DataExchangeModule.inc.cpp
  DataExchangeModule.hpp
  DataExchangeModuleRegister.cpp
  IneoNCEConnection.cpp
  IneoNCEConnection.hpp
  VDVSubscriptionService.cpp
  VDVSubscriptionService.hpp
  VDVClient.cpp
  VDVClient.hpp
  VDVClientsListService.cpp
  VDVClientsListService.hpp
  VDVClientSubscription.cpp
  VDVClientSubscription.hpp
  VDVClientTableSync.cpp
  VDVClientTableSync.hpp
  VDVDataReadyService.cpp
  VDVDataReadyService.hpp
  VDVDataSupplyService.cpp
  VDVDataSupplyService.hpp
  VDVServer.cpp
  VDVServer.hpp
  VDVServersListService.cpp
  VDVServersListService.hpp
  VDVServerSubscription.cpp
  VDVServerSubscription.hpp
  VDVServerSubscriptionTableSync.cpp
  VDVServerSubscriptionTableSync.hpp
  VDVServerTableSync.cpp
  VDVServerTableSync.hpp
  VDVStatusService.cpp
  VDVStatusService.hpp
)

set_source_files_properties(DataExchangeModule.gen.cpp DataExchangeModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

include_directories(${EXPAT_INCLUDE_DIRS})

add_library(61_data_exchange ${data_exchange_SRCS})

target_link_libraries(61_data_exchange
  00_framework
  01_util
  10_db
  11_interfaces
  12_security
  16_impex
  17_messages
  18_graph
  20_tree
  31_calendar
  32_geography
  33_algorithm
  38_vehicle
  expat
)

install(TARGETS 61_data_exchange DESTINATION lib)
