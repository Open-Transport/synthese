

// 14 Geography

// Factories
synthese::geography::CityTableSync::integrate();
synthese::geography::CityNameValueInterfaceElement::integrate();
synthese::geography::GeographyModule::integrate();
synthese::geography::PlaceAliasTableSync::integrate();

// Registries
synthese::util::Env::Integrate<synthese::geography::City>();
synthese::util::Env::Integrate<synthese::geography::PlaceAlias>();

