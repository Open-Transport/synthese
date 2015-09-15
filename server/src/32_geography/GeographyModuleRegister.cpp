
// 14 Geography

// Factories
#include "GeographyModule.h"

#include "CityTableSync.h"
#include "CityAliasTableSync.hpp"
#include "MapSourceTableSync.hpp"
#include "PlaceAliasTableSync.h"

#include "MapSourceUpdateAction.hpp"

#include "GetMapOpenLayersConstructorService.hpp"

#include "MapSourceAdmin.hpp"
#include "MapSourcesAdmin.hpp"

// Registries
#include "City.h"
#include "CityAlias.hpp"
#include "MapSource.hpp"
#include "PlaceAlias.h"

#include "GeographyModule.inc.cpp"

namespace synthese
{
	using namespace util;

	namespace geography
	{
		void moduleRegister()
		{
			// Factories
			GeographyModule::integrate();

			CityTableSync::integrate();
			CityAliasTableSync::integrate();
			MapSourceTableSync::integrate();
			PlaceAliasTableSync::integrate();

			PlaceAlias::integrate();

			MapSourceUpdateAction::integrate();

			GetMapOpenLayersConstructorService::integrate();

			MapSourceAdmin::integrate();
			MapSourcesAdmin::integrate();

			// Registries
			INTEGRATE(City);
			Env::Integrate<CityAlias>();
			Env::Integrate<MapSource>();
			Env::Integrate<PlaceAlias>();
		}
}	}
