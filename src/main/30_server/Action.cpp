
#include "Action.h"

namespace synthese
{
	namespace server
	{
		const std::string Action::PARAMETER_ACTION = "a";
		const std::string Action::PARAMETER_PREFIX = "actionParam";

		Action* Action::create(const Request*request, Request::ParametersMap& params )
		{
			Request::ParametersMap::iterator it = params.find(PARAMETER_ACTION);
			if (it == params.end())
				return NULL;

			params.erase(it);
			Action* action = Factory<Action>::create(it->second);

			action->setFromParametersMap(params);

			return action;		
		}
	}
}