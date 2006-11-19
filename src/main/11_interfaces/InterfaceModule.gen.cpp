
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::interfaces::InterfaceModule>("11_interfaces");

synthese::util::Factory<ValueInterfaceElement>::integrate<StaticValueInterfaceElement>("text");
synthese::util::Factory<ValueInterfaceElement>::integrate<ParameterValueInterfaceElement>("param");

synthese::util::Factory<LibraryInterfaceElement>::integrate<StaticValueInterfaceElement>("text");
synthese::util::Factory<LibraryInterfaceElement>::integrate<ParameterValueInterfaceElement>("param");
synthese::util::Factory<LibraryInterfaceElement>::integrate<LineLabelInterfaceElement>("label");
synthese::util::Factory<LibraryInterfaceElement>::integrate<IncludePageInterfaceElement>("include");
synthese::util::Factory<LibraryInterfaceElement>::integrate<IfThenElseInterfaceElement>("if");
synthese::util::Factory<LibraryInterfaceElement>::integrate<GotoInterfaceElement>("goto");

//synthese::util::Factory<SQLiteTableSync>::integrate<InterfaceTableSync>("16.01");