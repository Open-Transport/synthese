
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

	namespace departurestable
	{
		const std::string DeparturesTableDestinationContentInterfaceElement::DESTINATIONS_TO_DISPLAY_ALL = "all";
		const std::string DeparturesTableDestinationContentInterfaceElement::DESTINATIONS_TO_DISPLAY_TERMINUS = "terminus";
		const std::string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION_CITY = "station_city";
		const std::string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION_CITY_IF_NEW = "station_city_if_new";
		const std::string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION = "station";
		const std::string DeparturesTableDestinationContentInterfaceElement::TYPE_CHAR_13 = "char(13)";
		const std::string DeparturesTableDestinationContentInterfaceElement::TYPE_CHAR_26 = "char(26)";

		void DeparturesTableDestinationContentInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 6)
				throw InterfacePageException("Malformed departure table destination cell declaration");

			_destinationsToDisplayVIE = vel.front();
			_displayTerminusVIE = vel.front();
			_displayTypeVIE = vel.front();
			_stopsSeparatorVIE = vel.front();
			_beforeCityVIE = vel.front();
			_afterCityVIE = vel.front();
		}

		string DeparturesTableDestinationContentInterfaceElement::display(ostream& stream, const ParametersVector& parameters, VariablesMap& variables, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			const ArrivalDepartureRow* __DP = (const ArrivalDepartureRow*) object;

			std::string __DestinationsAAfficher = _destinationsToDisplayVIE->getValue(parameters, variables, object, request);
			bool __AfficherTerminus = !_displayTerminusVIE->isZero(parameters, variables, object, request);
			std::string __TypeAffichage = _displayTypeVIE->getValue(parameters, variables, object, request);
			std::string __SeparateurEntreArrets = _stopsSeparatorVIE->getValue(parameters, variables, object, request);
			std::string __AvantCommune = _beforeCityVIE->getValue(parameters, variables, object, request);
			std::string __ApresCommune = _afterCityVIE->getValue(parameters, variables, object, request);

			const City* __DerniereCommune = __DP->second.at(0)->getCity();

			for (int __i = 1; __i < __DP->second.size (); ++__i)
			{
				if ( __DestinationsAAfficher == DESTINATIONS_TO_DISPLAY_ALL && ( __i < __DP->second.size () - 1 || __AfficherTerminus )
					|| __DestinationsAAfficher == DESTINATIONS_TO_DISPLAY_TERMINUS && __i == __DP->second.size () - 1
					|| synthese::util::Conversion::ToInt(__DestinationsAAfficher) == __i && ( __i < __DP->second.size () - 1 || __AfficherTerminus )
				){
					if ( __i > 1 )
						stream << __SeparateurEntreArrets;

					// Affichage de la commune dans les cas necessaire
					if ( __TypeAffichage == TYPE_STATION_CITY
						|| __TypeAffichage == TYPE_STATION_CITY_IF_NEW && __DP->second.at(__i)->getCity() != __DerniereCommune
					){
						std::stringstream ss;
						boost::iostreams::filtering_ostream out;
						out.push (LowerCaseFilter());
						out.push (PlainCharFilter());
						out.push (ss);

						out << __DP->second.at(__i)->getCity () ->getName() << std::flush;
						std::string cityName (ss.str ());
						

						// std::stringMinuscules __TexteMinuscule;
						// __TexteMinuscule << __DP->GetGare( __i ) ->getTown() ->getName();
						if ((cityName.size () > 0) &&
						    (cityName[0] >= 'a') && (cityName[0]  <= 'z'))
						{
						    cityName[0] = cityName[0] - 'a' + 'A';
						}
						stream << __AvantCommune << cityName << __ApresCommune;

						__DerniereCommune = __DP->second.at(__i)->getCity();


					}

					// Affichage du nom d'arret dans les cas ou necessaire
					if ( __TypeAffichage.substr (0, 7) == TYPE_STATION)
						stream << __DP->second.at(__i)->getName();

					// Affichage de la destination 13 caracteres dans les cas ou necessaire
					if ( __TypeAffichage == TYPE_CHAR_13)
						stream << __DP->second.at(__i)->getName13();

					// Affichage de la destination 26 caracteres dans les cas ou necessaire
					if ( __TypeAffichage == TYPE_CHAR_26)
						stream << __DP->second.at(__i)->getName26();
			    }
			}
			return "";
		}
	}
}
