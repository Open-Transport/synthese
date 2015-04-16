
// Fare module 30

// Factories

#include "FareModule.hpp"

#include "FareTableSync.hpp"

#include "FareAdmin.hpp"
#include "FaresAdmin.hpp"

#include "FareUpdateAction.hpp"

// Registries

#include "Fare.hpp"

#include "FareModule.inc.cpp"

void synthese::fare::moduleRegister()
{

	// Fare MODULE 30

	// Factories
	synthese::fare::FareTableSync::integrate();

	synthese::fare::FareModule::integrate();

	synthese::fare::FareAdmin::integrate();
	synthese::fare::FaresAdmin::integrate();

	synthese::fare::FareUpdateAction::integrate();


	// Registries
	synthese::util::Env::Integrate<synthese::fare::Fare>();
}
