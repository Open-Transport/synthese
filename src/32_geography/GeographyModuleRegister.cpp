
// 14 Geography

// Factories
#include "CityTableSync.h"
#include "CityNameValueInterfaceElement.h"
#include "GeographyModule.h"
#include "PlaceAliasTableSync.h"
#include "CityAliasTableSync.hpp"

#include "CityUpdateAction.h"
#include "CityAddAction.h"

// Registries
#include "City.h"
#include "CityAlias.hpp"
#include "PlaceAlias.h"



#include "GeographyModule.inc.cpp"

void synthese::geography::moduleRegister()
{
	
	
	// 14 Geography
	
	// Factories
	synthese::geography::CityTableSync::integrate();
	synthese::geography::CityNameValueInterfaceElement::integrate();
	synthese::geography::GeographyModule::integrate();
	synthese::geography::PlaceAliasTableSync::integrate();
	synthese::geography::PlaceAlias::integrate();
	synthese::geography::CityAliasTableSync::integrate();
	
	synthese::geography::CityUpdateAction::integrate();
	synthese::geography::CityAddAction::integrate();
	
	// Registries
	synthese::util::Env::Integrate<synthese::geography::City>();
	synthese::util::Env::Integrate<synthese::geography::CityAlias>();
	synthese::util::Env::Integrate<synthese::geography::PlaceAlias>();
	
}
