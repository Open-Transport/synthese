
// TIMETABLES 55

// Factories

synthese::timetables::TimetableModule::integrate();

synthese::timetables::TimetableRowTableSync::integrate();
synthese::timetables::TimetableTableSync::integrate();

synthese::timetables::TimetableAdmin::integrate();

synthese::timetables::TimetableAddAction::integrate();
synthese::timetables::TimetableRowAddAction::integrate();
synthese::timetables::TimetableUpdateAction::integrate();
synthese::timetables::TimetableRowDeleteAction::integrate();
synthese::timetables::TimetableSetLineAction::integrate();
synthese::timetables::TimetableSetPhysicalStopAction::integrate();
synthese::timetables::TimetableTransferUpdateAction::integrate();

synthese::timetables::TimetableRight::integrate();

synthese::timetables::TimetableGenerateFunction::integrate();

// Registries
synthese::util::Env::Integrate<synthese::timetables::TimetableRow>();
synthese::util::Env::Integrate<synthese::timetables::Timetable>();
