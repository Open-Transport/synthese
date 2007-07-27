
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::interfaces::InterfaceModule>("11_interfaces");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::ParameterValueInterfaceElement>("param");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::IfThenElseInterfaceElement>("if");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::FormattedNumberInterfaceElement>("format");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::EqualsValueInterfaceElement>("=");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::ChrInterfaceElement>("chr");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::SuperiorValueInterfaceElement>(">");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::ConcatenateValueInterfaceElement>("$");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::SubstrValueInterfaceElement>("substr");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::AndValueInterfaceElement>("&&");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::PlainCharFilterValueInterfaceElement>("plain_char");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::CommentInterfaceElement>("//");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::PrintInterfaceElement>("print");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::LineLabelInterfaceElement>("label");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::IncludePageInterfaceElement>("include");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::GotoInterfaceElement>("goto");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::SetInterfaceElement>("set");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::interfaces::InterfaceTableSync>("16.01 Interfaces");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::interfaces::InterfacePageTableSync>("16.02 Interface Pages");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::interfaces::RedirectInterfacePage>("redir");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::interfaces::DurationInterfacePage>("duration");

synthese::util::Factory<synthese::server::Function>::integrate<synthese::interfaces::SimplePageRequest>("page");

