
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::server::ServerModule>("999_server");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::server::ServerConfigTableSync>("0 Config");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::server::SiteTableSync>("30.01 Site");

synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LoginAction>("login");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LogoutAction>("logout");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::server::SimpleLoginInterfacePage>("simplelogin");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::server::RedirectInterfacePage>("redir");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::HtmlFormInterfaceElement>("htmlform");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::LogoutHTMLLinkInterfaceElement>("logouthtmllink");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::LoginHtmlField>("loginhtmlfield");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::PasswordHtmlField>("passwordhtmlfield");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::RequestErrorMessageInterfaceElement>("errormessage");

synthese::util::Factory<synthese::server::Request>::integrate<synthese::server::SimplePageRequest>("page");
synthese::util::Factory<synthese::server::Request>::integrate<synthese::server::ActionOnlyRequest>("action");
