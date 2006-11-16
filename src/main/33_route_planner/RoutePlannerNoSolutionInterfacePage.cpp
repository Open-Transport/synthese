
#include "RoutePlannerNoSolutionInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		const bool RoutePlannerNoSolutionInterfacePage::_registered = Factory<InterfacePage>::integrate<RoutePlannerNoSolutionInterfacePage>("routeplanner_no_solution");

		void RoutePlannerNoSolutionInterfacePage::display( std::ostream& stream , const Site* site /*= NULL*/ ) const
		{
			InterfacePage::display(stream, ParametersVector(), NULL, site);
		}
	}
}