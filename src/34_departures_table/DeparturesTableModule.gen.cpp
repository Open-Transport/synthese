
synthese::departurestable::DeparturesTableModule::integrate();

synthese::departurestable::DeparturesTableInterfaceElement::integrate();
synthese::departurestable::DeparturesTableTimeContentInterfaceElement::integrate();
synthese::departurestable::DeparturesTableLineContentInterfaceElement::integrate();
synthese::departurestable::DeparturesTableDestinationContentInterfaceElement::integrate();
synthese::departurestable::DeparturesTableRowServiceNumberInterfaceElement::integrate();
synthese::departurestable::DisplayScreenHasAlarmValueInterfaceElement::integrate();
synthese::departurestable::DisplayScreenAlarmContentValueInterfaceElement::integrate();
synthese::departurestable::DeparturesTableTeamCell::integrate();
synthese::departurestable::DeparturesTableTrackCell::integrate();

synthese::departurestable::DeparturesTableInterfacePage::integrate();
synthese::departurestable::DepartureTableRowInterfacePage::integrate();
synthese::departurestable::ParseDisplayReturnInterfacePage::integrate();

synthese::departurestable::DisplaySearchAdmin::integrate();
synthese::departurestable::DisplayTypesAdmin::integrate();
synthese::departurestable::BroadcastPointsAdmin::integrate();
synthese::departurestable::DisplayAdmin::integrate();
synthese::departurestable::DisplayMaintenanceAdmin::integrate();
synthese::departurestable::DisplayTypeAdmin::integrate();

synthese::departurestable::DisplayTypeTableSync::integrate();
synthese::departurestable::DisplayScreenTableSync::integrate();
synthese::departurestable::DisplayMonitoringStatusTableSync::integrate();

synthese::departurestable::CreateDisplayTypeAction::integrate();
synthese::departurestable::UpdateDisplayTypeAction::integrate();
synthese::departurestable::CreateDisplayScreenAction::integrate();
synthese::departurestable::UpdateDisplayScreenAction::integrate();
synthese::departurestable::AddPreselectionPlaceToDisplayScreen::integrate();
synthese::departurestable::RemovePreselectionPlaceFromDisplayScreenAction::integrate();
synthese::departurestable::UpdateAllStopsDisplayScreenAction::integrate();
synthese::departurestable::AddDepartureStopToDisplayScreenAction::integrate();
synthese::departurestable::AddForbiddenPlaceToDisplayScreen::integrate();
synthese::departurestable::UpdateDisplayPreselectionParametersAction::integrate();
synthese::departurestable::DisplayScreenAddDisplayedPlace::integrate();
synthese::departurestable::DisplayScreenRemovePhysicalStopAction::integrate();
synthese::departurestable::DisplayScreenRemoveDisplayedPlaceAction::integrate();
synthese::departurestable::DisplayScreenRemoveForbiddenPlaceAction::integrate();
synthese::departurestable::UpdateDisplayMaintenanceAction::integrate();
synthese::departurestable::DisplayTypeRemoveAction::integrate();
synthese::departurestable::DisplayScreenRemove::integrate();

synthese::departurestable::DisplayScreenContentRequest::integrate();
synthese::departurestable::DisplayScreenSupervisionRequest::integrate();
synthese::departurestable::AlarmTestOnDisplayScreenFunction::integrate();
synthese::departurestable::DisplayScreenPhysicalStopFunction::integrate();

synthese::departurestable::ArrivalDepartureTableRight::integrate();
synthese::departurestable::DisplayMaintenanceRight::integrate();

synthese::departurestable::ArrivalDepartureTableLog::integrate();
synthese::departurestable::DisplayMaintenanceLog::integrate();

synthese::departurestable::DisplayScreenAlarmRecipient::integrate();

// Registries
synthese::util::Env::Integrate<synthese::departurestable::DisplayScreen>();
synthese::util::Env::Integrate<synthese::departurestable::DisplayType>();
synthese::util::Env::Integrate<synthese::departurestable::DisplayMonitoringStatus>();
