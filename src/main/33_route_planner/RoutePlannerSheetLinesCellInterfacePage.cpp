
#include "RoutePlannerSheetLinesCellInterfacePage.h"

#include "30_server/Request.h"


namespace synthese
{
	namespace interfaces
	{
		void RoutePlannerSheetLinesCellInterfacePage::display( 
		    std::ostream& stream, 
		    size_t columnNumber, 
		    const synthese::routeplanner::Journey* object, 
		    const server::Request* request /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back( synthese::util::Conversion::ToString( columnNumber ) );
			
			InterfacePage::display( stream, pv, (const void*) object, request );
		}
	}
}
