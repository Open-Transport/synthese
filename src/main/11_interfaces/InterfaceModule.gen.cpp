Factory<ModuleClass>::integrate<InterfaceModule>("11_interfaces");

Factory<InterfacePage>::integrate<AdminInterfacePage>("admin");

Factory<Request>::integrate<AdminRequest>("admin");

Factory<ValueInterfaceElement>::integrate<StaticValueInterfaceElement>("text");
Factory<ValueInterfaceElement>::integrate<ParameterValueInterfaceElement>("param");

Factory<LibraryInterfaceElement>::integrate<StaticValueInterfaceElement>("text");
Factory<LibraryInterfaceElement>::integrate<ParameterValueInterfaceElement>("param");
Factory<LibraryInterfaceElement>::integrate<LineLabelInterfaceElement>("label");
Factory<LibraryInterfaceElement>::integrate<IncludePageInterfaceElement>("include");
Factory<LibraryInterfaceElement>::integrate<IfThenElseInterfaceElement>("if");
Factory<LibraryInterfaceElement>::integrate<GotoInterfaceElement>("goto");

Factory<AdminInterfaceElement>::integrate<HomeAdmin>("home");

Factory<SQLiteTableSync>::integrate<Interface>("16.01");