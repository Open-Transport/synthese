
#include "ScheduleSheetLinesListLineInterfaceElement.h"
#include "11_interfaces/Site.h"
#include "11_interfaces/Interface.h"
#include "RoutePlanner.h"
#include "RoutePlannerSheetLinesCellInterfacePage.h"

using namespace synthese::routeplanner;

namespace synthese
{
	namespace interfaces
	{
		const bool ScheduleSheetLinesListLineInterfaceElement::_registered = Factory<LibraryInterfaceElement>::integrate<ScheduleSheetLinesListLineInterfaceElement>("schedules_sheet_lines_lists_line");

		void ScheduleSheetLinesListLineInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const Site* site /*= NULL*/ ) const
		{
			const JourneyVector* jv = (const JourneyVector*) object;
			const RoutePlannerSheetLinesCellInterfacePage* linesInterfacePage = site->getInterface()->getPage<RoutePlannerSheetLinesCellInterfacePage>();

			int n = 1;
			for ( JourneyVector::const_iterator it = jv->begin(); it != jv->end(); ++it, ++n )
			{
				linesInterfacePage->display( stream, n, &(*it), site );
			}
		}
	}
}