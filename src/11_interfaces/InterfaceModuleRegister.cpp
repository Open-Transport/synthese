
#include "InterfaceRight.h"

#include "InterfaceAdmin.h"
#include "InterfacePageAdmin.h"

#include "InterfaceUpdateAction.h"
#include "InterfacePageAddAction.h"
#include "InterfacePageUpdateAction.h"

#include "InterfaceTableSync.h"
#include "InterfacePageTableSync.h"
#include "InterfaceModule.h"

#include "11_interfaces/StaticValueInterfaceElement.h"
#include "11_interfaces/ParameterValueInterfaceElement.h"
#include "11_interfaces/ParameterValueInterfaceElement.h"
#include "11_interfaces/LineLabelInterfaceElement.h"
#include "11_interfaces/IncludePageInterfaceElement.h"
#include "11_interfaces/IfThenElseInterfaceElement.h"
#include "11_interfaces/GotoInterfaceElement.h"
#include "11_interfaces/PrintInterfaceElement.h"
#include "11_interfaces/DebugInterfaceElement.h"
#include "11_interfaces/CommentInterfaceElement.h"
#include "11_interfaces/FormattedNumberInterfaceElement.h"
#include "InterfaceIdInterfaceElement.h"
#include "LogicalAndInterfaceElement.h"
#include "LowerCaseInterfaceElement.h"
#include "11_interfaces/SetInterfaceElement.h"
#include "11_interfaces/StrLenInterfaceElement.h"
#include "11_interfaces/EqualsValueInterfaceElement.h"
#include "11_interfaces/ChrInterfaceElement.h"
#include "11_interfaces/SuperiorValueInterfaceElement.h"
#include "11_interfaces/SubstrValueInterfaceElement.h"
#include "11_interfaces/AndValueInterfaceElement.h"
#include "11_interfaces/PlainCharFilterValueInterfaceElement.h"
#include "11_interfaces/NonPredefinedInterfacePage.h"

#include "LineMarkerInterfacePage.h"

// Registries
#include "Interface.h"
#include "InterfacePage.h"


#include "InterfaceModule.inc.cpp"

void synthese::interfaces::moduleRegister()
{

	synthese::interfaces::InterfaceRight::integrate();

	synthese::interfaces::InterfaceModule::integrate();

	synthese::interfaces::InterfaceAdmin::integrate();
	synthese::interfaces::InterfacePageAdmin::integrate();

	synthese::interfaces::InterfacePageUpdateAction::integrate();
	synthese::interfaces::InterfacePageAddAction::integrate();

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
	synthese::interfaces::LogicalAndInterfaceElement::integrate();
	synthese::interfaces::CommentInterfaceElement::integrate();
	synthese::interfaces::PrintInterfaceElement::integrate();
	synthese::interfaces::DebugInterfaceElement::integrate();
	synthese::interfaces::LineLabelInterfaceElement::integrate();
	synthese::interfaces::IncludePageInterfaceElement::integrate();
	synthese::interfaces::GotoInterfaceElement::integrate();
	synthese::interfaces::SetInterfaceElement::integrate();
	synthese::interfaces::StrLenInterfaceElement::integrate();
	synthese::interfaces::LowerCaseInterfaceElement::integrate();

	synthese::interfaces::InterfaceTableSync::integrate();
	synthese::interfaces::InterfacePageTableSync::integrate();

	synthese::interfaces::NonPredefinedInterfacePage::integrate();
	synthese::pt::LineMarkerInterfacePage::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::interfaces::Interface>();
	synthese::util::Env::Integrate<synthese::interfaces::InterfacePage>();
}
