
#include "RoutePlannerSheetColumnInterfacePage.h"

#include "30_server/Request.h"
#include "30_server/Site.h"



namespace synthese
{
	namespace interfaces
	{
		void RoutePlannerSheetColumnInterfacePage::display( std::ostream& stream
			, bool isItFirstLine, bool isItLastLine, size_t columnNumber, bool isItFootLine 
			, const synthese::time::Hour& firstDepartureTime, const synthese::time::Hour& lastDepartureTime 
			, bool isItContinuousService, const server::Request* request /*= NULL */ ) const
		{
		    const server::Site* site = request->getSite ();
 
			ParametersVector pv;
			pv.push_back( synthese::util::Conversion::ToString( isItFirstLine ) );
			pv.push_back( synthese::util::Conversion::ToString( isItLastLine ) );
			pv.push_back( synthese::util::Conversion::ToString( columnNumber ) );
			pv.push_back( synthese::util::Conversion::ToString( isItFootLine ) );
			pv.push_back( firstDepartureTime.isUnknown() ? "" : firstDepartureTime.toInternalString() );
			pv.push_back( firstDepartureTime.isUnknown() ? "" : lastDepartureTime.toInternalString() );
			pv.push_back( synthese::util::Conversion::ToString( isItContinuousService ) );

			InterfacePage::display( stream, pv, NULL, request );
			
		}
	}
}
