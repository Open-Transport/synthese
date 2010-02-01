
synthese::transportwebsite::TransportSiteAdmin::integrate();
synthese::transportwebsite::SiteRoutePlanningAdmin::integrate();

synthese::transportwebsite::SiteTableSync::integrate();
synthese::transportwebsite::ObjectSiteLinkTableSync::integrate();
synthese::transportwebsite::RollingStockFilterTableSync::integrate();
synthese::transportwebsite::UserFavoriteJourneyTableSync::integrate();

synthese::transportwebsite::PlacesListModule::integrate();

synthese::transportwebsite::CityListInterfaceElement::integrate();
synthese::transportwebsite::CityListRequestInterfaceElement::integrate();
synthese::transportwebsite::PlacesListRequestInterfaceElement::integrate();
synthese::transportwebsite::UserFavoriteListInterfaceElement::integrate();

synthese::transportwebsite::PlacesListInterfacePage::integrate();
synthese::transportwebsite::PlacesListItemInterfacePage::integrate();
synthese::transportwebsite::UserFavoriteInterfacePage::integrate();

synthese::transportwebsite::CityListRequest::integrate();
synthese::transportwebsite::PlacesListFunction::integrate();

synthese::transportwebsite::SiteUpdateAction::integrate();

synthese::transportwebsite::TransportWebsiteRight::integrate();

// Registries
synthese::util::Env::Integrate<synthese::transportwebsite::ObjectSiteLink>();
synthese::util::Env::Integrate<synthese::transportwebsite::Site>();
synthese::util::Env::Integrate<synthese::transportwebsite::SiteCommercialLineLink>();
synthese::util::Env::Integrate<synthese::transportwebsite::RollingStockFilter>();
synthese::util::Env::Integrate<synthese::transportwebsite::UserFavoriteJourney>();