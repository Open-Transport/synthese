
// 36 CMS
synthese::cms::WebPageAdmin::integrate();

synthese::cms::WebPageTableSync::integrate();

synthese::cms::CMSModule::integrate();

synthese::cms::AddService::integrate();
synthese::cms::BitAndFunction::integrate();
synthese::cms::DateService::integrate();
synthese::cms::EvalService::integrate();
synthese::cms::LanguageSelectorService::integrate();
synthese::cms::WebPageDisplayFunction::integrate();
synthese::cms::WebPageLastNewsFunction::integrate();
synthese::cms::IfFunction::integrate();
synthese::cms::EqualFunction::integrate();
synthese::cms::SuperiorFunction::integrate();
synthese::cms::AndFunction::integrate();
synthese::cms::OrService::integrate();
synthese::cms::WebPageLinksFunction::integrate();
synthese::cms::WebPagePositionFunction::integrate();
synthese::cms::StrLenFunction::integrate();
synthese::cms::SubStrFunction::integrate();
synthese::cms::WebPageMenuFunction::integrate();
synthese::cms::WebPageLinkFunction::integrate();
synthese::cms::WebPageFormFunction::integrate();
synthese::cms::WebpageNextFunction::integrate();
synthese::cms::WebpagePreviousFunction::integrate();
synthese::cms::SubstractFunction::integrate();
synthese::cms::ChrFunction::integrate();

synthese::cms::WebPageAddAction::integrate();
synthese::cms::WebPageUpdateAction::integrate();
synthese::cms::WebPageLinkAddAction::integrate();
synthese::cms::WebPageLinkRemoveAction::integrate();
synthese::cms::WebPageMoveAction::integrate();


// Registries
synthese::util::Env::Integrate<synthese::cms::Webpage>();
