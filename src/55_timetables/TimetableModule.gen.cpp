
// TIMETABLES 55

// Factories

synthese::timetables::TimetableModule::integrate();

synthese::timetables::TimetableRowTableSync::integrate();
synthese::timetables::TimetableTableSync::integrate();
synthese::timetables::CalendarTemplateTableSync::integrate();
synthese::timetables::CalendarTemplateElementTableSync::integrate();

synthese::timetables::CalendarTemplateAdmin::integrate();
synthese::timetables::CalendarTemplatesAdmin::integrate();
synthese::timetables::TimetableAdmin::integrate();
synthese::timetables::TimetableBookAdmin::integrate();

synthese::timetables::TimetableAddAction::integrate();
synthese::timetables::TimetableRowAddAction::integrate();
synthese::timetables::TimetableUpdateAction::integrate();
synthese::timetables::CalendarTemplateAddAction::integrate();

synthese::timetables::TimetableRight::integrate();

// Registries
synthese::util::Env::Integrate<synthese::timetables::CalendarTemplate>();
synthese::util::Env::Integrate<synthese::timetables::TimetableRow>();
synthese::util::Env::Integrate<synthese::timetables::Timetable>();
synthese::util::Env::Integrate<synthese::timetables::CalendarTemplateElement>();
