
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableInterfaceElement>("departurestable");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableTimeContentInterfaceElement>("departurestabletimecell");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableLineContentInterfaceElement>("departurestablelinecell");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableDestinationContentInterfaceElement>("departurestabledestinationcell");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::departurestable::DisplayScreenHasAlarmValueInterfaceElement>("displayscreenalarmlevel");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::departurestable::DisplayScreenAlarmContentValueInterfaceElement>("displayscreenalarmcontent");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::departurestable::DeparturesTableInterfacePage>("departurestable");
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
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::CreateBroadcastPointAction>("crebp");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::RenameBroadcastPointAction>("renbp");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::DeleteBroadcastPointAction>("delbp");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::AddPreselectionPlaceToDisplayScreen>("apptds");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::RemovePreselectionPlaceFromDisplayScreenAction>("rmpsfds");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::UpdateAllStopsDisplayScreenAction>("uasdsa");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::AddDepartureStopToDisplayScreenAction>("adstdsa");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::AddForbiddenPlaceToDisplayScreen>("afptdsa");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::UpdateDisplayPreselectionParametersAction>("udpp");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::DisplayScreenAddDisplayedPlace>("dsadp");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::DisplayScreenRemovePhysicalStopAction>("dsrps");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::DisplayScreenRemoveDisplayedPlaceAction>("dsrdp");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::DisplayScreenRemoveForbiddenPlaceAction>("dsrfp");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::UpdateDisplayMaintenanceAction>("udm");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::DisplayScreenUpdateLocalizationAction>("dsula");


synthese::util::Factory<synthese::server::Function>::integrate<synthese::departurestable::DisplayScreenContentRequest>("tdg");
synthese::util::Factory<synthese::server::Function>::integrate<synthese::departurestable::DisplayScreenSupervisionRequest>("tds");

synthese::departurestable::ArrivalDepartureTableRight::integrate();
synthese::departurestable::DisplayMaintenanceRight::integrate();
synthese::departurestable::BroadcastPointsRight::integrate();

synthese::departurestable::ArrivalDepartureTableLog::integrate();
synthese::departurestable::BroadcastPointsDataLog::integrate();
synthese::departurestable::DisplayMaintenanceLog::integrate();
synthese::departurestable::DisplayDataControlLog::integrate();

synthese::departurestable::DisplayScreenAlarmRecipient::integrate();

