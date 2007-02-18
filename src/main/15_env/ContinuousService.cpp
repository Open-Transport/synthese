
/** ContinuousService class implementation.
	@file ContinuousService.cpp

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

#include "ContinuousService.h"

namespace synthese
{
	namespace env
	{
		ContinuousService::ContinuousService (const uid& id,
							int serviceNumber,
							Path* path,
							const synthese::time::Schedule& departureSchedule,
							int range,
							int maxWaitingTime)
			: synthese::util::Registrable<uid,ContinuousService> (id)
			, Service (serviceNumber, path, departureSchedule)
			, _range (range)
			, _maxWaitingTime (maxWaitingTime)
		{

		}

		ContinuousService::ContinuousService()
			: synthese::util::Registrable<uid,ContinuousService> ()
			, Service()

		{

		}
		    


		ContinuousService::~ContinuousService ()
		{
		}




		    
		int 
		ContinuousService::getMaxWaitingTime () const
		{
			return _maxWaitingTime;
		}




		void 
		ContinuousService::setMaxWaitingTime (int maxWaitingTime)
		{
			_maxWaitingTime = maxWaitingTime;
		}




		int 
		ContinuousService::getRange () const
		{
			return _range;
		}

		void 
		ContinuousService::setRange (int range)
		{
			_range = range;
		}



		bool 
		ContinuousService::isContinuous () const
		{
			return true;
		}

		uid ContinuousService::getId() const
		{
			return getKey();
		}

	}
}
