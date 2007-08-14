
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::admin::AdminInterfacePage>("admin");

synthese::admin::HomeAdmin::integrate();

synthese::util::Factory<synthese::server::Function>::integrate<synthese::admin::AdminRequest>("admin");

synthese::admin::AdminPagesTreeInterfaceElement::integrate();
synthese::admin::AdminPagePositionInterfaceElement::integrate();
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::admin::AdminContentInterfaceElement>("admincontent");

