
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
		void CityListInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, boost::shared_ptr<const void> object /*= NULL*/, const server::Request* request) const
		{
			// Parameters
			string errorMessage = _errorMessage->getValue(parameters, object, request);
			string openingText = _openingText->getValue(parameters, object, request);
			string closingText = _closingText->getValue(parameters, object, request);
			int n = Conversion::ToInt(_n->getValue(parameters, object, request));
			string inputText = _inputText->getValue(parameters, object, request);
			string emptyLineText = _emptyLineText->getValue(parameters, object, request);

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
							<< "<script>Nom[" << i << "]=\"" << tbCommunes[ i ] ->getName() << "\";Num[" << i << "]=" << tbCommunes[ i ] ->getKey() << ";</script>"
							<< openingText
                            << "<a href=\"javascript:MAJ(" << i << ")\">" << tbCommunes[ i ] ->getName() << "</a>"
							<< closingText;
					}
					else
						stream << emptyLineText;
			}

		}

		void CityListInterfaceElement::storeParameters( interfaces::ValueElementList& pv)
		{
			_errorMessage = pv.front();
			_openingText = pv.front();
			_closingText = pv.front();
			_n = pv.front();
			_inputText = pv.front();
			_emptyLineText = pv.front();
		}
	}
}

