
/** DesignatedLinePhysicalStop class header.
	@file DesignatedLinePhysicalStop.hpp

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

#ifndef SYNTHESE_pt_DesignatedLinePhysicalStop_hpp__
#define SYNTHESE_pt_DesignatedLinePhysicalStop_hpp__

#include "LinePhysicalStop.hpp"

namespace synthese
{
	namespace pt
	{
		/** DesignatedLinePhysicalStop class.
			@ingroup m35
		*/
		class DesignatedLinePhysicalStop:
			public LinePhysicalStop
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<DesignatedLinePhysicalStop> Registry;

		private:
			bool	_scheduleInput;

		public:
			DesignatedLinePhysicalStop(
				util::RegistryKeyType id = 0,
				JourneyPattern* line = NULL,
				std::size_t rankInPath = 0,
				bool isDeparture = true,
				bool isArrival = true,
				double metricOffset = 0,
				StopPoint* stop = NULL,
				bool scheduleInput = true
			);

			virtual bool getScheduleInput()	const { return _scheduleInput; }

			void setScheduleInput(bool value) { _scheduleInput = value; }
		};
	}
}

#endif // SYNTHESE_pt_DesignatedLinePhysicalStop_hpp__
