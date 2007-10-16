
synthese::db::DBModule::integrate();

synthese::db::DbModuleConfigTableSync::integrate();



synthese::dbring::DbRingModule::integrate();

synthese::dbring::NodeInfoTableSync::integrate();
synthese::dbring::UpdateRecordTableSync::integrate();


synthese::env::LineMarkerInterfacePage::integrate();
synthese::env::ReservationRuleInterfacePage::integrate();

synthese::env::LogicalStopNameValueInterfaceElement::integrate();
synthese::env::CityNameValueInterfaceElement::integrate();
synthese::env::LineStyleInterfaceElement::integrate();
synthese::env::LineShortNameInterfaceElement::integrate();
synthese::env::LineImageURLInterfaceElement::integrate();

synthese::env::FareTableSync::integrate();
synthese::env::BikeComplianceTableSync::integrate();
synthese::env::HandicappedComplianceTableSync::integrate();
synthese::env::PedestrianComplianceTableSync::integrate();
synthese::env::ReservationRuleTableSync::integrate();

synthese::env::CityTableSync::integrate();
synthese::env::TransportNetworkTableSync::integrate();
synthese::env::AxisTableSync::integrate();

synthese::env::CommercialLineTableSync::integrate();

synthese::env::LineTableSync::integrate();
synthese::env::RoadTableSync::integrate();

synthese::env::ConnectionPlaceTableSync::integrate();
synthese::env::CrossingTableSync::integrate();
synthese::env::PublicPlaceTableSync::integrate();

synthese::env::PlaceAliasTableSync::integrate();
synthese::env::AddressTableSync::integrate();

synthese::env::PhysicalStopTableSync::integrate();

synthese::env::LineStopTableSync::integrate();

synthese::env::RoadChunkTableSync::integrate();
synthese::env::ContinuousServiceTableSync::integrate();
synthese::env::ScheduledServiceTableSync::integrate();

synthese::env::ServiceDateTableSync::integrate();



synthese::interfaces::InterfaceModule::integrate();

synthese::interfaces::ParameterValueInterfaceElement::integrate();
synthese::interfaces::IfThenElseInterfaceElement::integrate();
synthese::interfaces::FormattedNumberInterfaceElement::integrate();
synthese::interfaces::EqualsValueInterfaceElement::integrate();
synthese::interfaces::ChrInterfaceElement::integrate();
synthese::interfaces::SuperiorValueInterfaceElement::integrate();
synthese::interfaces::SubstrValueInterfaceElement::integrate();
synthese::interfaces::AndValueInterfaceElement::integrate();
synthese::interfaces::PlainCharFilterValueInterfaceElement::integrate();

synthese::interfaces::CommentInterfaceElement::integrate();
synthese::interfaces::PrintInterfaceElement::integrate();
synthese::interfaces::DebugInterfaceElement::integrate();
synthese::interfaces::LineLabelInterfaceElement::integrate();
synthese::interfaces::IncludePageInterfaceElement::integrate();
synthese::interfaces::GotoInterfaceElement::integrate();
synthese::interfaces::SetInterfaceElement::integrate();

synthese::interfaces::InterfaceTableSync::integrate();
synthese::interfaces::InterfacePageTableSync::integrate();

synthese::interfaces::RedirectInterfacePage::integrate();
synthese::interfaces::DurationInterfacePage::integrate();
synthese::interfaces::DateTimeInterfacePage::integrate();

synthese::interfaces::SimplePageRequest::integrate();
synthese::interfaces::RedirRequest::integrate();

synthese::dblog::DBLogList::integrate();
synthese::dblog::DBLogViewer::integrate();

synthese::dblog::DBLogRight::integrate();

synthese::dblog::DBLogEntryTableSync::integrate();

synthese::admin::AdminInterfacePage::integrate();

synthese::admin::HomeAdmin::integrate();

synthese::admin::AdminRequest::integrate();

synthese::admin::AdminPagesTreeInterfaceElement::integrate();
synthese::admin::AdminPagePositionInterfaceElement::integrate();
synthese::admin::AdminContentInterfaceElement::integrate();


synthese::security::SecurityModule::integrate();

synthese::security::ProfileTableSync::integrate();
synthese::security::UserTableSync::integrate();

synthese::security::UserNameInterfaceElement::integrate();
synthese::security::UserIdInterfaceElement::integrate();
synthese::security::UserPhoneInterfaceElement::integrate();

synthese::security::UsersAdmin::integrate();
synthese::security::UserAdmin::integrate();
synthese::security::ProfilesAdmin::integrate();
synthese::security::ProfileAdmin::integrate();

synthese::security::AddUserAction::integrate();
synthese::security::DelUserAction::integrate();
synthese::security::AddProfileAction::integrate();
synthese::security::DeleteProfileAction::integrate();
synthese::security::AddRightAction::integrate();
synthese::security::DeleteRightAction::integrate();
synthese::security::UpdateRightAction::integrate();
synthese::security::UpdateProfileAction::integrate();
synthese::security::UserUpdateAction::integrate();
synthese::security::UserPasswordUpdateAction::integrate();

synthese::security::GlobalRight::integrate();
synthese::security::SecurityRight::integrate();

synthese::security::SecurityLog::integrate();

synthese::server::ServerModule::integrate();

synthese::server::LoginInterfacePage::integrate();

synthese::server::LoginAction::integrate();
synthese::server::LogoutAction::integrate();

synthese::server::HtmlFormInterfaceElement::integrate();
synthese::server::LogoutHTMLLinkInterfaceElement::integrate();
synthese::server::LoginHtmlField::integrate();
synthese::server::PasswordHtmlField::integrate();
synthese::server::RequestErrorMessageInterfaceElement::integrate();
synthese::server::ValidSessionInterfaceElement::integrate();
synthese::server::ObjectIdInterfaceElement::integrate();

synthese::messages::ScenarioTableSync::integrate();
synthese::messages::AlarmTableSync::integrate();
synthese::messages::TextTemplateTableSync::integrate();
synthese::messages::AlarmObjectLinkTableSync::integrate();

synthese::messages::MessagesAdmin::integrate();
synthese::messages::MessageAdmin::integrate();
synthese::messages::MessagesLibraryAdmin::integrate();
synthese::messages::MessagesScenarioAdmin::integrate();

synthese::messages::MessagesRight::integrate();
synthese::messages::MessagesLibraryRight::integrate();

synthese::messages::MessagesLog::integrate();
synthese::messages::MessagesLibraryLog::integrate();

synthese::messages::ScenarioUpdateDatesAction::integrate();
synthese::messages::ScenarioStopAction::integrate();
synthese::messages::DeleteAlarmAction::integrate();
synthese::messages::UpdateAlarmMessagesFromTemplateAction::integrate();
synthese::messages::UpdateAlarmAction::integrate();
synthese::messages::NewMessageAction::integrate();
synthese::messages::NewScenarioSendAction::integrate();
synthese::messages::ScenarioNameUpdateAction::integrate();
synthese::messages::DeleteTextTemplateAction::integrate();
synthese::messages::UpdateTextTemplateAction::integrate();
synthese::messages::AlarmStopAction::integrate();
synthese::messages::AddScenarioAction::integrate();
synthese::messages::DeleteScenarioAction::integrate();
synthese::messages::TextTemplateAddAction::integrate();
synthese::messages::UpdateAlarmMessagesAction::integrate();
synthese::messages::AlarmRemoveLinkAction::integrate();
synthese::messages::AlarmAddLinkAction::integrate();

synthese::impex::TridentExportAction::integrate();


synthese::resa::ResaRight::integrate();

synthese::resa::BookReservationAction::integrate();

synthese::resa::ReservationsListFunction::integrate();

synthese::resa::ReservationsListInterfacePage::integrate();

synthese::resa::ReservationsListInterfaceElement::integrate();
synthese::resa::BookableCommercialLinesInterfaceElement::integrate();

synthese::resa::ReservationTableSync::integrate();
synthese::resa::ReservationTransactionTableSync::integrate();
synthese::resa::OnlineReservationRuleTableSync::integrate();

synthese::routeplanner::RoutePlannerSheetLinesCellInterfacePage::integrate();
synthese::routeplanner::RoutePlannerSheetLineInterfacePage::integrate();
synthese::routeplanner::RoutePlannerSheetColumnInterfacePage::integrate();
synthese::routeplanner::JourneyBoardStopCellInterfacePage::integrate();
synthese::routeplanner::JourneyBoardServiceCellInterfacePage::integrate();
synthese::routeplanner::JourneyBoardJunctionCellInterfacePage::integrate();
synthese::routeplanner::RoutePlannerNoSolutionInterfacePage::integrate();
synthese::routeplanner::RoutePlannerInterfacePage::integrate();
synthese::routeplanner::JourneyBoardInterfacePage::integrate();

synthese::routeplanner::JourneyLineListInterfaceElement::integrate();
synthese::routeplanner::JourneyBoardInterfaceElement::integrate();
synthese::routeplanner::SchedulesTableInterfaceElement::integrate();
synthese::routeplanner::ScheduleSheetLinesListLineInterfaceElement::integrate();
synthese::routeplanner::ScheduleSheetDurationRowInterfaceElement::integrate();
synthese::routeplanner::ScheduleSheetReservationRowInterfaceElement::integrate();
synthese::routeplanner::DatesListInterfaceElement::integrate();
synthese::routeplanner::TextInputInterfaceElemet::integrate();
synthese::routeplanner::RequestInterfaceElement::integrate();
synthese::routeplanner::PeriodsListInterfaceElement::integrate();
synthese::routeplanner::JourneyBoardsInterfaceElement::integrate();

synthese::routeplanner::RoutePlannerFunction::integrate();

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


synthese::departurestable::DisplayTypeTableSync::integrate();
synthese::departurestable::DisplayScreenTableSync::integrate();


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

synthese::departurestable::ArrivalDepartureTableRight::integrate();
synthese::departurestable::DisplayMaintenanceRight::integrate();

synthese::departurestable::ArrivalDepartureTableLog::integrate();
synthese::departurestable::DisplayMaintenanceLog::integrate();
synthese::departurestable::DisplayDataControlLog::integrate();

synthese::departurestable::DisplayScreenAlarmRecipient::integrate();


synthese::transportwebsite::SiteTableSync::integrate();

synthese::transportwebsite::PlacesListModule::integrate();

synthese::transportwebsite::CityListInterfaceElement::integrate();
synthese::transportwebsite::CityListRequestInterfaceElement::integrate();
synthese::transportwebsite::PlacesListRequestInterfaceElement::integrate();

synthese::transportwebsite::PlacesListInterfacePage::integrate();
synthese::transportwebsite::PlacesListItemInterfacePage::integrate();

synthese::transportwebsite::CityListRequest::integrate();
synthese::transportwebsite::PlacesListFunction::integrate();

