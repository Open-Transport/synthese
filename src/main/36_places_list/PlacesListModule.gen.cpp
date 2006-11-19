
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::placeslist::PlacesListModule>("36_places_list");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::placeslist::CityListInterfaceElement>("city_list");
synthese::util::Factory<synthese::server::Request>::integrate<synthese::placeslist::CityListRequest>("city_list");
