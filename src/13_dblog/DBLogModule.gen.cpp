
synthese::dblog::DBLogModule::integrate();

synthese::dblog::DBLogViewer::integrate();

synthese::dblog::DBLogRight::integrate();

synthese::dblog::DBLogEntryTableSync::integrate();

// Registries
synthese::util::Env::Integrate<synthese::dblog::DBLogEntry>();
