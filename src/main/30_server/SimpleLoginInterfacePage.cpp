
#include "SimpleLoginInterfacePage.h"

namespace synthese
{
	using namespace interfaces;

	namespace server
	{

		void SimpleLoginInterfacePage::display(std::ostream& stream, const void* object /*= NULL*/, const server::Request* request /*= NULL*/) const
		{
			ParametersVector pv;

			/// @todo Implement the building of the parameter vector with parameters of the function

			InterfacePage::display(stream, pv, object, request);
		}
	}
}
 
