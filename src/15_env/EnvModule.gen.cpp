
// ENVMODULE 35

//Factories
synthese::env::LineMarkerInterfacePage::integrate();
synthese::env::ReservationRuleInterfacePage::integrate();

synthese::env::LogicalStopNameValueInterfaceElement::integrate();
synthese::env::CityNameValueInterfaceElement::integrate();
synthese::env::LineStyleInterfaceElement::integrate();
synthese::env::LineShortNameInterfaceElement::integrate();
synthese::env::LineImageURLInterfaceElement::integrate();

synthese::env::FareTableSync::integrate();
synthese::env::ReservationContactTableSync::integrate();
synthese::env::RollingStockTableSync::integrate();

synthese::env::CityTableSync::integrate();

synthese::env::CommercialLineTableSync::integrate();

synthese::env::LineTableSync::integrate();

synthese::env::ConnectionPlaceTableSync::integrate();
synthese::env::CrossingTableSync::integrate();
synthese::env::PublicPlaceTableSync::integrate();

synthese::env::PlaceAliasTableSync::integrate();
synthese::env::AddressTableSync::integrate();

synthese::env::PhysicalStopTableSync::integrate();

synthese::env::LineStopTableSync::integrate();

synthese::env::NonConcurrencyRuleTableSync::integrate();

synthese::env::RoadChunkTableSync::integrate();
synthese::env::ContinuousServiceTableSync::integrate();
synthese::env::ScheduledServiceTableSync::integrate();

synthese::env::ServiceDateTableSync::integrate();

synthese::env::TransportNetworkAdmin::integrate();
synthese::env::CommercialLineAdmin::integrate();
synthese::env::LineAdmin::integrate();

synthese::env::EnvModule::integrate();

synthese::env::LinesListFunction::integrate();

synthese::env::TransportNetworkRight::integrate();

// Registries
synthese::util::Env::Integrate<synthese::env::PhysicalStop>();
synthese::util::Env::Integrate<synthese::env::Line>();
synthese::util::Env::Integrate<synthese::env::Address>();
synthese::util::Env::Integrate<synthese::env::Crossing>();
synthese::util::Env::Integrate<synthese::env::LineStop>();
synthese::util::Env::Integrate<synthese::env::NonConcurrencyRule>();
synthese::util::Env::Integrate<synthese::env::PublicTransportStopZoneConnectionPlace>();
synthese::util::Env::Integrate<synthese::env::ReservationContact>();
synthese::util::Env::Integrate<synthese::env::CommercialLine>();
synthese::util::Env::Integrate<synthese::env::RollingStock>();
synthese::util::Env::Integrate<synthese::env::RoadChunk>();
synthese::util::Env::Integrate<synthese::env::City>();
synthese::util::Env::Integrate<synthese::env::ContinuousService>();
synthese::util::Env::Integrate<synthese::env::Fare>();
synthese::util::Env::Integrate<synthese::env::PlaceAlias>();
synthese::util::Env::Integrate<synthese::env::PublicPlace>();
synthese::util::Env::Integrate<synthese::env::ScheduledService>();
