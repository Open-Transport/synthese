
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::admin::AdminInterfacePage>("admin");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::admin::HomeAdmin>("home");

synthese::util::Factory<synthese::server::Request>::integrate<synthese::admin::AdminRequest>("admin");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::admin::AdminPagesTreeInterfaceElement>("admintree");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::admin::AdminPagePositionInterfaceElement>("adminpos");
