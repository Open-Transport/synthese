
// 36 CMS

#include "CMSModule.hpp"

#include "WebPageAddAction.h"
#include "WebpageContentUploadAction.hpp"
#include "WebPageUpdateAction.h"
#include "WebPageLinkAddAction.hpp"
#include "WebPageLinkRemoveAction.hpp"
#include "WebPageMoveAction.hpp"

#include "AddService.hpp"
#include "BitAndFunction.hpp"
#include "DateService.hpp"
#include "EvalService.hpp"
#include "LanguageSelectorService.hpp"
#include "NoOpService.hpp"
#include "NotService.hpp"
#include "PackagesService.hpp"
#include "PrecisionService.hpp"
#include "TimeDurationService.hpp"
#include "WebPageDisplayFunction.h"
#include "WebPageLastNewsFunction.hpp"
#include "IfFunction.hpp"
#include "OrService.hpp"
#include "EqualFunction.hpp"
#include "SuperiorFunction.hpp"
#include "AndFunction.hpp"
#include "WebPageLinksFunction.hpp"
#include "WebPagePositionFunction.hpp"
#include "WebsitesService.hpp"
#include "ServiceAPIListService.hpp"
#include "StrFillFunction.hpp"
#include "StrLenFunction.hpp"
#include "SubStrFunction.hpp"
#include "WebPageMenuFunction.hpp"
#include "WebPageLinkFunction.hpp"
#include "WebPageFormFunction.hpp"
#include "WebpageNextFunction.hpp"
#include "WebpagePreviousFunction.hpp"
#include "SubstractFunction.hpp"
#include "ChrFunction.hpp"
#include "RowsListFunction.hpp"
#include "MailService.hpp"

#include "WebPageAdmin.h"
#include "WebsiteAdmin.hpp"

#include "WebPageTableSync.h"
#include "WebsiteTableSync.hpp"
#include "WebsiteConfigTableSync.hpp"

#include "CMSInstallRight.hpp"
#include "CMSRight.hpp"

// Registries

#include "Webpage.h"


#include "CMSModule.inc.cpp"

void synthese::cms::moduleRegister()
{
	// Registries
	synthese::util::Env::Integrate<synthese::cms::Website>();
	synthese::util::Env::Integrate<synthese::cms::Webpage>();
	synthese::util::Env::Integrate<synthese::cms::WebsiteConfig>();

	// 36 CMS
	synthese::cms::WebPageAdmin::integrate();
	synthese::cms::WebsiteAdmin::integrate();

	synthese::cms::WebPageTableSync::integrate();
	synthese::cms::WebsiteTableSync::integrate();
	synthese::cms::WebsiteConfigTableSync::integrate();

	synthese::cms::CMSModule::integrate();

	synthese::cms::AddService::integrate();
	synthese::cms::BitAndFunction::integrate();
	synthese::cms::DateService::integrate();
	synthese::cms::EvalService::integrate();
	synthese::cms::LanguageSelectorService::integrate();
	synthese::cms::NotService::integrate();
	synthese::cms::PackagesService::integrate();
	synthese::cms::PrecisionService::integrate();
	synthese::cms::TimeDurationService::integrate();
	synthese::cms::WebPageDisplayFunction::integrate();
	synthese::cms::WebPageLastNewsFunction::integrate();
	synthese::cms::IfFunction::integrate();
	synthese::cms::EqualFunction::integrate();
	synthese::cms::SuperiorFunction::integrate();
	synthese::cms::AndFunction::integrate();
	synthese::cms::NoOpService::integrate();
	synthese::cms::OrService::integrate();
	synthese::cms::WebPageLinksFunction::integrate();
	synthese::cms::WebPagePositionFunction::integrate();
	synthese::cms::ServiceAPIListService::integrate();
	synthese::cms::StrFillFunction::integrate();
	synthese::cms::StrLenFunction::integrate();
	synthese::cms::SubStrFunction::integrate();
	synthese::cms::WebPageMenuFunction::integrate();
	synthese::cms::WebPageLinkFunction::integrate();
	synthese::cms::WebPageFormFunction::integrate();
	synthese::cms::WebpageNextFunction::integrate();
	synthese::cms::WebpagePreviousFunction::integrate();
	synthese::cms::SubstractFunction::integrate();
	synthese::cms::ChrFunction::integrate();
	synthese::cms::RowsListFunction::integrate();
	synthese::cms::MailService::integrate();
	synthese::cms::WebsitesService::integrate();

	synthese::cms::WebPageAddAction::integrate();
	synthese::cms::WebpageContentUploadAction::integrate();
	synthese::cms::WebPageUpdateAction::integrate();
	synthese::cms::WebPageLinkAddAction::integrate();
	synthese::cms::WebPageLinkRemoveAction::integrate();
	synthese::cms::WebPageMoveAction::integrate();

	synthese::cms::CMSInstallRight::integrate();
	synthese::cms::CMSRight::integrate();
}
