
/** TransportNetwork class header.
	@file TransportNetwork.h

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

#ifndef SYNTHESE_ENV_TRANSPORTNETWORK_H
#define SYNTHESE_ENV_TRANSPORTNETWORK_H

#include "Object.hpp"

#include "CalendarTemplate.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTemplate.hpp"
#include "PathClass.h"
#include "Registry.h"
#include "ReservationContact.h"
#include "TreeFolderRoot.hpp"

namespace synthese
{
	namespace pt
	{
		FIELD_DATASOURCE_LINKS(DataSourceLinksWithoutUnderscore)
		FIELD_POINTER(DaysCalendarParent, calendar::CalendarTemplate)
		FIELD_POINTER(PeriodsCalendarParent, calendar::CalendarTemplate)
		FIELD_STRING(Image)
		FIELD_STRING(Timezone)
		FIELD_STRING(Lang)
		FIELD_POINTER(Contact, pt::ReservationContact)
		FIELD_POINTER(FareContact, pt::ReservationContact)
		FIELD_STRING(CountryCode)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(DataSourceLinksWithoutUnderscore),
			FIELD(DaysCalendarParent),
			FIELD(PeriodsCalendarParent),
			FIELD(Image),
			FIELD(Timezone),
			FIELD(Lang),
			FIELD(Contact),
			FIELD(FareContact),
			FIELD(CountryCode)
		> TransportNetworkSchema;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Transport network class.
		/// TRIDENT OK
		///	@ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class TransportNetwork:
			public virtual Object<TransportNetwork, TransportNetworkSchema>,
			public graph::PathClass,
			public impex::ImportableTemplate<TransportNetwork>,
			public tree::TreeFolderRoot
		{
		public:

			TransportNetwork (
				util::RegistryKeyType id = 0,
				std::string name = std::string(),
				std::string image = std::string(),
				std::string country_code = std::string()
			);
			~TransportNetwork ();

			//! @name Services
			//@{
				virtual std::string getName() const { return get<Name>(); }

				virtual std::string getRuleUserName() const { return "Réseau " + getName(); }

				virtual PathClass::Identifier getIdentifier() const;
			//@}
		};
	}
}

#endif
