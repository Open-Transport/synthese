
#include "JourneyBoardStopCellInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		const bool JourneyBoardStopCellInterfacePage::_registered = Factory<InterfacePage>::integrate<JourneyBoardStopCellInterfacePage>("journey_board_stop_cell");

		void JourneyBoardStopCellInterfacePage::display( std::ostream& stream, bool isItArrival, const synthese::env::Alarm* alarm
			, bool isItTerminus, const std::string& stopName, bool color
			, const synthese::time::Hour& firstTime, const synthese::time::Hour& lastTime, const Site* site /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back( synthese::util::Conversion::ToString( isItArrival ));
			pv.push_back( alarm->getMessage() );
			pv.push_back( synthese::util::Conversion::ToString( alarm->getLevel() ));
			pv.push_back( synthese::util::Conversion::ToString( isItTerminus ));
			pv.push_back( stopName );
			pv.push_back( synthese::util::Conversion::ToString( color ));
			pv.push_back( firstTime.isUnknown() ? "" : firstTime.toInternalString() );
			pv.push_back( lastTime.isUnknown() ? "" : lastTime.toInternalString() );

			InterfacePage::display( stream, pv, NULL, site );
		}
	}
}