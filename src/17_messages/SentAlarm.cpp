////////////////////////////////////////////////////////////////////////////////
/// SentAlarm class implementation.
///	@file SentAlarm.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "SentAlarm.h"
#include "AlarmRecipient.h"
#include "Registry.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<messages::SentAlarm>::KEY("SentAlarm");
	}

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

		SentAlarm::SentAlarm(RegistryKeyType key)
			: Alarm(key)
		{
		}

		SentAlarm::~SentAlarm()
		{

		}

		AlarmConflict SentAlarm::wereInConflictWith( const SentAlarm& other ) const
		{
			// If one of the two alarms are not enabled, no conflict
			if (!getIsEnabled() || !other.getIsEnabled())
				return ALARM_NO_CONFLICT;

			// Inverting the parameters if necessary
			if ((other.getPeriodStart().isUnknown() && !getPeriodStart().isUnknown())
				|| (!other.getPeriodStart().isUnknown() && !getPeriodStart().isUnknown() && other.getPeriodStart() < getPeriodStart()))
				return other.wereInConflictWith(*this);

			// No common period : no conflict
			if (!other.getPeriodStart().isUnknown() && !getPeriodEnd().isUnknown() && other.getPeriodStart() > getPeriodEnd())
				return ALARM_NO_CONFLICT;

			// Common period : different level gives priority to an alarm
			if (other.getLevel() != getLevel())
				return ALARM_WARNING_ON_INFO;
			
			return ALARM_CONFLICT;
		}

		AlarmConflict SentAlarm::getConflictStatus() const
		{
			AlarmConflict conflictStatus(ALARM_NO_CONFLICT);
			vector<shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
			BOOST_FOREACH(const shared_ptr<AlarmRecipient> recipient, recipients)
			{
				AlarmConflict thisConflictStatus = recipient->getConflictStatus(this);
				if (thisConflictStatus > conflictStatus)
					conflictStatus = thisConflictStatus;
				if (conflictStatus == ALARM_CONFLICT)
					return conflictStatus;
			}
			return conflictStatus;
		}

		SentAlarm::Complements SentAlarm::getComplements() const
		{
			return _complements;
		}

		void SentAlarm::setComplements( const Complements& complements )
		{
			_complements = complements;
		}
	}
}
