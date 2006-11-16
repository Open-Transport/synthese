
#include "RoutePlannerSheetColumnInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		bool RoutePlannerSheetColumnInterfacePage::_registered = Factory<InterfacePage>::integrate<RoutePlannerSheetColumnInterfacePage>("schedule_sheet_column");

		void RoutePlannerSheetColumnInterfacePage::display( std::ostream& stream
			, bool isItFirstLine, bool isItLastLine, size_t columnNumber, bool isItFootLine 
			, const synthese::time::Hour& firstDepartureTime, const synthese::time::Hour& lastDepartureTime 
			, bool isItContinuousService, const Site* site /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back( synthese::util::Conversion::ToString( isItFirstLine ) );
			pv.push_back( synthese::util::Conversion::ToString( isItLastLine ) );
			pv.push_back( synthese::util::Conversion::ToString( columnNumber ) );
			pv.push_back( synthese::util::Conversion::ToString( isItFootLine ) );
			pv.push_back( firstDepartureTime.isUnknown() ? "" : firstDepartureTime.toInternalString() );
			pv.push_back( firstDepartureTime.isUnknown() ? "" : lastDepartureTime.toInternalString() );
			pv.push_back( synthese::util::Conversion::ToString( isItContinuousService ) );

			InterfacePage::display( stream, pv, NULL, site );
			
		}
	}
}