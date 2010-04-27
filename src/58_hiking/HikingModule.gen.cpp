
// 58 Hiking Module

// Factories
synthese::hiking::HikingModule::integrate();

synthese::hiking::HikingTrailTableSync::integrate();

synthese::hiking::HikingRight::integrate();

synthese::hiking::HikingTrailDisplayFunction::integrate();
synthese::hiking::HikingTrailSearchFunction::integrate();

synthese::hiking::HikingTrailAddAction::integrate();
synthese::hiking::HikingTrailUpdateAction::integrate();
synthese::hiking::HikingTrailRemoveAction::integrate();
synthese::hiking::HikingTrailStopAddAction::integrate();
synthese::hiking::HikingTrailStopRemoveAction::integrate();

synthese::hiking::HikingTrailAdmin::integrate();
synthese::hiking::HikingTrailsAdmin::integrate();

// Registries

synthese::util::Env::Integrate<synthese::hiking::HikingTrail>();

