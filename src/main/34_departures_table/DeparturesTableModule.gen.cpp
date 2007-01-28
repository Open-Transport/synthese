
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableInterfaceElement>("departurestable");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::departurestable::DepartureTableRowInterfacePage>("departurestablerow");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::DisplaySearchAdmin>("displays");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::DisplayTypesAdmin>("displaytypes");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::BroadcastPointsAdmin>("broadcastpoints");

