
Factory<InterfacePage>::integrate<LineMarkerInterfacePage>("line_marker");

Factory<ValueInterfaceElement>::integrate<LogicalStopNameValueInterfaceElement>("stop_name");
Factory<ValueInterfaceElement>::integrate<CityNameValueInterfaceElement>("city_name");


Factory<LibraryInterfaceElement>::integrate<LogicalStopNameValueInterfaceElement>("stop_name");
Factory<LibraryInterfaceElement>::integrate<CityNameValueInterfaceElement>("city_name");
Factory<LibraryInterfaceElement>::integrate<CityListInterfaceElement>("city_list");
