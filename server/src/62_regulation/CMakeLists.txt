if(WITH_62_REGULATION)

  set_source_groups()

  set(regulation_SRCS
    RegulationModule.cpp
    RegulationModule.gen.cpp

    RegulationModuleRegister.cpp
    RegulationModule.hpp
    RegulationModule.inc.cpp

    RegulationRight.cpp
    RegulationRight.hpp

    AlertProcessingThreadExec.hpp
    AlertProcessingThreadExec.cpp
    AlertProcessor.hpp
    CallbackRequestAlertProcessor.hpp
    CallbackRequestAlertProcessor.cpp
    Alert.hpp
    Alert.cpp
    AlertTableSync.hpp
    AlertTableSync.cpp
    AlertListService.hpp
    AlertListService.cpp
    ProcessAlertsAction.hpp
    ProcessAlertsAction.cpp

    #CheckLineCalendarFunction.cpp
    #CheckLineCalendarFunction.hpp
    #GenerateVehicleServiceUsagesAction.cpp
    #GenerateVehicleServiceUsagesAction.hpp
    #IsochronAdmin.cpp
    #IsochronAdmin.hpp
    #IsochronService.cpp
    #IsochronService.hpp
    #PTQualityControlAdmin.cpp
    #PTQualityControlAdmin.hpp
    #ServiceLengthService.cpp
    #ServiceLengthService.hpp
    #ServiceDensityService.cpp
    #ServiceDensityService.hpp
    #VehicleServiceUsage.cpp
    #VehicleServiceUsage.hpp
    #VehicleServiceUsagesListService.cpp
    #"VehicleServiceUsagesListService.hpp
    #VehicleServiceUsageTableSync.cpp
    #VehicleServiceUsageTableSync.hpp
)

set_source_files_properties(RegulationModule.gen.cpp RegulationModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(62_regulation ${regulation_SRCS})

target_link_libraries(62_regulation
#  53_pt_journey_planner
#  56_pt_website
)

install(TARGETS 62_regulation DESTINATION lib)

endif(WITH_62_REGULATION)
