
synthese::transportwebsite::TransportSiteAdmin::integrate();
synthese::transportwebsite::WebPageAdmin::integrate();

synthese::transportwebsite::SiteTableSync::integrate();
synthese::transportwebsite::ObjectSiteLinkTableSync::integrate();
synthese::transportwebsite::RollingStockFilterTableSync::integrate();
synthese::transportwebsite::UserFavoriteJourneyTableSync::integrate();
synthese::transportwebsite::WebPageTableSync::integrate();

synthese::transportwebsite::PlacesListModule::integrate();

synthese::transportwebsite::CityListRequestInterfaceElement::integrate();
synthese::transportwebsite::PlacesListRequestInterfaceElement::integrate();
synthese::transportwebsite::UserFavoriteListInterfaceElement::integrate();
synthese::transportwebsite::WebPageFormInterfaceElement::integrate();
synthese::transportwebsite::WebPageLinkInterfaceElement::integrate();
synthese::transportwebsite::WebPageMenuInterfaceElement::integrate();

synthese::transportwebsite::UserFavoriteInterfacePage::integrate();

synthese::transportwebsite::CityListFunction::integrate();
synthese::transportwebsite::PlacesListFunction::integrate();
synthese::transportwebsite::WebPageDisplayFunction::integrate();

synthese::transportwebsite::SiteUpdateAction::integrate();
synthese::transportwebsite::AddUserFavoriteJourneyAction::integrate();
synthese::transportwebsite::WebPageAddAction::integrate();
synthese::transportwebsite::WebPageUpdateAction::integrate();
synthese::transportwebsite::WebPageRemoveAction::integrate();
synthese::transportwebsite::WebPageContentUpdateAction::integrate();

synthese::transportwebsite::TransportWebsiteRight::integrate();

// Registries
synthese::util::Env::Integrate<synthese::transportwebsite::ObjectSiteLink>();
synthese::util::Env::Integrate<synthese::transportwebsite::Site>();
synthese::util::Env::Integrate<synthese::transportwebsite::SiteCommercialLineLink>();
synthese::util::Env::Integrate<synthese::transportwebsite::RollingStockFilter>();
synthese::util::Env::Integrate<synthese::transportwebsite::UserFavoriteJourney>();
synthese::util::Env::Integrate<synthese::transportwebsite::WebPage>();