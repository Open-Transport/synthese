
#include "CityListInterfaceElement.h"
#include "15_env/Environment.h"
#include "15_env/City.h"
#include "01_util/Conversion.h"
#include "11_interfaces/Site.h"
#include <string>
#include <vector>

namespace synthese
{
	using namespace env;
	using namespace util;
	using namespace std;

	namespace interfaces
	{
		const std::string CityListInterfaceElement::_factory_key = Factory<LibraryInterfaceElement>::integrate<CityListInterfaceElement>("city_list");

		void CityListInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const Site* site /*= NULL*/ ) const
		{
			// Parameters
			ParametersVector pv = _parameters.fillParameters(parameters);
			string errorMessage = pv.front();
			string openingText = pv.front();
			string closingText = pv.front();
			int n = Conversion::ToInt(pv.front());
			string inputText = pv.front();
			string emptyLineText = pv.front();

			const env::Environment* __Environnement = site->getEnvironment();
			
			vector<const City*> tbCommunes = __Environnement->guessCity(inputText, n );


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
