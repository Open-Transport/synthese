
// ROADMODULE 34

// Factories

#include "CrossingTableSync.hpp"
#include "HouseTableSync.hpp"
#include "PublicPlaceTableSync.h"
#include "RoadChunkTableSync.h"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"

#include "RoadModule.h"

#include "IGNstreetsFileFormat.hpp"
#include "NavstreetsFileFormat.hpp"
#include "OSMFileFormat.hpp"

// Registries

#include "Crossing.h"
#include "House.hpp"
#include "MainRoadChunk.hpp"
#include "MainRoadPart.hpp"
#include "PublicPlace.h"
#include "RoadPlace.h"

#include "RoadModule.inc.cpp"

void synthese::road::moduleRegister()
{

	// ROAD MODULE 34

	// Factories

	synthese::road::RoadTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::road::CrossingTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Vertex>, synthese::road::CrossingTableSync>::integrate();
	synthese::road::RoadChunkTableSync::integrate();
	synthese::road::RoadPlaceTableSync::integrate();
	synthese::road::HouseTableSync::integrate();
	synthese::road::RoadModule::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::road::PublicPlaceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::geography::NamedPlace>, synthese::road::PublicPlaceTableSync>::integrate();
	synthese::road::RoadPlace::integrate();
	synthese::road::PublicPlace::integrate();
	synthese::road::NavstreetsFileFormat::integrate();
	synthese::road::OSMFileFormat::integrate();
	synthese::road::IGNstreetsFileFormat::integrate();

	// Registries

	synthese::util::Env::Integrate<synthese::road::MainRoadPart>();
	synthese::util::Env::Integrate<synthese::road::Crossing>();
	synthese::util::Env::Integrate<synthese::road::House>();
	synthese::util::Env::Integrate<synthese::road::RoadPlace>();
	synthese::util::Env::Integrate<synthese::road::MainRoadChunk>();
	synthese::util::Env::Integrate<synthese::road::PublicPlace>();
}
