
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

#include "SentScenario.h"
#include "ScenarioTemplate.h"
#include "Registry.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<messages::SentScenario>::KEY("SentScenario");
	}


	namespace messages
	{
		SentScenario::SentScenario(
			util::RegistryKeyType key
		):	Registrable(key),
			Scenario()
			, _isEnabled(false)
			, _periodStart(second_clock::local_time())
			, _periodEnd(not_a_date_time)
			, _template(NULL)
		{
		}

		SentScenario::SentScenario(
			const ScenarioTemplate& source
		):	Scenario(source.getName()),
			Registrable(UNKNOWN_VALUE),
			_isEnabled(false),
			_periodStart(second_clock::local_time()),
			_periodEnd(not_a_date_time),
			_template(&source)
		{
		}


		SentScenario::SentScenario(
			const SentScenario& source
		):	Scenario(source._template ? source._template->getName() : source.getName()),
			Registrable(UNKNOWN_VALUE),
			_isEnabled(false),
			_periodStart(second_clock::local_time()),
			_periodEnd(not_a_date_time),
			_template(source._template),
			_variables(source._variables)
		{
		}


		void SentScenario::setPeriodStart( const ptime& periodStart )
		{
			_periodStart = periodStart;
		}

		void SentScenario::setPeriodEnd( const ptime& periodEnd )
		{
			_periodEnd = periodEnd;
		}


		const ptime& SentScenario::getPeriodStart() const
		{
			return _periodStart;
		}

		const ptime& SentScenario::getPeriodEnd() const
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



		AlarmConflict SentScenario::getConflictStatus() const
		{
			AlarmConflict conflictStatus(ALARM_NO_CONFLICT);
/*			for (AlarmsSet::const_iterator it = getAlarms().begin(); it != getAlarms().end(); ++it)
			{
				AlarmConflict thisConflictStatus = (*it)->getConflictStatus();
				if (thisConflictStatus > conflictStatus)
					conflictStatus = thisConflictStatus;
				if (conflictStatus == ALARM_CONFLICT)
					return conflictStatus;
			}*/
			return conflictStatus;
		}

		bool SentScenario::isApplicable( const ptime& start, const ptime& end ) const
		{
			// Disabled alarm is never applicable
			if (!getIsEnabled())
				return false;

			// Start date control
			if (!getPeriodStart().is_not_a_date_time() && end < getPeriodStart())
				return false;

			// End date control
			if (!getPeriodEnd().is_not_a_date_time() && start >= getPeriodEnd())
				return false;

			return true;
		}

		bool SentScenario::isApplicable( const ptime& date ) const
		{
			return isApplicable(date, date);
		}



		const ScenarioTemplate* SentScenario::getTemplate(
		) const {
			return _template;
		}



		void SentScenario::setTemplate(
			const ScenarioTemplate* value
		) {
			_template = value;
		}



		const SentScenario::VariablesMap& SentScenario::getVariables(
		) const {
			return _variables;
		}



		void SentScenario::setVariables(
			const VariablesMap& value
		) {
			_variables = value;
		}
	}
}
