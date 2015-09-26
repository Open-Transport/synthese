if(WITH_60_ANALYSIS)

  set_source_groups()

  set(analysis_SRCS
    AnalysisModule.cpp
    AnalysisModule.gen.cpp
    AnalysisModuleRegister.cpp
    AnalysisModule.hpp
    AnalysisModule.inc.cpp
    AnalysisRight.cpp
    AnalysisRight.hpp
    CheckLineCalendarFunction.cpp
    CheckLineCalendarFunction.hpp
    GenerateVehicleServiceUsagesAction.cpp
    GenerateVehicleServiceUsagesAction.hpp
    IsochronAdmin.cpp
    IsochronAdmin.hpp
    IsochronService.cpp
    IsochronService.hpp
    PTQualityControlAdmin.cpp
    PTQualityControlAdmin.hpp
    ServiceLengthService.cpp
    ServiceLengthService.hpp
    ServiceDensityService.cpp
    ServiceDensityService.hpp
    VehicleServiceUsage.cpp
    VehicleServiceUsage.hpp
    VehicleServiceUsagesListService.cpp
    VehicleServiceUsagesListService.hpp
    VehicleServiceUsageTableSync.cpp
    VehicleServiceUsageTableSync.hpp
  )

  set_source_files_properties(AnalysisModule.gen.cpp AnalysisModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

  add_library(60_analysis ${analysis_SRCS})

  target_link_libraries(60_analysis
    53_pt_journey_planner
    56_pt_website
  )

  install(TARGETS 60_analysis DESTINATION lib)

endif(WITH_60_ANALYSIS)
