
/** HikingTrail class header.
	@file HikingTrail.h

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

#ifndef SYNTHESE_transportwebsite_HikingTrail_h__
#define SYNTHESE_transportwebsite_HikingTrail_h__

#include "Registrable.h"
#include "Registry.h"
#include "Named.h"

#include <vector>

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace hiking
	{
		/** HikingTrail class.
			@ingroup m56
		*/
		class HikingTrail:
			public virtual util::Registrable,
			public util::Named
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<HikingTrail>	Registry;

			typedef std::vector<env::PublicTransportStopZoneConnectionPlace*> Stops;

		private:
			std::string _map;
			std::string _duration;
			std::string _profile;
			Stops _stops;

		public:
			HikingTrail(util::RegistryKeyType id = UNKNOWN_VALUE);

			const std::string& getMap() const;
			const std::string& getDuration() const;
			const std::string& getProfile() const;
			const Stops& getStops() const;

			void setMap(const std::string& value);
			void setDuration(const std::string& value);
			void setProfile(const std::string& value);
			void clearStops();
			void addStop(env::PublicTransportStopZoneConnectionPlace* value, size_t rank);
			void removeStop(size_t rank);
		};
	}
}

#endif // SYNTHESE_transportwebsite_HikingTrail_h__
