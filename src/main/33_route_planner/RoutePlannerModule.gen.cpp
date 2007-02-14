
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::routeplanner::SiteTableSync>("30.01 Site");

synthese::util::Factory<InterfacePage>::integrate<RoutePlannerSheetLinesCellInterfacePage>("schedule_sheet_lines_cell");
synthese::util::Factory<InterfacePage>::integrate<RoutePlannerSheetLineInterfacePage>("schedule_sheet_row");
synthese::util::Factory<InterfacePage>::integrate<RoutePlannerSheetColumnInterfacePage>("schedule_sheet_column");
synthese::util::Factory<InterfacePage>::integrate<RoutePlannerNoSolutionInterfacePage>("routeplanner_no_solution");
synthese::util::Factory<InterfacePage>::integrate<JourneyBoardStopCellInterfacePage>("journey_board_stop_cell");
synthese::util::Factory<InterfacePage>::integrate<JourneyBoardServiceCellInterfacePage>("journey_board_service_cell");
synthese::util::Factory<InterfacePage>::integrate<JourneyBoardJunctionCellInterfacePage>("journey_board_junction_cell");

synthese::util::Factory<LibraryInterfaceElement>::integrate<JourneyLineListInterfaceElement>("journey_line_list");
synthese::util::Factory<LibraryInterfaceElement>::integrate<JourneyBoardInterfaceElement>("journey_board");

