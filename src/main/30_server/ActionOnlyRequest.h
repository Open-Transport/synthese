
#ifndef SYNTHESE_ActionOnlyRequest_H__
#define SYNTHESE_ActionOnlyRequest_H__

#include "30_server/Request.h"

namespace synthese
{
	namespace server
	{
		class ActionOnlyRequest : public server::Request
		{
			static const std::string PARAMETER_URL;

			//! \name Request parameters
			//@{
			std::string _url;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			server::Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void setFromParametersMap(const server::Request::ParametersMap& map);

		public:
			ActionOnlyRequest();
			~ActionOnlyRequest();

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream) const;

			void setURL(const std::string& url);

		};
	}
}
#endif // SYNTHESE_ActionOnlyRequest_H__
