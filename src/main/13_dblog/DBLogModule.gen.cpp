
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::dblog::DBLogList>("dblogs");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::dblog::DBLogViewer>("dblog");

synthese::util::Factory<synthese::security::Right>::integrate<synthese::dblog::DBLogRight>("Logs");