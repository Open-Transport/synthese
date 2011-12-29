
/** DeparturesTableDestinationContentInterfaceElement class implementation.
	@file DeparturesTableDestinationContentInterfaceElement.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ValueElementList.h"
#include "InterfacePageException.h"
#include "City.h"
#include "StopArea.hpp"
#include "DeparturesTableTypes.h"
#include "DeparturesTableDestinationContentInterfaceElement.h"
#include "DepartureTableRowInterfacePage.h"
#include "NamedPlace.h"
#include "DeparturesTableTransferDestinationInterfacePage.h"
#include "DeparturesTableDestinationInterfacepage.h"
#include "Interface.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace pt;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::LibraryInterfaceElement, departure_boards::DeparturesTableDestinationContentInterfaceElement>::FACTORY_KEY("departurestabledestinationcell");
	}

	namespace departure_boards
	{
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION_CITY = "station_city";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION_CITY_IF_NEW = "station_city_if_new";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_STATION = "station";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_CHAR_13 = "char(13)";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_CHAR_26 = "char(26)";
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_CHAR_26_OR_STATION_CITY_IF_NEW("char(26)/station_city_if_new");
		const string DeparturesTableDestinationContentInterfaceElement::TYPE_USE_INTERFACE("use_interface");

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
		}

		string DeparturesTableDestinationContentInterfaceElement::display(ostream& stream, const ParametersVector& parameters, VariablesMap& variables, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			const ArrivalDepartureRow* __DP = (const ArrivalDepartureRow*) object;

			int firstIntermediatesStops(lexical_cast<int>(_firstIntermediatesStopsToDisplayVIE->getValue(parameters, variables, object, request)));
			int numberOfIntermediatesStops(lexical_cast<int>(_numberOfIntermediatesStopsToDisplayVIE->getValue(parameters, variables, object, request)));
			bool __AfficherTerminus = !_displayTerminusVIE->isFalse(parameters, variables, object, request);
			string __TypeAffichage = _displayTypeVIE->getValue(parameters, variables, object, request);
			string __SeparateurEntreArrets = _stopsSeparatorVIE->getValue(parameters, variables, object, request);
	//		string displayCity(_displayCityVIE->getValue(parameters, variables, object, request));
			string __AvantCommune = _beforeCityVIE->getValue(parameters, variables, object, request);
			string __ApresCommune = _afterCityVIE->getValue(parameters, variables, object, request);

			const City* __DerniereCommune = dynamic_cast<const NamedPlace*>(__DP->second.at(0).place)->getCity();
			size_t totalTransferRank(0);

			int terminusRank(__DP->second.size() - 1);
			size_t rank(0);
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
				const StopArea* place(__DP->second.at(i).place);

				if (i > firstIntermediatesStops)
					stream << __SeparateurEntreArrets;

				if(__TypeAffichage == TYPE_USE_INTERFACE)
				{
					const DeparturesTableDestinationInterfacepage* destinationPage(_page->getInterface()->getPage<DeparturesTableDestinationInterfacepage>());
					stringstream transferString;
					try
					{
						const DeparturesTableTransferDestinationInterfacePage* transferPage(_page->getInterface()->getPage<DeparturesTableTransferDestinationInterfacePage>());

						// Loop on the transfer pages
						size_t localTransferRank(0);
						BOOST_FOREACH(const IntermediateStop::TransferDestinations::value_type& transferServiceUse, __DP->second.at(i).transferDestinations)
						{
							transferPage->display(transferString, transferServiceUse, localTransferRank++, totalTransferRank++, variables, request);
						}
					}
					catch(InterfacePageException&)
					{
					}

					destinationPage->display(
						stream,
						__DP->second.at(i).serviceUse,
						place->getCity() == __DerniereCommune,
						i == terminusRank,
						transferString.str(),
						rank++,
						variables,
						request
					);

					continue;
				}

				// Display of the city name if necessary
				if(	__TypeAffichage == TYPE_STATION_CITY
					||(	(	__TypeAffichage == TYPE_STATION_CITY_IF_NEW
							|| __TypeAffichage == TYPE_CHAR_26_OR_STATION_CITY_IF_NEW
						)&&	place->getName26().empty()
						&& place->getCity() != __DerniereCommune
					)
				){
					string cityName(place->getCity()->getName());
					if(cityName.size() > 1)
					{
						cityName =
							to_upper_copy(cityName.substr(0, 1)) +
							to_lower_copy(cityName.substr(1))
						;
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
