
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::admin::AdminInterfacePage>("admin");

synthese::admin::HomeAdmin::integrate();

synthese::util::Factory<synthese::server::Function>::integrate<synthese::admin::AdminRequest>("admin");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::admin::AdminPagesTreeInterfaceElement>("admintree");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::admin::AdminPagePositionInterfaceElement>("adminpos");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::admin::AdminContentInterfaceElement>("admincontent");

