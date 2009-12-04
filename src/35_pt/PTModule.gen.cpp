
// PT MODULE 35

// Factories

synthese::pt::TransportNetworkTableSync::integrate();
synthese::pt::TridentExportFunction::integrate ();
synthese::pt::TridentFileFormat::integrate ();
synthese::pt::ServiceDateTableSync::integrate();
synthese::pt::PTUseRuleTableSync::integrate();
synthese::pt::PTModule::integrate();
synthese::pt::NavteqWithProjectionFileFormat::integrate();
synthese::pt::NonConcurrencyRuleAddAction::integrate();
synthese::pt::NonConcurrencyRuleRemoveAction::integrate();
synthese::pt::ServiceAdmin::integrate();
synthese::pt::PTImportAdmin::integrate();
synthese::pt::RealTimeUpdateFunction::integrate();
synthese::pt::RealTimeUpdateScreenServiceInterfacePage::integrate();
synthese::pt::ScheduleRealTimeUpdateAction::integrate();
synthese::pt::ServiceVertexRealTimeUpdateAction::integrate();

// Registries

synthese::util::Env::Integrate<synthese::pt::TransportNetwork>();
synthese::util::Env::Integrate<synthese::pt::ServiceDate>();
synthese::util::Env::Integrate<synthese::pt::PTUseRule>();

