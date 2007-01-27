
/** Alarm class implementation.
	@file Alarm.cpp

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

#include "17_messages/Alarm.h"


namespace synthese
{
	using namespace util;

	namespace messages
	{

		Alarm::Alarm(const uid& id)
			: Registrable<uid,Alarm>(id)
		{

		}

		Alarm::Alarm (const uid& id,
				const std::string& message, 
				const synthese::time::DateTime& periodStart,
				const synthese::time::DateTime& periodEnd,
				const AlarmLevel& level)
			: synthese::util::Registrable<uid,Alarm> (id)
			, _message (message)
			, _periodStart (periodStart)
			, _periodEnd (periodEnd)
			, _level (level)
		{
			_periodStart.updateDateTime( synthese::time::TIME_MIN );
			_periodEnd.updateDateTime( synthese::time::TIME_MAX );
		}
		    



		const std::string& 
		Alarm::getMessage () const
		{
			return _message;
		}


		void 
		Alarm::setMessage( const std::string& message)
		{
			_message = message;
		}



		const Alarm::AlarmLevel& 
		Alarm::getLevel () const
		{
			return _level;
		}



		void 
		Alarm::setLevel (const AlarmLevel& level)
		{
			_level = level;
		}


		void 
		Alarm::setPeriodStart ( const synthese::time::DateTime& periodStart)
		{
			_periodStart = periodStart;
		}


		void 
		Alarm::setPeriodEnd ( const synthese::time::DateTime& periodEnd)
		{
			_periodEnd = periodEnd;
		}




		bool 
		Alarm::isApplicable ( const synthese::time::DateTime& start, 
					const synthese::time::DateTime& end ) const
		{
			if ( _message.empty () )
				return false;

			if ( ( start < _periodStart ) || 
			( end > _periodEnd ) )
				return false;
		    
			return true;
		}
	}
}
