
// 14 Geography

// Factories
#include "CityNameValueInterfaceElement.h"
#include "GeographyModule.h"

#include "CityTableSync.h"
#include "CityAliasTableSync.hpp"
#include "MapSourceTableSync.hpp"
#include "PlaceAliasTableSync.h"

#include "CityAddAction.h"
#include "CityUpdateAction.h"
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
			CityNameValueInterfaceElement::integrate();

			GeographyModule::integrate();
			
			CityTableSync::integrate();
			CityAliasTableSync::integrate();
			MapSourceTableSync::integrate();
			PlaceAliasTableSync::integrate();

			PlaceAlias::integrate();
			
			CityAddAction::integrate();
			CityUpdateAction::integrate();
			MapSourceUpdateAction::integrate();

			GetMapOpenLayersConstructorService::integrate();

			MapSourceAdmin::integrate();
			MapSourcesAdmin::integrate();
			
			// Registries
			Env::Integrate<City>();
			Env::Integrate<CityAlias>();
			Env::Integrate<MapSource>();
			Env::Integrate<PlaceAlias>();
		}
}	}
