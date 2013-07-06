
/** DRTArea class header.
	@file DRTArea.hpp

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

#ifndef SYNTHESE_pt_DRTArea_hpp__
#define SYNTHESE_pt_DRTArea_hpp__

#include "Registrable.h"
#include "Registry.h"
#include "Vertex.h"
#include <set>

namespace synthese
{
	namespace pt
	{
		class StopArea;

		/** DRTArea class.
			@ingroup m35
		*/
		class DRTArea:
			virtual public util::Registrable,
			public graph::Vertex
		{
		public:
			typedef std::set<StopArea*> Stops;

			/// Chosen registry class.
			typedef util::Registry<DRTArea> Registry;

		private:
			Stops _stops;
			std::string _name;

		public:
			DRTArea(
				const util::RegistryKeyType id = 0,
				std::string name = std::string(),
				Stops stops = Stops()
			);

			void setStops(const Stops& value){ _stops = value; }
			void setName(const std::string& value){ _name = value; }

			const Stops& getStops() const { return _stops; }
			virtual std::string getName() const { return _name; }

			virtual graph::GraphIdType getGraphType() const;

			virtual std::string getRuleUserName() const;

			bool contains(const StopArea& stopArea) const;
		};
}	}

#endif // SYNTHESE_pt_DRTArea_hpp__
