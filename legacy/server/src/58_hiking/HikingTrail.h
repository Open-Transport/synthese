
/** HikingTrail class header.
	@file HikingTrail.h

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

#ifndef SYNTHESE_transportwebsite_HikingTrail_h__
#define SYNTHESE_transportwebsite_HikingTrail_h__

#include "Registrable.h"
#include "Registry.h"

#include <vector>

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace hiking
	{
		/** Hiking trail.
			@ingroup m56
		*/
		class HikingTrail:
			public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<HikingTrail>	Registry;

			typedef std::vector<pt::StopArea*> Stops;

		private:
			std::string _map;
			std::string _duration;
			std::string _profile;
			std::string _url;
			Stops _stops;
			std::string _name;

		public:
			HikingTrail(util::RegistryKeyType id = 0);

			//! @name Getters
			//@{
				const std::string& getMap() const { return _map; }
				const std::string& getDuration() const { return _duration; }
				const std::string& getProfile() const { return _profile; }
				const std::string& getURL() const { return _url; }
				const Stops& getStops() const { return _stops; }
				virtual std::string getName() const { return _name; }
			//@}

			//! @name Setters
			//@{
				void setMap(const std::string& value){ _map = value; }
				void setDuration(const std::string& value){ _duration = value; }
				void setProfile(const std::string& value){ _profile = value; }
				void setURL(const std::string& value){ _url = value; }
				void setStops(const Stops& value){ _stops = value; }
				void setName(const std::string& value){ _name = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_transportwebsite_HikingTrail_h__
