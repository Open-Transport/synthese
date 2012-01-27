
// 52 Forum Module

// Factories
#include "ForumModule.hpp"

#include "ForumRight.hpp"

#include "ForumTopicsAdmin.hpp"
#include "ForumMessagesAdmin.hpp"

#include "ForumTopicTableSync.hpp"
#include "ForumMessageTableSync.hpp"

#include "ForumTopicPostAction.hpp"
#include "ForumMessagePostAction.hpp"

#include "ForumTopicsFunction.hpp"
#include "ForumMessagesFunction.hpp"

// Registries
#include "ForumTopic.hpp"
#include "ForumMessage.hpp"


#include "ForumModule.inc.cpp"

void synthese::forum::moduleRegister()
{

	// 52 Forum Module

	// Factories
	synthese::forum::ForumModule::integrate();

	synthese::forum::ForumRight::integrate();

	synthese::forum::ForumMessagesAdmin::integrate();
	synthese::forum::ForumTopicsAdmin::integrate();

	synthese::forum::ForumTopicTableSync::integrate();
	synthese::forum::ForumMessageTableSync::integrate();

	synthese::forum::ForumTopicPostAction::integrate();
	synthese::forum::ForumMessagePostAction::integrate();

	synthese::forum::ForumTopicsFunction::integrate();
	synthese::forum::ForumMessagesFunction::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::forum::ForumMessage>();
	synthese::util::Env::Integrate<synthese::forum::ForumTopic>();
}
