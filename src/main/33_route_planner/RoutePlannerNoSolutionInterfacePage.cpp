
#include "RoutePlannerNoSolutionInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		void RoutePlannerNoSolutionInterfacePage::display( std::ostream& stream , const server::Request* request /*= NULL*/ ) const
		{
			InterfacePage::display(stream, ParametersVector(), NULL, request);
		}
	}
}
