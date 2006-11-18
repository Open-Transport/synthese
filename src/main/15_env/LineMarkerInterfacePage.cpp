
#include "LineMarkerInterfacePage.h"
#include "01_util/Conversion.h"

namespace synthese
{
	using namespace util;

	namespace interfaces
	{
		void LineMarkerInterfacePage::display( std::ostream& stream, const std::string& tableOpeningHTML , const std::string& tableClosingHTML , int pixelWidth , int pixelHeight , const synthese::env::Line* line , const Site* site /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back(tableOpeningHTML);
			pv.push_back(tableClosingHTML);
			pv.push_back(Conversion::ToString(pixelWidth));
			pv.push_back(Conversion::ToString(pixelHeight));
			InterfacePage::display(stream, pv, (const void*) line, site);
		}
	}
}