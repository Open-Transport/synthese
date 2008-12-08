
synthese::dblog::DBLogModule::integrate();

synthese::dblog::DBLogViewer::integrate();

synthese::dblog::DBLogRight::integrate();

synthese::dblog::DBLogEntryTableSync::integrate();

// Registries
synthese::util::Env::template Integrate<synthese::dblog::DBLogEntry>();
