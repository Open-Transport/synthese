
/** NonPermanentService class header.
	@file NonPermanentService.h

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

#ifndef SYNTHESE_env_NonPermanentService_h__
#define SYNTHESE_env_NonPermanentService_h__

#include "15_env/Service.h"
#include "15_env/Calendar.h"

namespace synthese
{
	namespace env
	{
		/** NonPermanentService class.
			@ingroup m15
		*/
		class NonPermanentService : public Service
		{
		private:
			Calendar		_calendar;  //!< Which days is this service available ?

		public:
			NonPermanentService();
			NonPermanentService(
				int serviceNumber
				, Path* path
			);


			Calendar&		getCalendar (); // MJ constness pb
			const Calendar&		getCalendar () const; // MJ constness pb

			virtual bool isProvided(const time::Date& originDate) const;

			/** Latest schedule of the service : the last arrival at the last vertex.
			@return The latest schedule of the service
			*/
			virtual const time::Schedule& getLastArrivalSchedule() const = 0;

		};
	}
}

#endif // SYNTHESE_env_NonPermanentService_h__
