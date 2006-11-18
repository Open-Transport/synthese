
#include "RoutePlannerNoSolutionInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		void RoutePlannerNoSolutionInterfacePage::display( std::ostream& stream , const Site* site /*= NULL*/ ) const
		{
			InterfacePage::display(stream, ParametersVector(), NULL, site);
		}
	}
}