
// ROADMODULE 34

// Factories

#include "RoadTableSync.h"
#include "RoadPlaceTableSync.h"
#include "HouseTableSync.hpp"
#include "CrossingTableSync.hpp"
#include "RoadChunkTableSync.h"
#include "RoadModule.h"
#include "PublicPlaceTableSync.h"
#include "NavstreetsFileFormat.hpp"
#include "OSMFileFormat.hpp"
#include "IGNstreetsFileFormat.hpp"

// Registries

#include "MainRoadPart.hpp"
#include "Crossing.h"
#include "RoadPlace.h"
#include "PublicPlace.h"
#include "MainRoadChunk.hpp"
#include "House.hpp"


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
	synthese::road::RoadModule::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::road::PublicPlaceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::geography::NamedPlace>, synthese::road::PublicPlaceTableSync>::integrate();
	synthese::road::RoadPlace::integrate();
	synthese::road::PublicPlace::integrate();
	synthese::road::NavstreetsFileFormat::integrate();
	synthese::road::OSMFileFormat::integrate();
	synthese::road::IGNstreetsFileFormat::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::road::HouseTableSync>::integrate();
	// Registries

	synthese::util::Env::Integrate<synthese::road::MainRoadPart>();
	synthese::util::Env::Integrate<synthese::road::Crossing>();
	synthese::util::Env::Integrate<synthese::road::House>();
	synthese::util::Env::Integrate<synthese::road::RoadPlace>();
	synthese::util::Env::Integrate<synthese::road::MainRoadChunk>();
	synthese::util::Env::Integrate<synthese::road::PublicPlace>();
}
