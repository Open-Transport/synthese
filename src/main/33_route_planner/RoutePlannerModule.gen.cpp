
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::routeplanner::SiteTableSync>("30.01 Site");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerSheetLinesCellInterfacePage>("schedule_sheet_lines_cell");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerSheetLineInterfacePage>("schedule_sheet_row");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerSheetColumnInterfacePage>("schedule_sheet_column");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::RoutePlannerNoSolutionInterfacePage>("routeplanner_no_solution");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::JourneyBoardStopCellInterfacePage>("journey_board_stop_cell");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::JourneyBoardServiceCellInterfacePage>("journey_board_service_cell");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::routeplanner::JourneyBoardJunctionCellInterfacePage>("journey_board_junction_cell");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::JourneyLineListInterfaceElement>("journey_line_list");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::routeplanner::JourneyBoardInterfaceElement>("journey_board");

