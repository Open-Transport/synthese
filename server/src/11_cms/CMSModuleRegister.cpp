
// 36 CMS

#include "CMSModule.hpp"

#include "AddService.hpp"
#include "AndFunction.hpp"
#include "BitAndFunction.hpp"
#include "ChrFunction.hpp"
#include "CMSInstallRight.hpp"
#include "CMSRight.hpp"
#include "DateService.hpp"
#include "EqualFunction.hpp"
#include "EvalService.hpp"
#include "IfFunction.hpp"
#include "LanguageSelectorService.hpp"
#include "MailService.hpp"
#include "NoOpService.hpp"
#include "NotService.hpp"
#include "OrService.hpp"
#include "PackagesService.hpp"
#include "PrecisionService.hpp"
#include "RowsListFunction.hpp"
#include "ServiceAPIListService.hpp"
#include "StrFillFunction.hpp"
#include "StripTagsFunction.hpp"
#include "StrLenFunction.hpp"
#include "StrReplaceFunction.hpp"
#include "StrStrFunction.hpp"
#include "SubstractFunction.hpp"
#include "SubStrFunction.hpp"
#include "SuperiorFunction.hpp"
#include "TimeDurationService.hpp"
#include "WebPageAddAction.h"
#include "WebPageAdmin.h"
#include "WebpageContentUploadAction.hpp"
#include "WebPageDisplayFunction.h"
#include "WebPageFormFunction.hpp"
#include "WebPageLastNewsFunction.hpp"
#include "WebPageLinkAddAction.hpp"
#include "WebPageLinkFunction.hpp"
#include "WebPageLinkRemoveAction.hpp"
#include "WebPageLinksFunction.hpp"
#include "StrStrFunction.hpp"
#include "WebPageMenuFunction.hpp"
#include "WebPageMoveAction.hpp"
#include "WebpageNextFunction.hpp"
#include "WebPagePositionFunction.hpp"
#include "WebpagePreviousFunction.hpp"
#include "WebPageTableSync.h"
#include "WebPageUpdateAction.h"
#include "WebsiteAdmin.hpp"
#include "WebsiteConfigTableSync.hpp"
#include "WebsiteRankCleanupAction.hpp"
#include "WebsitesService.hpp"
#include "WebsiteTableSync.hpp"

// Registries

#include "Webpage.h"


#include "CMSModule.inc.cpp"

void synthese::cms::moduleRegister()
{
	// Registries
	INTEGRATE(synthese::cms::Website);
	INTEGRATE(synthese::cms::Webpage);
	INTEGRATE(synthese::cms::WebsiteConfig);

	// 36 CMS
	synthese::cms::AddService::integrate();
	synthese::cms::AndFunction::integrate();
	synthese::cms::BitAndFunction::integrate();
	synthese::cms::ChrFunction::integrate();
	synthese::cms::CMSInstallRight::integrate();
	synthese::cms::CMSModule::integrate();
	synthese::cms::CMSRight::integrate();
	synthese::cms::DateService::integrate();
	synthese::cms::EqualFunction::integrate();
	synthese::cms::EvalService::integrate();
	synthese::cms::IfFunction::integrate();
	synthese::cms::LanguageSelectorService::integrate();
	synthese::cms::MailService::integrate();
	synthese::cms::NoOpService::integrate();
	synthese::cms::NotService::integrate();
	synthese::cms::OrService::integrate();
	synthese::cms::PackagesService::integrate();
	synthese::cms::PrecisionService::integrate();
	synthese::cms::RowsListFunction::integrate();
	synthese::cms::ServiceAPIListService::integrate();
	synthese::cms::StrFillFunction::integrate();
	synthese::cms::StripTagsFunction::integrate();
	synthese::cms::StrLenFunction::integrate();
	synthese::cms::StrReplaceFunction::integrate();
	synthese::cms::StrStrFunction::integrate();
	synthese::cms::SubstractFunction::integrate();
	synthese::cms::SubStrFunction::integrate();
	synthese::cms::SuperiorFunction::integrate();
	synthese::cms::TimeDurationService::integrate();
	synthese::cms::WebPageAddAction::integrate();
	synthese::cms::WebPageAdmin::integrate();
	synthese::cms::WebpageContentUploadAction::integrate();
	synthese::cms::WebPageDisplayFunction::integrate();
	synthese::cms::WebPageFormFunction::integrate();
	synthese::cms::WebPageLastNewsFunction::integrate();
	synthese::cms::WebPageLinkAddAction::integrate();
	synthese::cms::WebPageLinkFunction::integrate();
	synthese::cms::WebPageLinkRemoveAction::integrate();
	synthese::cms::WebPageLinksFunction::integrate();
	synthese::cms::WebPageMenuFunction::integrate();
	synthese::cms::WebPageMoveAction::integrate();
	synthese::cms::WebpageNextFunction::integrate();
	synthese::cms::WebPagePositionFunction::integrate();
	synthese::cms::WebpagePreviousFunction::integrate();
	synthese::cms::WebPageTableSync::integrate();
	synthese::cms::WebPageUpdateAction::integrate();
	synthese::cms::WebsiteAdmin::integrate();
	synthese::cms::WebsiteConfigTableSync::integrate();
	synthese::cms::WebsiteRankCleanupAction::integrate();
	synthese::cms::WebsitesService::integrate();
	synthese::cms::WebsiteTableSync::integrate();
}
