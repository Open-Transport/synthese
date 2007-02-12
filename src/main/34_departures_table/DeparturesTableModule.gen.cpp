
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableInterfaceElement>("departurestable");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::departurestable::DepartureTableRowInterfacePage>("departurestablerow");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::DisplaySearchAdmin>("displays");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::DisplayTypesAdmin>("displaytypes");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::BroadcastPointsAdmin>("broadcastpoints");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::BroadcastPointAdmin>("broadcastpoint");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::DisplayAdmin>("display");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::departurestable::DisplayMaintenanceAdmin>("dmaint");


synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::departurestable::DisplayTypeTableSync>("34.00 Display Types");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::departurestable::BroadcastPointTableSync>("34.10 Broadcast points");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::departurestable::DisplayScreenTableSync>("34.50 Display Screens");


synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::CreateDisplayTypeAction>("createdisplaytype");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::UpdateDisplayTypeAction>("updatedisplaytype");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::CreateDisplayScreenAction>("createdisplayscreen");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::UpdateDisplayScreenAction>("updatedisplayscreen");

synthese::util::Factory<synthese::server::Request>::integrate<synthese::departurestable::DisplayScreenContentRequest>("tdg");

synthese::util::Factory<synthese::security::Right>::integrate<synthese::departurestable::ArrivalDepartureTableRight>("ArrivalDepartureTable");
synthese::util::Factory<synthese::security::Right>::integrate<synthese::departurestable::DisplayMaintenanceRight>("DisplayMaintenance");
synthese::util::Factory<synthese::security::Right>::integrate<synthese::departurestable::BroadcastPointsRight>("BroadcastPoints");

synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::departurestable::ArrivalDepartureTableLog>("departurestable");
synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::departurestable::BroadcastPointsDataLog>("broadcastpoints");
synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::departurestable::DisplayMaintenanceLog>("displaymaintenance");

synthese::util::Factory<synthese::messages::AlarmRecipient>::integrate<synthese::departurestable::DisplayScreenAlarmRecipient>("displayscreen");

