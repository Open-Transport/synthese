
/** DeparturesTableDestinationContentInterfaceElement class implementation.
	@file DeparturesTableDestinationContentInterfaceElement.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <vector>
#include <sstream>
#include <boost/iostreams/filtering_stream.hpp>

#include "01_util/LowerCaseFilter.h"
#include "01_util/PlainCharFilter.h"


#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

#include "15_env/LineMarkerInterfacePage.h"
#include "15_env/City.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"

#include "34_departures_table/Types.h"
#include "34_departures_table/DeparturesTableDestinationContentInterfaceElement.h"
#include "34_departures_table/DepartureTableRowInterfacePage.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::LibraryInterfaceElement, departurestable::DeparturesTableDestinationContentInterfaceElement>::FACTORY_KEY("departurestabledestinationcell");
	}

	namespace departurestable
	{
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION_CITY = "station_city";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION_CITY_IF_NEW = "station_city_if_new";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION = "station";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_CHAR_13 = "char(13)";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_CHAR_26 = "char(26)";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_CHAR_26_OR_STATION_CITY_IF_NEW("char(26)/station_city_if_new");

		void DeparturesTableDestinationContentInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 7)
				throw InterfacePageException("Malformed departure table destination cell declaration");

			_firstIntermediatesStopsToDisplayVIE = vel.front();
			_numberOfIntermediatesStopsToDisplayVIE = vel.front();
			_displayTerminusVIE = vel.front();
			_displayTypeVIE = vel.front();
			_stopsSeparatorVIE = vel.front();
			_beforeCityVIE = vel.front();
			_afterCityVIE = vel.front();
			
			//	_displayCityVIE = vel.front();
		}

		string DeparturesTableDestinationContentInterfaceElement::display(ostream& stream, const ParametersVector& parameters, VariablesMap& variables, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			const ArrivalDepartureRow* __DP = (const ArrivalDepartureRow*) object;

			int firstIntermediatesStops(Conversion::ToInt(_firstIntermediatesStopsToDisplayVIE->getValue(parameters, variables, object, request)));
			int numberOfIntermediatesStops(Conversion::ToInt(_numberOfIntermediatesStopsToDisplayVIE->getValue(parameters, variables, object, request)));
			bool __AfficherTerminus = !_displayTerminusVIE->isFalse(parameters, variables, object, request);
			string __TypeAffichage = _displayTypeVIE->getValue(parameters, variables, object, request);
			string __SeparateurEntreArrets = _stopsSeparatorVIE->getValue(parameters, variables, object, request);
	//		string displayCity(_displayCityVIE->getValue(parameters, variables, object, request));
			string __AvantCommune = _beforeCityVIE->getValue(parameters, variables, object, request);
			string __ApresCommune = _afterCityVIE->getValue(parameters, variables, object, request);
			
			const City* __DerniereCommune = __DP->second.at(0)->getCity();

			int terminusRank(__DP->second.size() - 1);
			for(int i((numberOfIntermediatesStops != 0) ? firstIntermediatesStops : terminusRank);
				i != (__AfficherTerminus ? __DP->second.size() : terminusRank);
				++i
			){
				// Max number of destinations
				if(	numberOfIntermediatesStops != UNKNOWN_VALUE
					&& i - firstIntermediatesStops >= numberOfIntermediatesStops
					&& i < terminusRank
				)	continue;

				// Place
				const PublicTransportStopZoneConnectionPlace* place(__DP->second.at(i));

				if (i > firstIntermediatesStops)
					stream << __SeparateurEntreArrets;

				// Affichage de la commune dans les cas necessaire
				if(	__TypeAffichage == TYPE_STATION_CITY
					||(	__TypeAffichage == TYPE_STATION_CITY_IF_NEW
						||(	__TypeAffichage == TYPE_CHAR_26_OR_STATION_CITY_IF_NEW 
							&& place->getName26().empty()
						) && place->getCity() != __DerniereCommune
					)
				){
					stringstream ss;
					boost::iostreams::filtering_ostream out;
					out.push (LowerCaseFilter());
					out.push (PlainCharFilter());
					out.push (ss);

					out << place->getCity () ->getName() << flush;
					string cityName (ss.str ());
					

					// stringMinuscules __TexteMinuscule;
					// __TexteMinuscule << __DP->GetGare( __i ) ->getTown() ->getName();
					if ((cityName.size () > 0) &&
					    (cityName[0] >= 'a') && (cityName[0]  <= 'z'))
					{
					    cityName[0] = cityName[0] - 'a' + 'A';
					}
					stream << __AvantCommune << cityName << __ApresCommune;

					__DerniereCommune = place->getCity();


				}

				// Affichage du nom d'arret dans les cas ou necessaire
				if ( __TypeAffichage.substr (0, 7) == TYPE_STATION)
					stream << place->getName();

				// Affichage de la destination 13 caracteres dans les cas ou necessaire
				if ( __TypeAffichage == TYPE_CHAR_13)
					stream << place->getName13OrName();

				// Affichage de la destination 26 caracteres dans les cas ou necessaire
				if( __TypeAffichage == TYPE_CHAR_26
				||	__TypeAffichage == TYPE_CHAR_26_OR_STATION_CITY_IF_NEW					
				)	stream << place->getName26OrName();
			}
			return string();
		}
	}
}
