
#include <string>
#include <vector>

#include "01_util/Conversion.h"

#include "15_env/Environment.h"
#include "15_env/City.h"

#include "36_places_list/CityListInterfaceElement.h"

namespace synthese
{
	using namespace env;
	using namespace util;
	using namespace std;
	using namespace interfaces;

	namespace placeslist
	{
		void CityListInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request) const
		{
			// Parameters
			ParametersVector pv = _parameters.fillParameters(parameters);
			string errorMessage = pv.front();
			string openingText = pv.front();
			string closingText = pv.front();
			int n = Conversion::ToInt(pv.front());
			string inputText = pv.front();
			string emptyLineText = pv.front();

			const Environment::CityList tbCommunes = *((const Environment::CityList*) object);


			if ( tbCommunes[ 1 ] == NULL )
				stream << errorMessage;
			else
			{
				stream << "<script>Nom = new Array; Num = new Array;</script>";
				for ( int i = 1; i <= n; i++ )
					if ( tbCommunes[ i ] != NULL )
					{
						stream
							<< "<script>Nom[" << i << "]=\"" << tbCommunes[ i ] ->getName() << "\";Num[" << i << "]=" << tbCommunes[ i ] ->getId() << ";</script>"
							<< openingText
                            << "<a href=\"javascript:MAJ(" << i << ")\">" << tbCommunes[ i ] ->getName() << "</a>"
							<< closingText;
					}
					else
						stream << emptyLineText;
			}

		}

		void CityListInterfaceElement::parse(const std::string& text )
		{
			_parameters.parse(text);
		}
	}
}
