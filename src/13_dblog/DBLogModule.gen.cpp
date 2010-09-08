
synthese::dblog::DBLogModule::integrate();

synthese::dblog::DBLogAdmin::integrate();

synthese::dblog::DBLogRight::integrate();

synthese::dblog::DBLogEntryTableSync::integrate();
synthese::dblog::DBLogPurgeAction::integrate();

// Registries
synthese::util::Env::Integrate<synthese::dblog::DBLogEntry>();
