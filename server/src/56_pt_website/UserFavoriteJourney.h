
/** UserFavoriteJourney class header.
	@file UserFavoriteJourney.h

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

#ifndef SYNTHESE_routeplanner_UserFavoriteJourney_h__
#define SYNTHESE_routeplanner_UserFavoriteJourney_h__

#include <string>

#include "Object.hpp"

#include "AccessParameters.h"
#include "NumericField.hpp"
#include "StringField.hpp"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace pt_website
	{
		FIELD_POINTER(FavoriteJourneyUser, security::User)
		FIELD_STRING(OriginCityName)
		FIELD_STRING(OriginPlaceName)
		FIELD_STRING(DestinationCityName)
		FIELD_STRING(DestinationPlaceName)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(FavoriteJourneyUser),
			FIELD(Rank),
			FIELD(OriginCityName),
			FIELD(OriginPlaceName),
			FIELD(DestinationCityName),
			FIELD(DestinationPlaceName)
		> UserFavoriteJourneySchema;

		/** UserFavoriteJourney class.
			@ingroup m56
		*/
		class UserFavoriteJourney
		:	public Object<UserFavoriteJourney, UserFavoriteJourneySchema>
		{
		private:
			graph::AccessParameters	_accessParameters;

		public:
			UserFavoriteJourney(
				util::RegistryKeyType id = 0
			);

			//! @name Setters
			//@{
				void setRank(boost::optional<std::size_t> value) { set<Rank>(value ? *value : 0); }
				void setUser(security::User* value);
				void setOriginCityName(const std::string& value);
				void setOriginPlaceName(const std::string& value);
				void setDestinationCityName(const std::string& value);
				void setDestinationPlaceName(const std::string& value);
				void setAccessParameters(const graph::AccessParameters& value);
			//@}

			//! @name Getters
			//@{
				boost::optional<std::size_t>	getRank()					const;
				const security::User*			getUser()					const;
				const std::string&				getOriginCityName()			const;
				const std::string&				getOriginPlaceName()		const;
				const std::string&				getDestinationCityName()	const;
				const std::string&				getDestinationPlaceName()	const;
				const graph::AccessParameters&	getAccessParameters()		const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_routeplanner_UserFavoriteJourney_h__
