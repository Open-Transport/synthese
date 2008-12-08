
synthese::routeplanner::RoutePlannerSheetLinesCellInterfacePage::integrate();
synthese::routeplanner::RoutePlannerSheetLineInterfacePage::integrate();
synthese::routeplanner::RoutePlannerSheetColumnInterfacePage::integrate();
synthese::routeplanner::JourneyBoardStopCellInterfacePage::integrate();
synthese::routeplanner::JourneyBoardServiceCellInterfacePage::integrate();
synthese::routeplanner::JourneyBoardJunctionCellInterfacePage::integrate();
synthese::routeplanner::RoutePlannerNoSolutionInterfacePage::integrate();
synthese::routeplanner::RoutePlannerInterfacePage::integrate();
synthese::routeplanner::JourneyBoardInterfacePage::integrate();
synthese::routeplanner::UserFavoriteInterfacePage::integrate();

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
synthese::routeplanner::UserFavoriteListInterfaceElement::integrate();

synthese::routeplanner::RoutePlannerFunction::integrate();

synthese::routeplanner::UserFavoriteJourneyTableSync::integrate();

// Registries
synthese::util::Env::template Integrate<synthese::routeplanner::UserFavoriteJourney>();