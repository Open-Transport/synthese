
// ROAD MODULE 34

// Factories

synthese::road::RoadTableSync::integrate();
synthese::road::AddressTableSync::integrate();
synthese::road::RoadChunkTableSync::integrate();
synthese::road::RoadPlaceTableSync::integrate();

// Registries

synthese::util::Env::Integrate<synthese::road::Road>();
synthese::util::Env::Integrate<synthese::road::Crossing>();
synthese::util::Env::Integrate<synthese::road::RoadPlace>();
synthese::util::Env::Integrate<synthese::road::Address>();
synthese::util::Env::Integrate<synthese::road::RoadChunk>();
