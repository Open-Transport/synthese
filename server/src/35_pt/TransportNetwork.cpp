
/** TransportNetwork class implementation.
	@file TransportNetwork.cpp

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

#include "TransportNetwork.h"

#include "CalendarTemplateTableSync.h"
#include "CommercialLineTableSync.h"
#include "DataSourceLinksField.hpp"
#include "DBConstants.h"
#include "ImportableTableSync.hpp"
#include "ParametersMap.h"
#include "TransportNetworkTableSync.h"
#include "TreeFolder.hpp"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"

using namespace std;

namespace synthese
{
	using namespace calendar;
	using namespace db;
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace tree;
	using namespace util;

	CLASS_DEFINITION(TransportNetwork, "t022_transport_networks", 22)
	FIELD_DEFINITION_OF_OBJECT(TransportNetwork, "transport_network_id", "transport_network_ids")

	FIELD_DEFINITION_OF_TYPE(DataSourceLinksWithoutUnderscore, "creator_id", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DaysCalendarParent, "days_calendars_parent_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(PeriodsCalendarParent, "periods_calendars_parent_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Image, "image", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Timezone, "timezone", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Lang, "lang", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Contact, "contact_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(FareContact, "fare_contact_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CountryCode, "country_code", SQL_TEXT)

	namespace pt
	{
		TransportNetwork::TransportNetwork(
			util::RegistryKeyType id,
			std::string name,
			std::string image,
			std::string country_code
		):	util::Registrable(id),
			Object<TransportNetwork, TransportNetworkSchema> (
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(Name, name),
					FIELD_DEFAULT_CONSTRUCTOR(DataSourceLinksWithoutUnderscore),
					FIELD_DEFAULT_CONSTRUCTOR(DaysCalendarParent),
					FIELD_DEFAULT_CONSTRUCTOR(PeriodsCalendarParent),
					FIELD_VALUE_CONSTRUCTOR(Image, image),
					FIELD_DEFAULT_CONSTRUCTOR(Timezone),
					FIELD_DEFAULT_CONSTRUCTOR(Lang),
					FIELD_DEFAULT_CONSTRUCTOR(Contact),
					FIELD_DEFAULT_CONSTRUCTOR(FareContact),
					FIELD_VALUE_CONSTRUCTOR(CountryCode, country_code)
			)	),
			graph::PathClass()
		{}



		TransportNetwork::~TransportNetwork()
		{}



		PathClass::Identifier TransportNetwork::getIdentifier() const
		{
			return getKey();
		}
}	}
