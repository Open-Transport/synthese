
// IMPEX 16

// Factories

synthese::impex::ImpExModule::integrate();

synthese::impex::ImportFunction::integrate ();
synthese::impex::ExportFunction::integrate ();

synthese::impex::DataSourceTableSync::integrate();

synthese::impex::DataSourcesAdmin::integrate();
synthese::impex::DataSourceAdmin::integrate();

synthese::impex::DataSourceUpdateAction::integrate();
synthese::impex::DataSourceRemoveAction::integrate();

// Registries

synthese::util::Env::Integrate<synthese::impex::DataSource>();
