
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::admin::AdminInterfacePage>("admin");

synthese::util::Factory<synthese::interfaces::AdminInterfaceElement>::integrate<synthese::admin::HomeAdmin>("home");

synthese::util::Factory<synthese::server::Request>::integrate<synthese::admin::AdminRequest>("admin");