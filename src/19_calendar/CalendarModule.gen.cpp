
// 19 calendar module

// Factories

synthese::calendar::CalendarTemplateTableSync::integrate();
synthese::calendar::CalendarTemplateElementTableSync::integrate();

synthese::calendar::CalendarTemplateAdmin::integrate();
synthese::calendar::CalendarTemplatesAdmin::integrate();

synthese::calendar::CalendarTemplateAddAction::integrate();
synthese::calendar::CalendarTemplateElementAddAction::integrate();

synthese::calendar::CalendarModule::integrate();
synthese::calendar::CalendarRight::integrate();

// Registries
synthese::util::Env::Integrate<synthese::calendar::CalendarTemplateElement>();
synthese::util::Env::Integrate<synthese::calendar::CalendarTemplate>();
