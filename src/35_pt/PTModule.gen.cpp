
// PT MODULE 35

// Factories

synthese::pt::TransportNetworkTableSync::integrate();

synthese::pt::PTUseRuleTableSync::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::pt::PhysicalStopTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Vertex>, synthese::pt::PhysicalStopTableSync>::integrate();
synthese::pt::JunctionTableSync::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::pt::ContinuousServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::SchedulesBasedService>, synthese::pt::ContinuousServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Service>, synthese::pt::ContinuousServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::pt::ScheduledServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::SchedulesBasedService>, synthese::pt::ScheduledServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Service>, synthese::pt::ScheduledServiceTableSync>::integrate();
synthese::pt::FareTableSync::integrate();
synthese::pt::RollingStockTableSync::integrate();
synthese::pt::NonConcurrencyRuleTableSync::integrate();
synthese::pt::ReservationContactTableSync::integrate();
synthese::pt::CommercialLineTableSync::integrate();
synthese::pt::LineTableSync::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::pt::StopAreaTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::geography::NamedPlace>, synthese::pt::StopAreaTableSync>::integrate();
synthese::pt::LineStopTableSync::integrate();

synthese::pt::PTModule::integrate();

synthese::pt::TridentFileFormat::integrate ();
synthese::pt::NavteqWithProjectionFileFormat::integrate();
synthese::pt::CarPostalFileFormat::integrate();

synthese::pt::PTStopsImportWizardAdmin::integrate();
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
synthese::pt::PTUseRuleAdmin::integrate();
synthese::pt::PTUseRulesAdmin::integrate();
synthese::pt::PTPhysicalStopAdmin::integrate();

synthese::pt::RealTimeUpdateFunction::integrate();
synthese::pt::PhysicalStopsCSVExportFunction::integrate();
synthese::pt::LinesListFunction::integrate();
synthese::pt::TridentExportFunction::integrate ();
synthese::pt::PTNetworksListFunction::integrate();
synthese::pt::PTRoutesListFunction::integrate();
synthese::pt::CheckLineCalendarFunction::integrate();
synthese::pt::PTObjectInformationFunction::integrate();

synthese::pt::LineMarkerInterfacePage::integrate();
synthese::pt::RealTimeUpdateScreenServiceInterfacePage::integrate();

synthese::pt::NonConcurrencyRuleAddAction::integrate();
synthese::pt::NonConcurrencyRuleRemoveAction::integrate();
synthese::pt::ScheduleRealTimeUpdateAction::integrate();
synthese::pt::ServiceVertexRealTimeUpdateAction::integrate();
synthese::pt::TransportNetworkAddAction::integrate();
synthese::pt::CommercialLineAddAction::integrate();
synthese::pt::LineAddAction::integrate();
synthese::pt::ServiceAddAction::integrate();
synthese::pt::StopAreaUpdateAction::integrate();
synthese::pt::CommercialLineCalendarTemplateUpdateAction::integrate();
synthese::pt::CommercialLineUpdateAction::integrate();
synthese::pt::ContinuousServiceUpdateAction::integrate();
synthese::pt::StopAreaNameUpdateAction::integrate();
synthese::pt::PTUseRuleAddAction::integrate();
synthese::pt::PTUseRuleUpdateAction::integrate();
synthese::pt::ServiceTimetableUpdateAction::integrate();
synthese::pt::ServiceUpdateAction::integrate();
synthese::pt::LineUpdateAction::integrate();
synthese::pt::ServiceApplyCalendarAction::integrate();
synthese::pt::LineStopAddAction::integrate();
synthese::pt::LineStopRemoveAction::integrate();
synthese::pt::ServiceDateChangeAction::integrate();
synthese::pt::PhysicalStopUpdateAction::integrate();
synthese::pt::PhysicalStopAddAction::integrate();
synthese::pt::StopAreaAddAction::integrate();
synthese::pt::LineStopUpdateAction::integrate();
synthese::pt::PhysicalStopMoveAction::integrate();

synthese::pt::TransportNetworkRight::integrate();

synthese::pt::StopArea::integrate();


// Registries
synthese::util::Env::Integrate<synthese::pt::JourneyPattern>();
synthese::util::Env::Integrate<synthese::pt::LineStop>();
synthese::util::Env::Integrate<synthese::pt::StopArea>();
synthese::util::Env::Integrate<synthese::pt::CommercialLine>();
synthese::util::Env::Integrate<synthese::pt::PhysicalStop>();
synthese::util::Env::Integrate<synthese::pt::Junction>();
synthese::util::Env::Integrate<synthese::pt::TransportNetwork>();
synthese::util::Env::Integrate<synthese::pt::PTUseRule>();
synthese::util::Env::Integrate<synthese::pt::ContinuousService>();
synthese::util::Env::Integrate<synthese::pt::ScheduledService>();
synthese::util::Env::Integrate<synthese::pt::RollingStock>();
synthese::util::Env::Integrate<synthese::pt::Fare>();
synthese::util::Env::Integrate<synthese::pt::NonConcurrencyRule>();
synthese::util::Env::Integrate<synthese::pt::ReservationContact>();

