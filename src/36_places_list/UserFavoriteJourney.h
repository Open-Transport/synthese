
/** UserFavoriteJourney class header.
	@file UserFavoriteJourney.h

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

	namespace transportwebsite
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
			int						_rank;
			std::string				_originCityName;
			std::string				_originPlaceName;
			std::string				_destinationCityName;
			std::string				_destinationPlaceName;
			graph::AccessParameters	_accessParameters;

		public:
			UserFavoriteJourney(
				util::RegistryKeyType id = UNKNOWN_VALUE
			);

			void setRank(int value);
			void setUser(const security::User* value);
			void setOriginCityName(const std::string& value);
			void setOriginPlaceName(const std::string& value);
			void setDestinationCityName(const std::string& value);
			void setDestinationPlaceName(const std::string& value);
			void setAccessParameters(const graph::AccessParameters& value);

			int								getRank()					const;
			const security::User*			getUser()					const;
			const std::string&				getOriginCityName()			const;
			const std::string&				getOriginPlaceName()		const;
			const std::string&				getDestinationCityName()	const;
			const std::string&				getDestinationPlaceName()	const;
			const graph::AccessParameters&	getAccessParameters()		const;

		};
	}
}

#endif // SYNTHESE_routeplanner_UserFavoriteJourney_h__
