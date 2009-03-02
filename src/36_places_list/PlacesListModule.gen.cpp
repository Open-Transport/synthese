
synthese::transportwebsite::TransportSiteAdmin::integrate();
synthese::transportwebsite::SiteRoutePlanningAdmin::integrate();

synthese::transportwebsite::SiteTableSync::integrate();
synthese::transportwebsite::ObjectSiteLinkTableSync::integrate();

synthese::transportwebsite::PlacesListModule::integrate();

synthese::transportwebsite::CityListInterfaceElement::integrate();
synthese::transportwebsite::CityListRequestInterfaceElement::integrate();
synthese::transportwebsite::PlacesListRequestInterfaceElement::integrate();

synthese::transportwebsite::PlacesListInterfacePage::integrate();
synthese::transportwebsite::PlacesListItemInterfacePage::integrate();

synthese::transportwebsite::CityListRequest::integrate();
synthese::transportwebsite::PlacesListFunction::integrate();

synthese::transportwebsite::SiteUpdateAction::integrate();

synthese::transportwebsite::TransportWebsiteRight::integrate();

// Registries
synthese::util::Env::Integrate<synthese::transportwebsite::ObjectSiteLink>();
synthese::util::Env::Integrate<synthese::transportwebsite::Site>();
synthese::util::Env::Integrate<synthese::transportwebsite::SiteCommercialLineLink>();