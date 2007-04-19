
/** SentAlarm class implementation.
	@file SentAlarm.cpp

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

#include "SentAlarm.h"

namespace synthese
{
	namespace messages
	{

		bool SentAlarm::isApplicable (
			const synthese::time::DateTime& start
			, const synthese::time::DateTime& end ) const
		{
			// Disabled alarm is never applicable
			if (!getIsEnabled())
				return false;

			// Start date control
			if (!getPeriodStart().isUnknown() && end < getPeriodStart())
				return false;

			// End date control
			if (!getPeriodEnd().isUnknown() && start >= getPeriodEnd())
				return false;

			return true;
		}

		bool SentAlarm::isApplicable( const time::DateTime& date ) const
		{
			return isApplicable(date, date);
		}

		SentAlarm::SentAlarm()
			: util::Registrable<uid, SentAlarm>()
		{

		}

		SentAlarm::~SentAlarm()
		{

		}

		uid SentAlarm::getId() const
		{
			return getKey();
		}
	}
}
