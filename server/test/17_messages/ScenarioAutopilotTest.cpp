/** ScenarioAutopilot unit test.
	@file ScenarioAutopilotTest.cpp

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
#include "ScenarioAutopilot.hpp"
#include "MessagesSection.hpp"
#include "SentScenario.h"
#include "SentScenarioDao.hpp"
#include "FakeClock.hpp"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>


using namespace boost::posix_time;
using namespace boost::gregorian;

using namespace synthese::util;
using namespace synthese::messages;
using namespace synthese;


boost::shared_ptr<MessageApplicationPeriod>
createMessageApplicationPeriod(ptime startDateTime, ptime endDateTime,
							   time_duration startHour = time_duration(0, 0, 0, 0),
							   time_duration endHour = time_duration(23, 59, 0, 0))
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod(new MessageApplicationPeriod);
	messageApplicationPeriod->set<StartTime>(startDateTime);
	messageApplicationPeriod->set<EndTime>(endDateTime);
	messageApplicationPeriod->set<StartHour>(startHour);
	messageApplicationPeriod->set<EndHour>(endHour);
	return messageApplicationPeriod;
}


struct SentScenarioDaoMock : public SentScenarioDao
{
	std::vector<boost::shared_ptr<SentScenario> > sentScenarios;
	mutable int saveCalled;

	SentScenarioDaoMock(boost::shared_ptr<SentScenario>& sentScenario)
		: saveCalled(0)
	{
		sentScenarios.push_back(sentScenario);
	}

	std::vector<boost::shared_ptr<SentScenario> > list() const { return sentScenarios; }
	void save(boost::shared_ptr<SentScenario>& sentScenario) const { ++saveCalled; }
};


struct single_scenario_on_single_application_period_fixture
{

	FakeClock clock;
	boost::shared_ptr<SentScenario> sentScenario;
	boost::shared_ptr<ScenarioCalendar> scenarioCalendar;
	boost::shared_ptr<MessageApplicationPeriod> applicationPeriod;
	boost::shared_ptr<MessagesSection> scenarioSection;

	boost::shared_ptr<SentScenarioDaoMock> sentScenarioDao;
	boost::shared_ptr<ScenarioAutopilot> autopilot;

	single_scenario_on_single_application_period_fixture(ptime startDateTime, ptime endDateTime,
														 time_duration startHour = time_duration(0, 0, 0, 0),
														 time_duration endHour = time_duration(23, 59, 0, 0))
		: clock(ptime(date(2015, Jan, 1)))
	{
		scenarioSection.reset(new MessagesSection());
		scenarioSection->set<AutoActivation>(true);
		std::set<const MessagesSection*> scenarioSections;
		scenarioSections.insert(scenarioSection.get());

		scenarioCalendar.reset(new ScenarioCalendar);
		std::set<MessageApplicationPeriod*> messageApplicationPeriods;
		applicationPeriod = createMessageApplicationPeriod(startDateTime, endDateTime, startHour, endHour);
		messageApplicationPeriods.insert(applicationPeriod.get());
		scenarioCalendar->setApplicationPeriods(messageApplicationPeriods);
		std::set<ScenarioCalendar*> scenarioCalendars;
		scenarioCalendars.insert(scenarioCalendar.get());

		sentScenario.reset(new SentScenario);
		sentScenario->setIsEnabled(false);
		sentScenario->setArchived(false);
		sentScenario->setManualOverride(false);
		sentScenario->setCalendars(scenarioCalendars);
		sentScenario->setSections(scenarioSections);

		sentScenarioDao.reset(new SentScenarioDaoMock(sentScenario));

		autopilot.reset(new ScenarioAutopilot(*sentScenarioDao, clock));
	}

};

struct single_scenario_from_2015_Jul_10_to_2015_Jul_20 : public single_scenario_on_single_application_period_fixture
{

	single_scenario_from_2015_Jul_10_to_2015_Jul_20()
		: single_scenario_on_single_application_period_fixture(ptime(date(2015, Jul, 10)),
															   ptime(date(2015, Jul, 20)))
	{
	}

	void clock_is_before_application_period()
	{
		clock.setLocalTime(ptime(date(2015, Jul, 10)) - seconds(1));
	}

	void clock_is_inside_application_period()
	{
		clock.setLocalTime(ptime(date(2015, Jul, 15)));
	}

	void clock_is_after_application_period()
	{
		clock.setLocalTime(ptime(date(2015, Jul, 20), seconds(1)));
	}


	void check_scenario_was_enabled()
	{
		BOOST_CHECK_EQUAL(true, sentScenario->getIsEnabled());
		BOOST_CHECK_EQUAL(1, sentScenarioDao->saveCalled);
		sentScenarioDao->saveCalled = 0;
	}

	void check_scenario_was_disabled()
	{
		BOOST_CHECK_EQUAL(false, sentScenario->getIsEnabled());
		BOOST_CHECK_EQUAL(1, sentScenarioDao->saveCalled);
		sentScenarioDao->saveCalled = 0;
	}

	void check_scenario_is_still_enabled()
	{
		BOOST_CHECK_EQUAL(true, sentScenario->getIsEnabled());
		BOOST_CHECK_EQUAL(0, sentScenarioDao->saveCalled);
	}

	void check_scenario_is_still_disabled()
	{
		BOOST_CHECK_EQUAL(false, sentScenario->getIsEnabled());
		BOOST_CHECK_EQUAL(0, sentScenarioDao->saveCalled);
	}


	void check_scenario_was_disabled_and_archived()
	{
		BOOST_CHECK_EQUAL(false, sentScenario->getIsEnabled());
		BOOST_CHECK_EQUAL(true, sentScenario->getArchived());
		BOOST_CHECK_EQUAL(1, sentScenarioDao->saveCalled);
		sentScenarioDao->saveCalled = 0;
	}


};



BOOST_FIXTURE_TEST_CASE (scenario_should_be_enabled_when_entering_application_period,
						 single_scenario_from_2015_Jul_10_to_2015_Jul_20)
{
	clock_is_inside_application_period();
	autopilot->runOnce();
	check_scenario_was_enabled();
}



BOOST_FIXTURE_TEST_CASE (scenario_not_belonging_to_an_automatic_section_should_not_be_enabled_when_entering_application_period,
						 single_scenario_from_2015_Jul_10_to_2015_Jul_20)
{
	scenarioSection->set<AutoActivation>(false);

	clock_is_inside_application_period();
	autopilot->runOnce();
	check_scenario_is_still_disabled();
}


BOOST_FIXTURE_TEST_CASE (scenario_with_manual_override_should_not_be_enabled_when_entering_application_period,
						 single_scenario_from_2015_Jul_10_to_2015_Jul_20)
{
	sentScenario->setManualOverride(true);

	clock_is_inside_application_period();
	autopilot->runOnce();
	check_scenario_is_still_disabled();
}



BOOST_FIXTURE_TEST_CASE (archived_scenario_should_not_be_enabled_when_entering_application_period,
						 single_scenario_from_2015_Jul_10_to_2015_Jul_20)
{
	sentScenario->setArchived(true);

	clock_is_inside_application_period();
	autopilot->runOnce();
	check_scenario_is_still_disabled();
}


BOOST_FIXTURE_TEST_CASE (scenario_should_be_disabled_and_archived_when_leaving_application_period,
						 single_scenario_from_2015_Jul_10_to_2015_Jul_20)
{
	sentScenario->setIsEnabled(true);

	clock_is_after_application_period();
	autopilot->runOnce();
	check_scenario_was_disabled_and_archived();
}


BOOST_FIXTURE_TEST_CASE (scenario_not_belonging_to_an_automatic_section_should_not_be_disabled_when_leaving_application_period,
						 single_scenario_from_2015_Jul_10_to_2015_Jul_20)
{
	scenarioSection->set<AutoActivation>(false);
	sentScenario->setIsEnabled(true);

	clock_is_after_application_period();
	autopilot->runOnce();
	check_scenario_is_still_enabled();
}



BOOST_FIXTURE_TEST_CASE (scenario_with_manual_override_should_not_be_disabled_when_leaving_application_period,
						 single_scenario_from_2015_Jul_10_to_2015_Jul_20)
{
	sentScenario->setManualOverride(true);
	sentScenario->setIsEnabled(true);

	clock_is_after_application_period();
	autopilot->runOnce();
	check_scenario_is_still_enabled();
}


