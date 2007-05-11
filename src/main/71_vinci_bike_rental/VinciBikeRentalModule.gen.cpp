
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::vinci::VinciBikeRentalModule>("71_vinci_bike_rental");

synthese::vinci::VinciCustomerAdminInterfaceElement::integrate();
synthese::vinci::VinciCustomerSearchAdminInterfaceElement::integrate();
synthese::vinci::VinciBikeSearchAdminInterfaceElement::integrate();
synthese::vinci::VinciReportsAdminInterfaceElement::integrate();
synthese::vinci::VinciBikeAdminInterfaceElement::integrate();
synthese::vinci::VinciSitesAdminInterfaceElement::integrate();

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciBikeTableSync>("71.01 Vinci Bike");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciRateTableSync>("71.02 Vinci Rate");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciSiteTableSync>("71.03 Vinci Site");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciAntivolTableSync>("71.04 Vinci Locks");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::vinci::VinciContractTableSync>("71.11 Vinci Contract");

synthese::util::Factory<synthese::server::Action>::integrate<synthese::vinci::AddCustomerAction>("vinciaddcustomer");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::vinci::VinciUpdateCustomerAction>("vinciupdatecustomer");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::vinci::VinciAddGuaranteeAction>("vinciaddguarantee");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::vinci::VinciReturnGuaranteeAction>("vinciretguarantee");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::vinci::VinciAddBike>("vinciaddbike");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::vinci::RentABikeAction>("rentabike");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::vinci::ReturnABikeAction>("vincireturnbike");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::vinci::VinciUpdateBikeAction>("vinciupdatebike");

synthese::util::Factory<synthese::server::Function>::integrate<synthese::vinci::VinciContractPrintRequest>("vinciprintcontract");
synthese::util::Factory<synthese::server::Function>::integrate<synthese::vinci::BarCodeInterpretFunction>("barcoderead");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::vinci::VinciPrintedContractInterfacePage>("vincicontractprint");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::vinci::BarCodeHTMLFormValueInterfaceElement>("barcodefield");
