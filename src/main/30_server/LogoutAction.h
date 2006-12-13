
#ifndef SYNTHESE_LogoutAction_H__
#define SYNTHESE_LogoutAction_H__

#include "30_server/Action.h"

namespace synthese
{
	namespace server
	{
		class LogoutAction : public server::Action
		{
		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void setFromParametersMap(server::Request::ParametersMap& map);

		public:

			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_LogoutAction_H__

