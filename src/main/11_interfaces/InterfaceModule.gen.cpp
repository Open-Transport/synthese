
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::interfaces::InterfaceModule>("11_interfaces");

synthese::interfaces::ParameterValueInterfaceElement::integrate();
synthese::interfaces::IfThenElseInterfaceElement::integrate();
synthese::interfaces::FormattedNumberInterfaceElement::integrate();
synthese::interfaces::EqualsValueInterfaceElement::integrate();
synthese::interfaces::ChrInterfaceElement::integrate();
synthese::interfaces::SuperiorValueInterfaceElement::integrate();
synthese::interfaces::SubstrValueInterfaceElement::integrate();
synthese::interfaces::AndValueInterfaceElement::integrate();
synthese::interfaces::PlainCharFilterValueInterfaceElement::integrate();

synthese::interfaces::CommentInterfaceElement::integrate();
synthese::interfaces::PrintInterfaceElement::integrate();
synthese::interfaces::DebugInterfaceElement::integrate();
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::LineLabelInterfaceElement>("label");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::IncludePageInterfaceElement>("include");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::GotoInterfaceElement>("goto");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::SetInterfaceElement>("set");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::interfaces::InterfaceTableSync>("16.01 Interfaces");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::interfaces::InterfacePageTableSync>("16.02 Interface Pages");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::interfaces::RedirectInterfacePage>("redir");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::interfaces::DurationInterfacePage>("duration");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::interfaces::DateTimeInterfacePage>("datetime");

synthese::util::Factory<synthese::server::Function>::integrate<synthese::interfaces::SimplePageRequest>("page");

