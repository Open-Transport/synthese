
#include "01_util/Factorable.h"

#include "30_server/Request.h"

namespace synthese
{
	namespace server
	{
		/** Action to run before the display of a function result.

			A factory of actions is handled by 30_server module.

		*/
		class Action : public util::Factorable
		{
		public:
			static const std::string PARAMETER_ACTION;
			static const std::string PARAMETER_PREFIX;

		protected:
			Request* _request;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
			*/
			virtual void setFromParametersMap(Request::ParametersMap& map) = 0;

		public:
			/** Conversion from attributes to generic parameter maps.
			*/
			virtual Request::ParametersMap getParametersMap() const = 0;

			/** Action to run, defined by each subclass.
			*/
			virtual void run() const = 0;

			/** Action creation from a request.
			*/
			static Action* create(const Request*request, Request::ParametersMap& params);
		};
	}
}