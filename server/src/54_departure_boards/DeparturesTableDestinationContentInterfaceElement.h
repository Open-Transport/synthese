
/** DeparturesTableDestinationContentInterfaceElement class header.
	@file DeparturesTableDestinationContentInterfaceElement.h

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

#ifndef SYNTHESE_DeparturesTableDestinationContentInterfaceElement_H__
#define SYNTHESE_DeparturesTableDestinationContentInterfaceElement_H__


#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

#include <string>

namespace synthese
{
	namespace departure_boards
	{
		/** Departure Table Destination Content Interface Element Class.
			@ingroup m54Library refLibrary

			Parameters :
				- 0 : Rank of first intermediate stop to display (1 = first intermediate stop)
				- 1 : Max number of intermediates stops to display (0 = do not display intermediates stops)
				- 2 : Display terminus name (boolean)
				- 3 : Name to display (see also 4)
					- char(13) : 13 chars name if exists, connection place name truncated else
					- char(26) : 26 chars name if exists, connection place name truncated else
					- else : connection place name
				- 4 : Display city name
					- station_city : adds the city name before the connection place name
					- station_city_if_new : adds the city name before the connection place name only at city changes
					- else : do not add the city name before the connection place name
				- 4 : stopsSeparator
				- 5 : beforeCity
				- 6 : afterCity
				- 7 : beforeWholwStopName
				- 8 : afterWholeStopName

			Object : ArrivalDepartureRow object

		*/
		class DeparturesTableDestinationContentInterfaceElement : public util::FactorableTemplate<interfaces::LibraryInterfaceElement, DeparturesTableDestinationContentInterfaceElement>
		{
		public:
			static const std::string TYPE_STATION_CITY;
			static const std::string TYPE_STATION_CITY_IF_NEW;
			static const std::string TYPE_STATION;
			static const std::string TYPE_CHAR_13;
			static const std::string TYPE_CHAR_26;
			static const std::string TYPE_CHAR_26_OR_STATION_CITY_IF_NEW;
			static const std::string TYPE_USE_INTERFACE;


		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _firstIntermediatesStopsToDisplayVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _numberOfIntermediatesStopsToDisplayVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayTerminusVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayTypeVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _stopsSeparatorVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _displayCityVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _beforeCityVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _afterCityVIE;

			void displayDestination(std::ostream& stream, bool displayCity) const;

		public:
			void storeParameters(interfaces::ValueElementList& vel);
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL, const server::Request* request = NULL) const;
			const std::string getLabel(const interfaces::ParametersVector& parameters) const;
		};

	}
}

#endif // SYNTHESE_DeparturesTableDestinationContentInterfaceElement_H__
