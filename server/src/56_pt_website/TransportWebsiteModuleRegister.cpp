
// Transport website module

// Factories

#include "TransportSiteAdmin.h"

#include "TransportWebsiteModule.h"

#include "CityListFunction.h"
#include "PlacesListFunction.h"
#include "PlacesListService.hpp"
#include "ServiceDetailService.hpp"
#include "ServiceInformationsFunction.hpp"
#include "ServicesListService.hpp"
#include "UserBookmarksService.hpp"

#include "PTServiceConfigTableSync.hpp"
#include "ObjectSiteLinkTableSync.h"
#include "RollingStockFilterTableSync.h"
#include "UserFavoriteJourneyTableSync.h"

#include "SiteUpdateAction.h"
#include "AddUserFavoriteJourneyAction.h"
#include "SiteCityAddAction.hpp"

#include "TransportWebsiteRight.h"

// Registries
#include "ObjectSiteLink.h"
#include "PTServiceConfig.hpp"
#include "SiteCommercialLineLink.h"
#include "RollingStockFilter.h"
#include "UserFavoriteJourney.h"


#include "TransportWebsiteModule.inc.cpp"

void synthese::pt_website::moduleRegister()
{
	synthese::pt_website::TransportSiteAdmin::integrate();

	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt_website::PTServiceConfigTableSync>::integrate();

	synthese::pt_website::ObjectSiteLinkTableSync::integrate();
	synthese::pt_website::RollingStockFilterTableSync::integrate();
	synthese::pt_website::UserFavoriteJourneyTableSync::integrate();

	synthese::pt_website::TransportWebsiteModule::integrate();

	synthese::pt_website::CityListFunction::integrate();
	synthese::pt_website::PlacesListFunction::integrate();
	synthese::pt_website::PlacesListService::integrate();
	synthese::pt_website::ServiceDetailService::integrate();
	synthese::pt_website::ServiceInformationsFunction::integrate();
	synthese::pt_website::ServicesListService::integrate();
	synthese::pt_website::UserBookmarksService::integrate();

	synthese::pt_website::SiteUpdateAction::integrate();
	synthese::pt_website::AddUserFavoriteJourneyAction::integrate();
	synthese::pt_website::SiteCityAddAction::integrate();

	synthese::pt_website::TransportWebsiteRight::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::pt_website::ObjectSiteLink>();
	INTEGRATE(synthese::pt_website::PTServiceConfig);
	synthese::util::Env::Integrate<synthese::pt_website::SiteCommercialLineLink>();
	synthese::util::Env::Integrate<synthese::pt_website::RollingStockFilter>();
	synthese::util::Env::Integrate<synthese::pt_website::UserFavoriteJourney>();

}
