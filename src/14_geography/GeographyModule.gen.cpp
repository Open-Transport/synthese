

// 14 Geography

// Factories
synthese::geography::CityTableSync::integrate();
synthese::geography::CityNameValueInterfaceElement::integrate();
synthese::geography::GeographyModule::integrate();
synthese::geography::PlaceAliasTableSync::integrate();
synthese::geography::PlaceAlias::integrate();
synthese::geography::CityAliasTableSync::integrate();

synthese::geography::CityUpdateAction::integrate();

// Registries
synthese::util::Env::Integrate<synthese::geography::City>();
synthese::util::Env::Integrate<synthese::geography::CityAlias>();
synthese::util::Env::Integrate<synthese::geography::PlaceAlias>();

