
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableInterfaceElement>("departurestable");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::departurestable::DepartureTableRowInterfacePage>("departurestablerow");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::DisplaySearchAdmin>("displays");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::DisplayTypesAdmin>("displaytypes");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::BroadcastPointsAdmin>("broadcastpoints");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::departurestable::DisplayTypeTableSync>("34.00 Display Types");

synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::CreateDisplayTypeAction>("createdisplaytype");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::UpdateDisplayTypeAction>("updatedisplaytype");

synthese::util::Factory<synthese::security::Right>::integrate<synthese::departurestable::ArrivalDepartureTableRight>("ArrivalDepartureTable");
synthese::util::Factory<synthese::security::Right>::integrate<synthese::departurestable::DisplayMaintenanceRight>("DisplayMaintenance");
synthese::util::Factory<synthese::security::Right>::integrate<synthese::departurestable::BroadcastPointsRight>("BroadcastPoints");

synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::departurestable::ArrivalDepartureTableLog>("departurestable");
synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::departurestable::BroadcastPointsDataLog>("broadcastpoints");
synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::departurestable::DisplayMaintenanceLog>("displaymaintenance");
