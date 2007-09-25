
synthese::departurestable::DeparturesTableInterfaceElement::integrate();
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableTimeContentInterfaceElement>("departurestabletimecell");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableLineContentInterfaceElement>("departurestablelinecell");
synthese::departurestable::DeparturesTableDestinationContentInterfaceElement::integrate();
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableRowServiceNumberInterfaceElement>("departurestableservicenumbercell");
synthese::departurestable::DisplayScreenHasAlarmValueInterfaceElement::integrate();
synthese::departurestable::DisplayScreenAlarmContentValueInterfaceElement::integrate();

synthese::departurestable::DeparturesTableInterfacePage::integrate();
synthese::departurestable::DepartureTableRowInterfacePage::integrate();
synthese::departurestable::ParseDisplayReturnInterfacePage::integrate();

synthese::departurestable::DisplaySearchAdmin::integrate();
synthese::departurestable::DisplayTypesAdmin::integrate();
synthese::departurestable::BroadcastPointsAdmin::integrate();
synthese::departurestable::DisplayAdmin::integrate();
synthese::departurestable::DisplayMaintenanceAdmin::integrate();


synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::departurestable::DisplayTypeTableSync>("34.00 Display Types");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::departurestable::DisplayScreenTableSync>("34.50 Display Screens");


synthese::departurestable::CreateDisplayTypeAction::integrate();
synthese::departurestable::UpdateDisplayTypeAction::integrate();
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::CreateDisplayScreenAction>("createdisplayscreen");
synthese::departurestable::UpdateDisplayScreenAction::integrate();
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
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::DisplayTypeRemoveAction>("dtra");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::departurestable::DisplayScreenRemove>("dsra");



synthese::util::Factory<synthese::server::Function>::integrate<synthese::departurestable::DisplayScreenContentRequest>("tdg");
synthese::util::Factory<synthese::server::Function>::integrate<synthese::departurestable::DisplayScreenSupervisionRequest>("tds");

synthese::departurestable::ArrivalDepartureTableRight::integrate();
synthese::departurestable::DisplayMaintenanceRight::integrate();

synthese::departurestable::ArrivalDepartureTableLog::integrate();
synthese::departurestable::DisplayMaintenanceLog::integrate();
synthese::departurestable::DisplayDataControlLog::integrate();

synthese::departurestable::DisplayScreenAlarmRecipient::integrate();

