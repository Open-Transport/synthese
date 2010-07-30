
synthese::transportwebsite::TransportSiteAdmin::integrate();

synthese::util::FactorableTemplate<synthese::db::SQLiteTableSync,synthese::transportwebsite::TransportWebsiteTableSync>::integrate();
synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::cms::Website>, synthese::transportwebsite::TransportWebsiteTableSync>::integrate();

synthese::transportwebsite::ObjectSiteLinkTableSync::integrate();
synthese::transportwebsite::RollingStockFilterTableSync::integrate();
synthese::transportwebsite::UserFavoriteJourneyTableSync::integrate();

synthese::transportwebsite::TransportWebsiteModule::integrate();

synthese::transportwebsite::PlacesListRequestFunction::integrate();
synthese::transportwebsite::UserFavoriteListInterfaceElement::integrate();

synthese::transportwebsite::UserFavoriteInterfacePage::integrate();

synthese::transportwebsite::CityListRequestFunction::integrate();
synthese::transportwebsite::CityListFunction::integrate();
synthese::transportwebsite::PlacesListFunction::integrate();

synthese::transportwebsite::SiteUpdateAction::integrate();
synthese::transportwebsite::AddUserFavoriteJourneyAction::integrate();

synthese::transportwebsite::TransportWebsiteRight::integrate();

// Registries
synthese::util::Env::Integrate<synthese::transportwebsite::ObjectSiteLink>();
synthese::util::Env::Integrate<synthese::transportwebsite::TransportWebsite>();
synthese::util::Env::Integrate<synthese::transportwebsite::SiteCommercialLineLink>();
synthese::util::Env::Integrate<synthese::transportwebsite::RollingStockFilter>();
synthese::util::Env::Integrate<synthese::transportwebsite::UserFavoriteJourney>();

