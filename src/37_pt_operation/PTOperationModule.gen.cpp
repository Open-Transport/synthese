
// 37 PT Operation
synthese::pt_operation::CompositionTableSync::integrate();
synthese::pt_operation::VehicleTableSync::integrate();

synthese::pt_operation::PTOperationModule::integrate();

synthese::pt_operation::GetCompositionsService::integrate();
synthese::pt_operation::GetVehiclesService::integrate();

synthese::pt_operation::CompositionVehicleAddAction::integrate();
synthese::pt_operation::CompositionVehicleRemoveAction::integrate();
synthese::pt_operation::CompositionUpdateAction::integrate();
synthese::pt_operation::VehicleUpdateAction::integrate();

synthese::pt_operation::VehicleAdmin::integrate();
synthese::pt_operation::VehiclesAdmin::integrate();

synthese::util::Env::Integrate<synthese::pt_operation::Composition>();
synthese::util::Env::Integrate<synthese::pt_operation::Vehicle>();
synthese::util::Env::Integrate<synthese::pt_operation::VehiclePosition>();

