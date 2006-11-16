
#include "JourneyBoardJunctionCellInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		const bool JourneyBoardJunctionCellInterfacePage::_registered = Factory<InterfacePage>::integrate<JourneyBoardJunctionCellInterfacePage>("journey_board_junction_cell");


		void JourneyBoardJunctionCellInterfacePage::display( std::ostream& stream , const synthese::env::ConnectionPlace* place
			, const synthese::env::Alarm* alarm, bool color, const Site* site /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back(synthese::util::Conversion::ToString(place->getKey()));
			pv.push_back(alarm == NULL ? "" : alarm->getMessage());
			pv.push_back(alarm == NULL ? "" : synthese::util::Conversion::ToString(alarm->getMessage()));
			pv.push_back(synthese::util::Conversion::ToString(color));

			InterfacePage::display(stream, pv, NULL, site);

		}
	}
}