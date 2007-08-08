
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::transportwebsite::SiteTableSync>("36.01 Site");

synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::transportwebsite::PlacesListModule>("36_places_list");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::transportwebsite::CityListInterfaceElement>("city_list");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::transportwebsite::PlacesListInterfacePage>("places_list");

synthese::util::Factory<synthese::server::Function>::integrate<synthese::transportwebsite::CityListRequest>("lc");

