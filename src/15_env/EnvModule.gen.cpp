
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
synthese::env::BikeComplianceTableSync::integrate();
synthese::env::HandicappedComplianceTableSync::integrate();
synthese::env::PedestrianComplianceTableSync::integrate();
synthese::env::ReservationRuleTableSync::integrate();
synthese::env::RollingStockTableSync::integrate();

synthese::env::CityTableSync::integrate();
synthese::env::TransportNetworkTableSync::integrate();
synthese::env::AxisTableSync::integrate();

synthese::env::CommercialLineTableSync::integrate();

synthese::env::LineTableSync::integrate();
synthese::env::RoadTableSync::integrate();

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

synthese::env::TransportNetworkRight::integrate();

synthese::env::LinesListFunction::integrate();

// Registries
synthese::util::Env::template Integrate<synthese::env::PhysicalStop>();
synthese::util::Env::template Integrate<synthese::env::Line>();
synthese::util::Env::template Integrate<synthese::env::Road>();
synthese::util::Env::template Integrate<synthese::env::Address>();
synthese::util::Env::template Integrate<synthese::env::Axis>();
synthese::util::Env::template Integrate<synthese::env::BikeCompliance>();
synthese::util::Env::template Integrate<synthese::env::Crossing>();
synthese::util::Env::template Integrate<synthese::env::LineStop>();
synthese::util::Env::template Integrate<synthese::env::HandicappedCompliance>();
synthese::util::Env::template Integrate<synthese::env::NonConcurrencyRule>();
synthese::util::Env::template Integrate<synthese::env::TransportNetwork>();
synthese::util::Env::template Integrate<synthese::env::PublicTransportStopZoneConnectionPlace>();
synthese::util::Env::template Integrate<synthese::env::ReservationRule>();
synthese::util::Env::template Integrate<synthese::env::CommercialLine>();
synthese::util::Env::template Integrate<synthese::env::PedestrianCompliance>();
synthese::util::Env::template Integrate<synthese::env::RollingStock>();
synthese::util::Env::template Integrate<synthese::env::RoadChunk>();
synthese::util::Env::template Integrate<synthese::env::City>();
synthese::util::Env::template Integrate<synthese::env::ContinuousService>();
synthese::util::Env::template Integrate<synthese::env::Fare>();
synthese::util::Env::template Integrate<synthese::env::PlaceAlias>();
synthese::util::Env::template Integrate<synthese::env::PublicPlace>();
synthese::util::Env::template Integrate<synthese::env::ScheduledService>();
