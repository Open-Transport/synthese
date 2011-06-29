
// Transport website module

// Factories

#include "TransportSiteAdmin.h"

#include "TransportWebsiteModule.h"

#include "CityListFunction.h"
#include "PlacesListFunction.h"
#include "ServiceInformationsFunction.hpp"
#include "UserBookmarksService.hpp"

#include "TransportWebsiteTableSync.h"
#include "ObjectSiteLinkTableSync.h"
#include "RollingStockFilterTableSync.h"
#include "UserFavoriteJourneyTableSync.h"

#include "SiteUpdateAction.h"
#include "AddUserFavoriteJourneyAction.h"
#include "SiteCityAddAction.hpp"

#include "TransportWebsiteRight.h"

// Registries
#include "ObjectSiteLink.h"
#include "TransportWebsite.h"
#include "SiteCommercialLineLink.h"
#include "RollingStockFilter.h"
#include "UserFavoriteJourney.h"


#include "TransportWebsiteModule.inc.cpp"

void synthese::pt_website::moduleRegister()
{
	
	synthese::pt_website::TransportSiteAdmin::integrate();
	
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt_website::TransportWebsiteTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::cms::Website>, synthese::pt_website::TransportWebsiteTableSync>::integrate();
	
	synthese::pt_website::ObjectSiteLinkTableSync::integrate();
	synthese::pt_website::RollingStockFilterTableSync::integrate();
	synthese::pt_website::UserFavoriteJourneyTableSync::integrate();
	
	synthese::pt_website::TransportWebsiteModule::integrate();
	
	synthese::pt_website::CityListFunction::integrate();
	synthese::pt_website::PlacesListFunction::integrate();
	synthese::pt_website::ServiceInformationsFunction::integrate();
	synthese::pt_website::UserBookmarksService::integrate();
	
	synthese::pt_website::SiteUpdateAction::integrate();
	synthese::pt_website::AddUserFavoriteJourneyAction::integrate();
	synthese::pt_website::SiteCityAddAction::integrate();
	
	synthese::pt_website::TransportWebsiteRight::integrate();
	
	// Registries
	synthese::util::Env::Integrate<synthese::pt_website::ObjectSiteLink>();
	synthese::util::Env::Integrate<synthese::pt_website::TransportWebsite>();
	synthese::util::Env::Integrate<synthese::pt_website::SiteCommercialLineLink>();
	synthese::util::Env::Integrate<synthese::pt_website::RollingStockFilter>();
	synthese::util::Env::Integrate<synthese::pt_website::UserFavoriteJourney>();
	
}
