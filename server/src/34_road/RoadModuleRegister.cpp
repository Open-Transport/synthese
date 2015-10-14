
// ROADMODULE 34

// Factories

#include "CrossingTableSync.hpp"
#include "HouseTableSync.hpp"
#include "PublicPlaceTableSync.h"
#include "PublicPlaceEntranceTableSync.hpp"
#include "RoadChunkTableSync.h"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"
#include "RoadPlaceUpdateAction.h"

#include "ProjectAddressAction.hpp"
#include "PublicPlaceEntranceCreationAction.hpp"
#include "ProjectAllPublicPlacesAction.hpp"

#include "PublicPlacesListService.hpp"

#include "PublicPlaceAdmin.hpp"

#include "RoadModule.h"

// Registries

#include "Crossing.h"
#include "House.hpp"
#include "RoadChunk.h"
#include "Road.h"
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
	synthese::road::RoadPlaceUpdateAction::integrate();
	synthese::road::PublicPlace::integrate();

	synthese::road::ProjectAddressAction::integrate();
	synthese::road::PublicPlaceEntranceCreationAction::integrate();
	synthese::road::ProjectAllPublicPlacesAction::integrate();

	synthese::road::PublicPlacesListService::integrate();

	synthese::road::PublicPlaceAdmin::integrate();

	// Registries
	INTEGRATE(synthese::road::Crossing);
	INTEGRATE(synthese::road::House);
	synthese::util::Env::Integrate<synthese::road::RoadChunk>();
	INTEGRATE(synthese::road::Road);
	INTEGRATE(synthese::road::PublicPlace);
	INTEGRATE(synthese::road::PublicPlaceEntrance);
	INTEGRATE(synthese::road::RoadPlace);
}
