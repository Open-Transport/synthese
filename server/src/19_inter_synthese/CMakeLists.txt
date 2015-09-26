if(WITH_19_INTER_SYNTHESE)

  set_source_groups()

    set(inter_synthese_SRCS
    InterSYNTHESEConfig.cpp
    InterSYNTHESEConfig.hpp
    InterSYNTHESEConfigTableSync.cpp
    InterSYNTHESEConfigTableSync.hpp
    InterSYNTHESEConfigItem.cpp
    InterSYNTHESEConfigItem.hpp
    InterSYNTHESEConfigItemTableSync.cpp
    InterSYNTHESEConfigItemTableSync.hpp
    InterSYNTHESEConfigsViewService.cpp
    InterSYNTHESEConfigsViewService.hpp
    InterSYNTHESEContent.cpp
    InterSYNTHESEContent.hpp
    InterSYNTHESEFileFormat.cpp
    InterSYNTHESEFileFormat.hpp
    InterSYNTHESEIsSynchronisingService.cpp
    InterSYNTHESEIsSynchronisingService.hpp
    InterSYNTHESEModule.cpp
    InterSYNTHESEModule.hpp
    InterSYNTHESEModule.gen.cpp
    InterSYNTHESEModule.inc.cpp
    InterSYNTHESEModuleRegister.cpp
    InterSYNTHESEObjectCreateService.cpp
    InterSYNTHESEObjectCreateService.hpp
    InterSYNTHESEObjectUpdateService.cpp
    InterSYNTHESEObjectUpdateService.hpp
    InterSYNTHESEPackage.cpp
    InterSYNTHESEPackage.hpp
    InterSYNTHESEPackageAutoLockAction.cpp
    InterSYNTHESEPackageAutoLockAction.hpp
    InterSYNTHESEPackageCommitAction.cpp
    InterSYNTHESEPackageCommitAction.hpp
    InterSYNTHESEPackageCommitService.cpp
    InterSYNTHESEPackageCommitService.hpp
    InterSYNTHESEPackageContent.cpp
    InterSYNTHESEPackageContent.hpp
    InterSYNTHESEPackageFileFormat.cpp
    InterSYNTHESEPackageFileFormat.hpp
    InterSYNTHESEPackageGetContentService.cpp
    InterSYNTHESEPackageGetContentService.hpp
    InterSYNTHESEPackageTableSync.cpp
    InterSYNTHESEPackageTableSync.hpp
    InterSYNTHESEPackagesService.cpp
    InterSYNTHESEPackagesService.hpp
    InterSYNTHESEPacket.cpp
    InterSYNTHESEPacket.hpp
    InterSYNTHESEQueue.cpp
    InterSYNTHESEQueue.hpp
    InterSYNTHESEQueueTableSync.cpp
    InterSYNTHESEQueueTableSync.hpp
    InterSYNTHESESlave.cpp
    InterSYNTHESESlave.hpp
    InterSYNTHESESlavesViewService.cpp
    InterSYNTHESESlavesViewService.hpp
    InterSYNTHESESlaveUpdateService.cpp
    InterSYNTHESESlaveUpdateService.hpp
    InterSYNTHESESlaveTableSync.cpp
    InterSYNTHESESlaveTableSync.hpp
    InterSYNTHESESyncTypeFactory.cpp
    InterSYNTHESESyncTypeFactory.hpp
    InterSYNTHESEUpdateAckService.cpp
    InterSYNTHESEUpdateAckService.hpp
    InterSYNTHESEUpdatePushService.cpp
    InterSYNTHESEUpdatePushService.hpp
    SpecificPostInstall.cpp
    SpecificPostInstall.hpp
)

  set_source_files_properties(InterSYNTHESEModule.gen.cpp InterSYNTHESEModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

  add_library(19_inter_synthese ${inter_synthese_SRCS})

  target_link_libraries(19_inter_synthese
    01_util
    # 15_server
  )

  install(TARGETS 19_inter_synthese DESTINATION lib)

endif(WITH_19_INTER_SYNTHESE)
