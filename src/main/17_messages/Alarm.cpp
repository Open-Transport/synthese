
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
#include "17_messages/Scenario.h"

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace messages
	{

		Alarm::Alarm()
			: Registrable<uid,Alarm>()
			, _enabled(false)
			, _periodStart(TIME_UNKNOWN)
			, _periodEnd(TIME_UNKNOWN)
			, _scenario(NULL)
		{

		}


		const AlarmLevel& 
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
			if ( ( start < _periodStart ) || ( end > _periodEnd ) || !_enabled )
				return false;
		    
			return true;
		}

		const time::DateTime& Alarm::getPeriodStart() const
		{
			return _periodStart;
		}

		const time::DateTime& Alarm::getPeriodEnd() const
		{
			return _periodEnd;
		}

		bool Alarm::getIsATemplate() const
		{
			return _isATemplate;
		}

		void Alarm::setIsATemplate( bool value )
		{
			_isATemplate = value;
		}

		void Alarm::setLongMessage( const std::string& message )
		{
			_longMessage = message;
		}

		void Alarm::setShortMessage( const std::string& message )
		{
			_shortMessage = message;
		}

		const std::string& Alarm::getLongMessage() const
		{
			return _longMessage;
		}

		const std::string& Alarm::getShortMessage() const
		{
			return _shortMessage;
		}

		void Alarm::setScenario(Scenario* scenario)
		{
			_scenario = scenario;
			if (_scenario != NULL)
			{
				setPeriodStart(_scenario->getPeriodStart());
				setPeriodEnd(_scenario->getPeriodEnd());
			}
		}

		Scenario* Alarm::getScenario() const
		{
			return _scenario;
		}

		bool Alarm::getIsEnabled() const
		{
			return _enabled;
		}

		void Alarm::setIsEnabled( bool value )
		{
			_enabled = value;
		}

		Alarm* Alarm::createCopy(Scenario* scenario) const
		{
			Alarm* alarm = new Alarm;
			alarm->setIsATemplate(false);
			alarm->setLevel(getLevel());
			alarm->setScenario(scenario);
			alarm->setShortMessage(getShortMessage());
			alarm->setLongMessage(getLongMessage());
			
			/// @todo Broadcast list copy

			return alarm;
		}
	}
}
