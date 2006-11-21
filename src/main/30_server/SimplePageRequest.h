
#ifndef SYNTHESE_SimplePageRequest_H__
#define SYNTHESE_SimplePageRequest_H__


#include "30_server/Request.h"

namespace synthese
{
	namespace interfaces
	{
		class InterfacePage;
	}

	namespace server
	{
		class SimplePageRequest : public server::Request
		{
			static const std::string PARAMETER_PAGE;

			//! \name Request parameters
			//@{
			const interfaces::InterfacePage* _page;
			Request::ParametersMap _parameters;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			server::Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void setFromParametersMap(const server::Request::ParametersMap& map);

		public:

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream) const;

			void setPage(const interfaces::InterfacePage* page);
		};
	}
}
#endif // SYNTHESE_SimplePageRequest_H__
