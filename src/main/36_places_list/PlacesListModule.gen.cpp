
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::transportwebsite::SiteTableSync>("36.01 Site");

synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::transportwebsite::PlacesListModule>("36_places_list");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::transportwebsite::CityListInterfaceElement>("places_list");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::transportwebsite::CityListRequestInterfaceElement>("city_list_request");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::transportwebsite::PlacesListInterfacePage>("places_list");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::transportwebsite::PlacesListItemInterfacePage>("places_list_item");

synthese::util::Factory<synthese::server::Function>::integrate<synthese::transportwebsite::CityListRequest>("lc");

