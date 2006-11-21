
#include "LogoutAction.h"

namespace synthese
{
	namespace server
	{
		server::Request::ParametersMap LogoutAction::getParametersMap() const
		{
			Request::ParametersMap map;
			return map;
		}

		void LogoutAction::setFromParametersMap( server::Request::ParametersMap& map )
		{

		}

		void LogoutAction::run() const
		{
			_request->deleteSession();
		}
	}
}