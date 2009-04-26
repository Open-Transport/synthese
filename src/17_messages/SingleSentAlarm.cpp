
/** SingleSentAlarm class implementation.
	@file SingleSentAlarm.cpp

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

#include "SingleSentAlarm.h"

namespace synthese
{
	using namespace time;

	namespace messages
	{
		SingleSentAlarm::SingleSentAlarm(util::RegistryKeyType key)
		:	Registrable(key),
			SentAlarm(key),
			_enabled(false)
			, _periodStart(DateTime(TIME_UNKNOWN))
			, _periodEnd(DateTime(TIME_UNKNOWN))
		{

		}

		SingleSentAlarm::SingleSentAlarm( const SingleSentAlarm& source)
		:	Registrable(UNKNOWN_VALUE),
			SentAlarm(),
			_enabled(false),
			_periodStart(DateTime(TIME_UNKNOWN)),
			_periodEnd(DateTime(TIME_UNKNOWN))
		{			
			setLevel(source.getLevel());
			setShortMessage(source.getShortMessage());
			setLongMessage(source.getLongMessage());
		}

		void SingleSentAlarm::setIsEnabled( bool value )
		{
			_enabled = value;
		}

		void SingleSentAlarm::setPeriodStart( const time::DateTime& periodStart )
		{
			_periodStart = periodStart;
		}

		void SingleSentAlarm::setPeriodEnd( const time::DateTime& periodEnd )
		{
			_periodEnd = periodEnd;
		}

		bool SingleSentAlarm::getIsEnabled() const
		{
			return _enabled;
		}

		const time::DateTime& SingleSentAlarm::getPeriodStart() const
		{
			return _periodStart;
		}

		const time::DateTime& SingleSentAlarm::getPeriodEnd() const
		{
			return _periodEnd;
		}

		SingleSentAlarm::~SingleSentAlarm()
		{

		}
	}
}
