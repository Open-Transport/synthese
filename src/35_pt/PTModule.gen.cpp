
// PT MODULE 35

// Factories

synthese::pt::TransportNetworkTableSync::integrate();
synthese::pt::TridentExportFunction::integrate ();
synthese::pt::TridentFileFormat::integrate ();
synthese::pt::ServiceDateTableSync::integrate();
synthese::pt::PTUseRuleTableSync::integrate();
synthese::pt::PTModule::integrate();

// Registries

synthese::util::Env::Integrate<synthese::pt::TransportNetwork>();
synthese::util::Env::Integrate<synthese::pt::ServiceDate>();
synthese::util::Env::Integrate<synthese::pt::PTUseRule>();

