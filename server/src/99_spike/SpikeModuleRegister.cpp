
// 62 Spike module

#include "Env.h"
#include "SpikeModule.hpp"
#include "FwChild1.hpp"
#include "FwChild2.hpp"

// Registries
#include "SpikeModule.inc.cpp"


void synthese::spike::moduleRegister()
{

	// 62 Spike Module

	//synthese::spike::Fwchild1TableSync::integrate();

	synthese::spike::SpikeModule::integrate();

	synthese::util::Env::Integrate<synthese::spike::FwChild1>();
	synthese::util::Env::Integrate<synthese::spike::FwChild2>();


}
