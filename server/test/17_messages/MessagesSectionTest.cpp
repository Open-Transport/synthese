
/** MessagesSection unit test.
	@file MessagesSectionTest.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#define private public

#include "MessageApplicationPeriod.hpp"
#include "MessagesModule.h"
#include "MessagesSection.hpp"
#include "ScenarioCalendar.hpp"
#include "SentScenario.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace boost::posix_time;
using namespace boost::gregorian;

using namespace std;
using namespace synthese::util;
using namespace synthese::messages;
using namespace synthese;

BOOST_AUTO_TEST_CASE (testMessagesSectionAutoActivation)
{
	MessagesSection* messageSectionWithoutAutoActivation = new MessagesSection(1);
	messageSectionWithoutAutoActivation->set<Name>("Section sans activation automatique");
	messageSectionWithoutAutoActivation->set<Rank>(0);
	messageSectionWithoutAutoActivation->set<Code>("code1");
	messageSectionWithoutAutoActivation->set<Color>("red");
	messageSectionWithoutAutoActivation->set<AutoActivation>(false);

	MessagesSection* messageSectionWithAutoActivation = new MessagesSection(2);
	messageSectionWithAutoActivation->set<Name>("Section avec activation automatique");
	messageSectionWithAutoActivation->set<Rank>(0);
	messageSectionWithAutoActivation->set<Code>("code2");
	messageSectionWithAutoActivation->set<Color>("green");
	messageSectionWithAutoActivation->set<AutoActivation>(true);

	BOOST_CHECK_EQUAL(messageSectionWithoutAutoActivation->get<AutoActivation>(), false);
	BOOST_CHECK_EQUAL(messageSectionWithAutoActivation->get<AutoActivation>(), true);

	ptime now(second_clock::local_time());

	SentScenario* scenario1 = new SentScenario(1);
	scenario1->setEventStart(now - days(1));
	scenario1->setEventEnd(now + days(1));
	scenario1->setIsEnabled(false);
	Sections::Type sections1;
	sections1.insert(messageSectionWithoutAutoActivation);
	scenario1->setSections(sections1);

	ScenarioCalendar* calendar1 = new ScenarioCalendar(1);
	calendar1->set<ScenarioPointer>(*scenario1);
	Scenario::ScenarioCalendars calendars1;
	calendars1.insert(calendar1);
	scenario1->setCalendars(calendars1);
	MessageApplicationPeriod* ap1 = new MessageApplicationPeriod(1);
	ap1->set<StartTime>(now - days(1));
	ap1->set<EndTime>(now + days(1));
	ap1->set<StartHour>(not_a_date_time);
	ap1->set<EndHour>(not_a_date_time);
	ap1->set<ScenarioCalendar>(*calendar1);
	ScenarioCalendar::ApplicationPeriods aps1;
	aps1.insert(ap1);
	calendar1->setApplicationPeriods(aps1);

	SentScenario* scenario2 = new SentScenario(2);
	scenario2->setEventStart(now - days(1));
	scenario2->setEventEnd(now + days(1));
	scenario2->setIsEnabled(false);
	Sections::Type sections2;
	sections2.insert(messageSectionWithAutoActivation);
	scenario2->setSections(sections2);

	ScenarioCalendar* calendar2 = new ScenarioCalendar(2);
	calendar2->set<ScenarioPointer>(*scenario2);
	Scenario::ScenarioCalendars calendars2;
	calendars2.insert(calendar2);
	scenario2->setCalendars(calendars2);
	MessageApplicationPeriod* ap2 = new MessageApplicationPeriod(2);
	ap2->set<StartTime>(now - days(1));
	ap2->set<EndTime>(now + days(1));
	ap2->set<StartHour>(not_a_date_time);
	ap2->set<EndHour>(not_a_date_time);
	ap2->set<ScenarioCalendar>(*calendar2);
	ScenarioCalendar::ApplicationPeriods aps2;
	aps2.insert(ap2);
	calendar2->setApplicationPeriods(aps2);

	BOOST_CHECK_EQUAL(scenario1->getIsEnabled(), false);
	BOOST_CHECK_EQUAL(scenario2->getIsEnabled(), false);

	BOOST_CHECK_EQUAL(MessagesModule::_enableScenarioIfAutoActivation(scenario1), false);
	BOOST_CHECK_EQUAL(MessagesModule::_enableScenarioIfAutoActivation(scenario2), true);

	BOOST_CHECK_EQUAL(scenario1->getIsEnabled(), false);
	BOOST_CHECK_EQUAL(scenario2->getIsEnabled(), true);

}

