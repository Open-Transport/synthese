
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::vinci::VinciBikeRentalModule>("71_vinci_bike_rental");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::vinci::VinciCustomerSearchAdminInterfaceElement>("vincicustomers");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::vinci::VinciBikeSearchAdminInterfaceElement>("vincibikes");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciBikeTableSync>("71.01 Vinci Bike");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciRateTableSync>("71.02 Vinci Rate");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciSiteTableSync>("71.03 Vinci Site");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciContractTableSync>("71.11 Vinci Contract");