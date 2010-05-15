
// 52 Forum Module

// Factories
synthese::forum::ForumTopicTableSync::integrate();
synthese::forum::ForumMessageTableSync::integrate();

synthese::forum::ForumTopicPostAction::integrate();
synthese::forum::ForumMessagePostAction::integrate();

synthese::forum::ForumTopicsFunction::integrate();
synthese::forum::ForumMessagesFunction::integrate();

// Registries
synthese::util::Env::Integrate<synthese::forum::ForumMessage>();
synthese::util::Env::Integrate<synthese::forum::ForumTopic>();
