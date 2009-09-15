
synthese::departurestable::DeparturesTableModule::integrate();

synthese::departurestable::DeparturesTableInterfaceElement::integrate();
synthese::departurestable::DeparturesTableRoutePlanningInterfaceElement::integrate();
synthese::departurestable::DeparturesTableLineContentInterfaceElement::integrate();
synthese::departurestable::DeparturesTableDestinationContentInterfaceElement::integrate();
synthese::departurestable::DisplayScreenHasAlarmValueInterfaceElement::integrate();
synthese::departurestable::DisplayScreenAlarmContentValueInterfaceElement::integrate();

synthese::departurestable::DeparturesTableInterfacePage::integrate();
synthese::departurestable::DeparturesTableRoutePlanningInterfacePage::integrate();
synthese::departurestable::DepartureTableRowInterfacePage::integrate();
synthese::departurestable::DeparturesTableRoutePlanningRowInterfacePage::integrate();
synthese::departurestable::ParseDisplayReturnInterfacePage::integrate();
synthese::departurestable::DeparturesTableTransferDestinationInterfacePage::integrate();
synthese::departurestable::DeparturesTableDestinationInterfacepage::integrate();

synthese::departurestable::DisplaySearchAdmin::integrate();
synthese::departurestable::DisplayTypesAdmin::integrate();
synthese::departurestable::BroadcastPointsAdmin::integrate();
synthese::departurestable::DisplayAdmin::integrate();
synthese::departurestable::DisplayTypeAdmin::integrate();
synthese::departurestable::DisplayScreenCPUAdmin::integrate();
synthese::departurestable::DeparturesTableBenchmarkAdmin::integrate();

synthese::departurestable::DisplayTypeTableSync::integrate();
synthese::departurestable::DisplayScreenTableSync::integrate();
synthese::departurestable::DisplayMonitoringStatusTableSync::integrate();
synthese::departurestable::DisplayScreenCPUTableSync::integrate();

synthese::departurestable::CreateDisplayTypeAction::integrate();
synthese::departurestable::UpdateDisplayTypeAction::integrate();
synthese::departurestable::CreateDisplayScreenAction::integrate();
synthese::departurestable::UpdateDisplayScreenAction::integrate();
synthese::departurestable::AddPreselectionPlaceToDisplayScreenAction::integrate();
synthese::departurestable::RemovePreselectionPlaceFromDisplayScreenAction::integrate();
synthese::departurestable::UpdateAllStopsDisplayScreenAction::integrate();
synthese::departurestable::AddDepartureStopToDisplayScreenAction::integrate();
synthese::departurestable::AddForbiddenPlaceToDisplayScreenAction::integrate();
synthese::departurestable::UpdateDisplayPreselectionParametersAction::integrate();
synthese::departurestable::DisplayScreenAddDisplayedPlaceAction::integrate();
synthese::departurestable::DisplayScreenRemovePhysicalStopAction::integrate();
synthese::departurestable::DisplayScreenRemoveDisplayedPlaceAction::integrate();
synthese::departurestable::DisplayScreenRemoveForbiddenPlaceAction::integrate();
synthese::departurestable::UpdateDisplayMaintenanceAction::integrate();
synthese::departurestable::DisplayTypeRemoveAction::integrate();
synthese::departurestable::DisplayScreenRemoveAction::integrate();
synthese::departurestable::DisplayScreenAppearanceUpdateAction::integrate();
synthese::departurestable::DisplayScreenCPUCreateAction::integrate();
synthese::departurestable::DisplayScreenCPUUpdateAction::integrate();
synthese::departurestable::DisplayScreenCPUMaintenanceUpdateAction::integrate();
synthese::departurestable::DisplayScreenTransferDestinationAddAction::integrate();
synthese::departurestable::DisplayScreenTransferDestinationRemoveAction::integrate();

synthese::departurestable::DisplayScreenContentFunction::integrate();
synthese::departurestable::DisplayScreenSupervisionFunction::integrate();
synthese::departurestable::AlarmTestOnDisplayScreenFunction::integrate();
synthese::departurestable::DisplayScreenPhysicalStopFunction::integrate();

synthese::departurestable::ArrivalDepartureTableRight::integrate();
synthese::departurestable::DisplayMaintenanceRight::integrate();

synthese::departurestable::ArrivalDepartureTableLog::integrate();
synthese::departurestable::DisplayMaintenanceLog::integrate();

synthese::departurestable::DisplayScreenAlarmRecipient::integrate();

synthese::departurestable::DisplayGetNagiosStatusFunction::integrate();

synthese::departurestable::CentreonConfigExportFunction::integrate();
synthese::departurestable::CPUGetWiredScreensFunction::integrate();

// Registries
synthese::util::Env::Integrate<synthese::departurestable::DisplayScreen>();
synthese::util::Env::Integrate<synthese::departurestable::DisplayType>();
synthese::util::Env::Integrate<synthese::departurestable::DisplayMonitoringStatus>();
synthese::util::Env::Integrate<synthese::departurestable::DisplayScreenCPU>();
