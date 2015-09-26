if(WITH_61_DATA_EXCHANGE)

  set_source_groups()

  set(data_exchange_SRCS
    CalendarFileFormat.cpp
    CalendarFileFormat.hpp
    CalendarOGTFileFormat.cpp
    CalendarOGTFileFormat.hpp
    CMSExport.cpp
    CMSExport.hpp
    CMSImport.cpp
    CMSImport.hpp
    DataExchangeModule.cpp
    DataExchangeModule.gen.cpp
    DataExchangeModule.inc.cpp
    DataExchangeModule.hpp
    DataExchangeModuleRegister.cpp
    DinoFileFormat.cpp
    DinoFileFormat.hpp
    GTFSFileFormat.cpp
    GTFSFileFormat.hpp
    GPSdFileFormat.cpp
    GPSdFileFormat.hpp
    GPSSimuFileFormat.cpp
    GPSSimuFileFormat.hpp
    HafasFileFormat.cpp
    HafasFileFormat.hpp
    HanoverTheoricalFileFormat.cpp
    HanoverTheoricalFileFormat.hpp
    HastusCSVFileFormat.cpp
    HastusCSVFileFormat.hpp
    HastusInterfaceFileFormat.cpp
    HastusInterfaceFileFormat.hpp
    HeuresFileFormat.cpp
    HeuresFileFormat.hpp
    HousesCSVFileFormat.cpp
    HousesCSVFileFormat.hpp
    IGNstreetsFileFormat.cpp
    IGNstreetsFileFormat.hpp
    IneoBDSIFileFormat.cpp
    IneoBDSIFileFormat.hpp
    IneoDepartureBoardsFileFormat.cpp
    IneoDepartureBoardsFileFormat.hpp
    IneoFileFormat.cpp
    IneoFileFormat.hpp
    IneoOperationFileFormat.cpp
    IneoOperationFileFormat.hpp
    IneoRealTimeFileFormat.cpp
    IneoRealTimeFileFormat.hpp
    NavstreetsFileFormat.cpp
    NavstreetsFileFormat.hpp
    ObitiFileFormat.cpp
    ObitiFileFormat.hpp
    OGTFileFormat.cpp
    OGTFileFormat.hpp
    PegaseFileFormat.cpp
    PegaseFileFormat.hpp
    PhysicalStopsCSVExportFunction.cpp
    PhysicalStopsCSVExportFunction.h
    POICSVFileFormat.cpp
    POICSVFileFormat.hpp
    PTDataCleanerFileFormat.cpp
    PTDataCleanerFileFormat.hpp
    PTFileFormat.cpp
    PTFileFormat.hpp
    PTOperationFileFormat.cpp
    PTOperationFileFormat.hpp
    RoadFileFormat.cpp
    RoadFileFormat.hpp
    RoadShapeFileFormat.cpp
    RoadShapeFileFormat.hpp
    RTMFileFormat.cpp
    RTMFileFormat.hpp
    RSSFileFormat.cpp
    RSSFileFormat.hpp
    ServicesCSVFileFormat.hpp
    ServicesCSVFileFormat.cpp
    StopsShapeFileFormat.cpp
    StopsShapeFileFormat.hpp
    TridentFileFormat.cpp
    TridentFileFormat.h
  )

  option(WITH_61_DATA_EXCHANGE_VDV "SubModule 61_DATA_EXCHANGE_VDV" ON)
  if(WITH_61_DATA_EXCHANGE_VDV)
    add_definitions(-DWITH_61_DATA_EXCHANGE_VDV)
    list(APPEND data_exchange_SRCS
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
      VDVStatusService.hpp)
  endif(WITH_61_DATA_EXCHANGE_VDV)

  option(WITH_61_DATA_EXCHANGE_VIX "SubModule 61_DATA_EXCHANGE_VIX" ON)
  if(WITH_61_DATA_EXCHANGE_VIX)
    add_definitions(-DWITH_61_DATA_EXCHANGE_VIX)
    list(APPEND data_exchange_SRCS
      VixV6000FileFormat.cpp
      VixV6000FileFormat.hpp
      vix/VIX-BSC-defines.hpp
      vix/VIX-CIntSurvMsg.cpp
      vix/VIX-CIntSurvMsg.hpp
      vix/VIX-Ccrc16.cpp
      vix/VIX-Ccrc16.hpp
      vix/VIX-ComPortMgr.cpp
      vix/VIX-ComPortMgr.hpp
      vix/VIX-SerialReader.cpp
      vix/VIX-SerialReader.hpp
      vix/VIX-rs232.cpp
      vix/VIX-rs232.hpp
      vix/VIX-timeutil.cpp
      vix/VIX-timeutil.hpp)
  endif(WITH_61_DATA_EXCHANGE_VIX)

  option(WITH_61_DATA_EXCHANGE_STOP_BUTTON_POLLER "SubModule 61_DATA_EXCHANGE_STOP_BUTTON_POLLER" OFF)
  if(WITH_61_DATA_EXCHANGE_STOP_BUTTON_POLLER)
    add_definitions(-DWITH_61_DATA_EXCHANGE_STOP_BUTTON_POLLER)
    list(APPEND data_exchange_SRCS
      StopButtonFilePoller.cpp
      StopButtonFilePoller.hpp)
  endif(WITH_61_DATA_EXCHANGE_STOP_BUTTON_POLLER)

  option(WITH_61_DATA_EXCHANGE_INEO_NCE "SubModule 61_DATA_EXCHANGE_INEO_NCE" OFF)
  if(WITH_61_DATA_EXCHANGE_INEO_NCE)
    add_definitions(-DWITH_61_DATA_EXCHANGE_INEO_NCE)
    list(APPEND data_exchange_SRCS
      IneoNCEConnection.cpp
      IneoNCEConnection.hpp
      NCEStatusService.cpp
      NCEStatusService.hpp)
  endif(WITH_61_DATA_EXCHANGE_INEO_NCE)

  option(WITH_61_DATA_EXCHANGE_MG_SCREEN "SubModule 61_DATA_EXCHANGE_MG_SCREEN" OFF)
  if(WITH_61_DATA_EXCHANGE_MG_SCREEN)
    add_definitions(-DWITH_61_DATA_EXCHANGE_MG_SCREEN)
    list(APPEND data_exchange_SRCS
      MGScreenConnection.cpp
      MGScreenConnection.hpp)
  endif(WITH_61_DATA_EXCHANGE_MG_SCREEN)

  if(WITH_06_OPENSTREETMAP)
    list(APPEND data_exchange_SRCS
      OSMFileFormat.cpp
      OSMFileFormat.hpp)
  endif(WITH_06_OPENSTREETMAP)

  set_source_files_properties(DataExchangeModule.gen.cpp DataExchangeModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

  include_directories(${EXPAT_INCLUDE_DIRS})

  add_library(61_data_exchange ${data_exchange_SRCS})

  target_link_libraries(61_data_exchange
    00_framework
    01_util
    10_db
    12_security
    16_impex
    17_messages
    18_graph
    20_tree
    31_calendar
    32_geography
    33_algorithm
    37_pt_operation
    38_vehicle
    ${EXPAT_LIBRARIES}
  )

  install(TARGETS 61_data_exchange DESTINATION lib)

  #add_subdirectory(testapps/ValidateurTestApp)

endif(WITH_61_DATA_EXCHANGE)
