
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::server::ServerModule>("999_server");


synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LoginAction>("login");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LogoutAction>("logout");

synthese::server::HtmlFormInterfaceElement::integrate();
synthese::server::LogoutHTMLLinkInterfaceElement::integrate();
synthese::server::LoginHtmlField::integrate();
synthese::server::PasswordHtmlField::integrate();
synthese::server::RequestErrorMessageInterfaceElement::integrate();
