
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerSheetLinesCellInterfacePage>("schedule_sheet_lines_cell");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerSheetLineInterfacePage>("schedule_sheet_row");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerSheetColumnInterfacePage>("schedule_sheet_column");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::JourneyBoardStopCellInterfacePage>("journey_board_stop_cell");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::JourneyBoardServiceCellInterfacePage>("journey_board_service_cell");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::JourneyBoardJunctionCellInterfacePage>("journey_board_junction_cell");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerNoSolutionInterfacePage>("routeplanner_no_solution");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerInterfacePage>("route_planner");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::JourneyBoardInterfacePage>("journey_board");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::JourneyLineListInterfaceElement>("journey_line_list");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::JourneyBoardInterfaceElement>("journey_board");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::SchedulesTableInterfaceElement>("schedules_table");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::ScheduleSheetLinesListLineInterfaceElement>("schedules_lines");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::ScheduleSheetDurationRowInterfaceElement>("schedules_durations");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::ScheduleSheetReservationRowInterfaceElement>("schedules_reservation");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::DatesListInterfaceElement>("dates_list");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::TextInputInterfaceElemet>("route_planner_input");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::RequestInterfaceElement>("route_planner_form");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::PeriodsListInterfaceElement>("route_planner_periods");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::JourneyBoardsInterfaceElement>("journey_boards");

synthese::util::Factory<synthese::server::Function>::integrate<synthese::routeplanner::RoutePlannerFunction>("rp");
