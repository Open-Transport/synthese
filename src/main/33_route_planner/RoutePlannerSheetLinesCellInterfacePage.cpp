
#include "RoutePlannerSheetLinesCellInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		void RoutePlannerSheetLinesCellInterfacePage::display( std::ostream& stream, size_t columnNumber
			, const synthese::routeplanner::Journey* object, const Site* site /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back( synthese::util::Conversion::ToString( columnNumber ) );
			
			InterfacePage::display( stream, pv, (const void*) object, site );
		}
	}
}