
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
synthese::pt::TransportNetworkAdmin::integrate();
synthese::pt::CommercialLineAdmin::integrate();
synthese::pt::LineAdmin::integrate();
synthese::pt::PTCitiesAdmin::integrate();
synthese::pt::PTPlacesAdmin::integrate();
synthese::pt::PTRoadsAdmin::integrate();
synthese::pt::PTRoadAdmin::integrate();
synthese::pt::PTNetworksAdmin::integrate();
synthese::pt::PTPlaceAdmin::integrate();

synthese::pt::RealTimeUpdateFunction::integrate();
synthese::pt::PhysicalStopsCSVExportFunction::integrate();

synthese::pt::RealTimeUpdateScreenServiceInterfacePage::integrate();
synthese::pt::ScheduleRealTimeUpdateAction::integrate();
synthese::pt::ServiceVertexRealTimeUpdateAction::integrate();
synthese::pt::TransportNetworkAddAction::integrate();
synthese::pt::CommercialLineAddAction::integrate();
synthese::pt::LineAddAction::integrate();

synthese::pt::TransportNetworkRight::integrate();

// Registries

synthese::util::Env::Integrate<synthese::pt::TransportNetwork>();
synthese::util::Env::Integrate<synthese::pt::ServiceDate>();
synthese::util::Env::Integrate<synthese::pt::PTUseRule>();

