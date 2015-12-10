
/** PublicBikeNetwork class header.
	@file PublicBikeNetwork.h

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

#ifndef SYNTHESE_public_biking_PUBLICBIKENETWORK_H
#define SYNTHESE_public_biking_PUBLICBIKENETWORK_H

#include "Object.hpp"

#include "DataSourceLinksField.hpp"
#include "ImportableTemplate.hpp"

namespace synthese
{
	namespace public_biking
	{
		FIELD_DATASOURCE_LINKS(PublicBikeNetworkDataSourceLinks)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(PublicBikeNetworkDataSourceLinks)
		> PublicBikeNetworkSchema;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace public_biking
	{
		//////////////////////////////////////////////////////////////////////////
		/// Public Bike network class.
		///	@ingroup m65
		//////////////////////////////////////////////////////////////////////////
		class PublicBikeNetwork:
			public virtual Object<PublicBikeNetwork, PublicBikeNetworkSchema>,
			public impex::ImportableTemplate<PublicBikeNetwork>
		{
		public:

			PublicBikeNetwork (
				util::RegistryKeyType id = 0
			);
			~PublicBikeNetwork ();

			//! @name Services
			//@{
				virtual std::string getName() const { return get<Name>(); }
				void setName(const std::string value) { set<Name>(value); }
			//@}
		};
	}
}

#endif
