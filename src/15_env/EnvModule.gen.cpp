
// ENVMODULE 35

//Factories
synthese::env::LineMarkerInterfacePage::integrate();


synthese::env::CommercialLineTableSync::integrate();

synthese::env::LineTableSync::integrate();

synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::env::ConnectionPlaceTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::geography::NamedPlace>, synthese::env::ConnectionPlaceTableSync>::integrate();

synthese::env::LineStopTableSync::integrate();

synthese::env::EnvModule::integrate();

synthese::env::PublicTransportStopZoneConnectionPlace::integrate();

// Registries
synthese::util::Env::Integrate<synthese::env::Line>();
synthese::util::Env::Integrate<synthese::env::LineStop>();
synthese::util::Env::Integrate<synthese::env::PublicTransportStopZoneConnectionPlace>();
synthese::util::Env::Integrate<synthese::env::CommercialLine>();
