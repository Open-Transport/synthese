
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

#include "AccessParameters.h"
#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace pt_website
	{
		/** UserFavoriteJourney class.
			@ingroup m56
		*/
		class UserFavoriteJourney
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<UserFavoriteJourney>	Registry;

		private:
			const security::User*	_user;
			boost::optional<std::size_t>	_rank;
			std::string				_originCityName;
			std::string				_originPlaceName;
			std::string				_destinationCityName;
			std::string				_destinationPlaceName;
			graph::AccessParameters	_accessParameters;

		public:
			UserFavoriteJourney(
				util::RegistryKeyType id = 0
			);

			//! @name Setters
			//@{
			void setRank(boost::optional<std::size_t> value) { _rank = value; }
				void setUser(const security::User* value) { _user = value; }
				void setOriginCityName(const std::string& value);
				void setOriginPlaceName(const std::string& value);
				void setDestinationCityName(const std::string& value);
				void setDestinationPlaceName(const std::string& value);
				void setAccessParameters(const graph::AccessParameters& value);
			//@}

			//! @name Getters
			//@{
				boost::optional<std::size_t>	getRank()					const { return _rank; }
				const security::User*			getUser()					const { return _user; }
				const std::string&				getOriginCityName()			const;
				const std::string&				getOriginPlaceName()		const;
				const std::string&				getDestinationCityName()	const;
				const std::string&				getDestinationPlaceName()	const;
				const graph::AccessParameters&	getAccessParameters()		const;
			//@}
		};
	}
}

#endif // SYNTHESE_routeplanner_UserFavoriteJourney_h__
