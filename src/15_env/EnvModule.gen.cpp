
// ENVMODULE 35

//Factories
synthese::env::LineMarkerInterfacePage::integrate();
synthese::env::ReservationRuleInterfacePage::integrate();

synthese::env::FareTableSync::integrate();
synthese::env::ReservationContactTableSync::integrate();
synthese::env::RollingStockTableSync::integrate();

synthese::env::CommercialLineTableSync::integrate();

synthese::env::LineTableSync::integrate();

synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::env::ConnectionPlaceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::geography::NamedPlace>, synthese::env::ConnectionPlaceTableSync>::integrate();

synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::env::PhysicalStopTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Vertex>, synthese::env::PhysicalStopTableSync>::integrate();

synthese::env::LineStopTableSync::integrate();

synthese::env::NonConcurrencyRuleTableSync::integrate();

synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::env::ContinuousServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::env::NonPermanentService>, synthese::env::ContinuousServiceTableSync>::integrate();

synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::env::ScheduledServiceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::env::NonPermanentService>, synthese::env::ScheduledServiceTableSync>::integrate();

synthese::env::EnvModule::integrate();

synthese::env::LinesListFunction::integrate();

synthese::env::PublicTransportStopZoneConnectionPlace::integrate();

// Registries
synthese::util::Env::Integrate<synthese::env::PhysicalStop>();
synthese::util::Env::Integrate<synthese::env::Line>();
synthese::util::Env::Integrate<synthese::env::LineStop>();
synthese::util::Env::Integrate<synthese::env::NonConcurrencyRule>();
synthese::util::Env::Integrate<synthese::env::PublicTransportStopZoneConnectionPlace>();
synthese::util::Env::Integrate<synthese::env::ReservationContact>();
synthese::util::Env::Integrate<synthese::env::CommercialLine>();
synthese::util::Env::Integrate<synthese::env::RollingStock>();
synthese::util::Env::Integrate<synthese::env::ContinuousService>();
synthese::util::Env::Integrate<synthese::env::Fare>();
synthese::util::Env::Integrate<synthese::env::ScheduledService>();
