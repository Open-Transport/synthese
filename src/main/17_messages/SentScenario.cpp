
/** SentScenario class implementation.
	@file SentScenario.cpp

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

#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioTemplate.h"

namespace synthese
{
	using namespace time;

	namespace messages
	{
		SentScenario::SentScenario()
			: Registrable<uid, SentScenario>()
			, ScenarioSubclassTemplate<ScenarioSentAlarm>()
			, _isEnabled(false)
			, _periodStart(TIME_UNKNOWN)
			, _periodEnd(TIME_UNKNOWN)
		{
		}

		SentScenario::SentScenario( const ScenarioTemplate& source , time::DateTime periodStart /*= time::DateTime(time::TIME_UNKNOWN) */, time::DateTime periodEnd /*= time::DateTime(time::TIME_UNKNOWN) */ )
			: Registrable<uid, SentScenario>()
			, ScenarioSubclassTemplate<ScenarioSentAlarm>(source.getName())
			, _isEnabled(false)
			, _periodStart(periodStart)
			, _periodEnd(periodEnd)
		{
			// Creation of the SentAlarm from the AlarmTemplate
		}

		void SentScenario::setPeriodStart( const synthese::time::DateTime& periodStart )
		{
			_periodStart = periodStart;
		}

		void SentScenario::setPeriodEnd( const synthese::time::DateTime& periodEnd )
		{
			_periodEnd = periodEnd;
		}


		const time::DateTime& SentScenario::getPeriodStart() const
		{
			return _periodStart;
		}

		const time::DateTime& SentScenario::getPeriodEnd() const
		{
			return _periodEnd;
		}

		void SentScenario::setIsEnabled( bool value )
		{
			_isEnabled = value;
		}

		bool SentScenario::getIsEnabled() const
		{
			return _isEnabled;
		}

		SentScenario::~SentScenario()
		{

		}

		uid SentScenario::getId() const
		{
			return getKey();
		}
	}
}
