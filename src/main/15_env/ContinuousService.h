
/** ContinuousService class header.
	@file ContinuousService.h

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

#ifndef SYNTHESE_ENV_CONTINUOUSSERVICE_H
#define SYNTHESE_ENV_CONTINUOUSSERVICE_H

#include "Service.h"
#include <string>

#include "01_util/Registrable.h"


namespace synthese
{
	namespace env
	{

		class Calendar;


		/** Continuous service.


		@ingroup m15
		*/
		class ContinuousService : 
			public synthese::util::Registrable<uid,ContinuousService>, 
			public Service
		{
		private:

			int _range; //!< Continuous service range (minutes).
			int _maxWaitingTime; //!< Max waiting waiting time before next service.
		    

		public:

			ContinuousService (const uid& id,
					int serviceNumber,
					Path* path,
					const synthese::time::Schedule& departureSchedule,
					int range,
					int maxWaitingTime);
			ContinuousService();

			~ContinuousService ();

		    
			//! @name Getters/Setters
			//@{
				int getMaxWaitingTime () const;
				void setMaxWaitingTime (int maxWaitingTime);

				int getRange () const;
				void setRange (int range);

				uid	getId() const;
			//@}

			//! @name Query methods
			//@{
				bool isContinuous () const;
			//@}

		};
	}
}

#endif
