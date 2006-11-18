
#include "RoutePlannerSheetLineInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		void RoutePlannerSheetLineInterfacePage::display( std::ostream& stream, const std::string& text, bool alternateColor
			, const synthese::env::ConnectionPlace* place, const Site* site /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back(text);
			pv.push_back( synthese::util::Conversion::ToString( alternateColor ));

			InterfacePage::display( stream, pv, place, site );
		}
	}
}