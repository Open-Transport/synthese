
/** DesignatedLinePhysicalStop class header.
	@file DesignatedLinePhysicalStop.hpp

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
		private:
			bool	_scheduleInput;
			bool	_reservationNeeded;

		public:
			DesignatedLinePhysicalStop(
				util::RegistryKeyType id = 0,
				JourneyPattern* line = NULL,
				std::size_t rankInPath = 0,
				bool isDeparture = true,
				bool isArrival = true,
				double metricOffset = 0,
				StopPoint* stop = NULL,
				bool scheduleInput = true,
				bool reservationNeeded = true
			);

			virtual bool getScheduleInput()	const { return _scheduleInput; }
			virtual bool getReservationNeeded() const { return _reservationNeeded; }

			void setScheduleInput(bool value) { _scheduleInput = value; }
			void setReservationNeeded(bool value) { _reservationNeeded = value; }

			virtual void toParametersMap(
				util::ParametersMap& pm,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()
			) const;
		};
	}
}

#endif // SYNTHESE_pt_DesignatedLinePhysicalStop_hpp__
