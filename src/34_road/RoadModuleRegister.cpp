
// ROADMODULE 34

// Factories

#include "CrossingTableSync.hpp"
#include "HouseTableSync.hpp"
#include "PublicPlaceTableSync.h"
#include "PublicPlaceEntranceTableSync.hpp"
#include "RoadChunkTableSync.h"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"

#include "ProjectAddressAction.hpp"
#include "PublicPlaceEntranceCreationAction.hpp"
#include "ProjectAllPublicPlacesAction.hpp"

#include "PublicPlacesListService.hpp"

#include "PublicPlaceAdmin.hpp"

#include "RoadModule.h"

// Registries

#include "Crossing.h"
#include "House.hpp"
#include "MainRoadChunk.hpp"
#include "MainRoadPart.hpp"
#include "PublicPlace.h"
#include "PublicPlaceEntrance.hpp"
#include "RoadPlace.h"

#include "RoadModule.inc.cpp"

void synthese::road::moduleRegister()
{

	// ROAD MODULE 34

	// Factories

	synthese::road::PublicPlaceEntranceTableSync::integrate();
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

	synthese::road::ProjectAddressAction::integrate();
	synthese::road::PublicPlaceEntranceCreationAction::integrate();
	synthese::road::ProjectAllPublicPlacesAction::integrate();

	synthese::road::PublicPlacesListService::integrate();

	synthese::road::PublicPlaceAdmin::integrate();

	// Registries

	synthese::util::Env::Integrate<synthese::road::Crossing>();
	synthese::util::Env::Integrate<synthese::road::House>();
	synthese::util::Env::Integrate<synthese::road::MainRoadChunk>();
	synthese::util::Env::Integrate<synthese::road::MainRoadPart>();
	synthese::util::Env::Integrate<synthese::road::PublicPlace>();
	synthese::util::Env::Integrate<synthese::road::PublicPlaceEntrance>();
	synthese::util::Env::Integrate<synthese::road::RoadPlace>();
}
