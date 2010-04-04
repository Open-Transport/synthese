
// PT MODULE 35

// Factories

synthese::pt::TransportNetworkTableSync::integrate();
synthese::pt::TridentFileFormat::integrate ();

synthese::pt::PTUseRuleTableSync::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::pt::PhysicalStopTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Vertex>, synthese::pt::PhysicalStopTableSync>::integrate();
synthese::pt::JunctionTableSync::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::pt::ContinuousServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::SchedulesBasedService>, synthese::pt::ContinuousServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::NonPermanentService>, synthese::pt::ContinuousServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::pt::ScheduledServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::SchedulesBasedService>, synthese::pt::ScheduledServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::NonPermanentService>, synthese::pt::ScheduledServiceTableSync>::integrate();
synthese::pt::FareTableSync::integrate();
synthese::pt::RollingStockTableSync::integrate();
synthese::pt::NonConcurrencyRuleTableSync::integrate();
synthese::pt::ReservationContactTableSync::integrate();

synthese::pt::PTModule::integrate();

synthese::pt::NavteqWithProjectionFileFormat::integrate();

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

synthese::pt::RealTimeUpdateFunction::integrate();
synthese::pt::PhysicalStopsCSVExportFunction::integrate();
synthese::pt::LinesListFunction::integrate();
synthese::pt::TridentExportFunction::integrate ();
synthese::pt::PTNetworksListFunction::integrate();
synthese::pt::PTRoutesListFunction::integrate();
synthese::pt::CheckLineCalendarFunction::integrate();

synthese::pt::RealTimeUpdateScreenServiceInterfacePage::integrate();
synthese::pt::PTNetworkListItemInterfacePage::integrate();
synthese::pt::PTLinesListItemInterfacePage::integrate();
synthese::pt::PTRoutesListItemInterfacePage::integrate();
synthese::pt::ReservationRuleInterfacePage::integrate();

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

synthese::pt::TransportNetworkRight::integrate();

// Registries

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

