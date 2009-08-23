
synthese::interfaces::InterfaceModule::integrate();

synthese::interfaces::ParameterValueInterfaceElement::integrate();
synthese::interfaces::IfThenElseInterfaceElement::integrate();
synthese::interfaces::FormattedNumberInterfaceElement::integrate();
synthese::interfaces::EqualsValueInterfaceElement::integrate();
synthese::interfaces::ChrInterfaceElement::integrate();
synthese::interfaces::SuperiorValueInterfaceElement::integrate();
synthese::interfaces::SubstrValueInterfaceElement::integrate();
synthese::interfaces::AndValueInterfaceElement::integrate();
synthese::interfaces::PlainCharFilterValueInterfaceElement::integrate();
synthese::interfaces::InterfaceIdInterfaceElement::integrate();

synthese::interfaces::CommentInterfaceElement::integrate();
synthese::interfaces::PrintInterfaceElement::integrate();
synthese::interfaces::DebugInterfaceElement::integrate();
synthese::interfaces::LineLabelInterfaceElement::integrate();
synthese::interfaces::IncludePageInterfaceElement::integrate();
synthese::interfaces::GotoInterfaceElement::integrate();
synthese::interfaces::SetInterfaceElement::integrate();
synthese::interfaces::StrLenInterfaceElement::integrate();

synthese::interfaces::InterfaceTableSync::integrate();
synthese::interfaces::InterfacePageTableSync::integrate();

synthese::interfaces::DurationInterfacePage::integrate();
synthese::interfaces::DateTimeInterfacePage::integrate();
synthese::interfaces::NonPredefinedInterfacePage::integrate();

synthese::interfaces::SimplePageFunction::integrate();

// Registries
synthese::util::Env::Integrate<synthese::interfaces::Interface>();
synthese::util::Env::Integrate<synthese::interfaces::InterfacePage>();