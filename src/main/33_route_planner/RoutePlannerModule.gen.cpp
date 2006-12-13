Factory<InterfacePage>::integrate<RoutePlannerSheetLinesCellInterfacePage>("schedule_sheet_lines_cell");
Factory<InterfacePage>::integrate<RoutePlannerSheetLineInterfacePage>("schedule_sheet_row");
Factory<InterfacePage>::integrate<RoutePlannerSheetColumnInterfacePage>("schedule_sheet_column");
Factory<InterfacePage>::integrate<RoutePlannerNoSolutionInterfacePage>("routeplanner_no_solution");
Factory<InterfacePage>::integrate<JourneyBoardStopCellInterfacePage>("journey_board_stop_cell");
Factory<InterfacePage>::integrate<JourneyBoardServiceCellInterfacePage>("journey_board_service_cell");
Factory<InterfacePage>::integrate<JourneyBoardJunctionCellInterfacePage>("journey_board_junction_cell");

Factory<LibraryInterfaceElement>::integrate<JourneyLineListInterfaceElement>("journey_line_list");
Factory<LibraryInterfaceElement>::integrate<JourneyBoardInterfaceElement>("journey_board");

