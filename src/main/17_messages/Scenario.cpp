
/** Scenario class implementation.
	@file Scenario.cpp

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

#include "17_messages/Scenario.h"
#include "17_messages/Alarm.h"

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace messages
	{


		Scenario::Scenario()
			: Registrable<uid, Scenario>()
			, _isEnabled(false)
			, _periodStart(TIME_UNKNOWN)
			, _periodEnd(TIME_UNKNOWN)
		{

		}

		void Scenario::setName( const std::string& name )
		{
			_name = name;
		}

		const Scenario::AlarmsSet& Scenario::getAlarms() const
		{
			return _alarms;
		}

		const std::string& Scenario::getName() const
		{
			return _name;
		}

		void Scenario::setPeriodStart( const synthese::time::DateTime& periodStart )
		{
			_periodStart = periodStart;
			for (AlarmsSet::const_iterator it = getAlarms().begin(); it != getAlarms().end(); ++it)
				(*it)->setPeriodStart(_periodStart);
		}

		void Scenario::setPeriodEnd( const synthese::time::DateTime& periodEnd )
		{
			_periodEnd = periodEnd;
			for (AlarmsSet::const_iterator it = getAlarms().begin(); it != getAlarms().end(); ++it)
				(*it)->setPeriodEnd(_periodEnd);
		}

		void Scenario::setIsATemplate( bool isATemplate )
		{
			_isATemplate = isATemplate;
		}

		bool Scenario::getIsATemplate() const
		{
			return _isATemplate;
		}

		const time::DateTime& Scenario::getPeriodStart() const
		{
			return _periodStart;
		}

		const time::DateTime& Scenario::getPeriodEnd() const
		{
			return _periodEnd;
		}

		void Scenario::setIsEnabled( bool value )
		{
			_isEnabled = value;
		}

		bool Scenario::getIsEnabled() const
		{
			return _isEnabled;
		}

		Scenario* Scenario::createCopy() const
		{
			Scenario* scenario = new Scenario;
			scenario->setIsATemplate(false);
			scenario->setName(getName());
			
			for (AlarmsSet::const_iterator it = getAlarms().begin(); it != getAlarms().end(); ++it)
				scenario->addAlarm((*it)->createCopy(scenario));

			return scenario;
		}

		Scenario::~Scenario()
		{
			for (AlarmsSet::const_iterator it = getAlarms().begin(); it != getAlarms().end(); ++it)
				delete *it;
		}

		void Scenario::addAlarm( Alarm* alarm )
		{
			_alarms.insert(alarm);
		}

		void Scenario::removeAlarm( Alarm* alarm )
		{
			AlarmsSet::iterator it = _alarms.find(alarm);
			if (it != _alarms.end())
				_alarms.erase(it);
		}
	}
}
