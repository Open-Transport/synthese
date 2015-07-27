/** MessageApplicationPeriod unit test.
	@file MessageApplicationPeriodTest.cpp

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
#include "MessageApplicationPeriod.hpp"

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



BOOST_AUTO_TEST_CASE (middleOfPeriodShouldBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 15))));
}


BOOST_AUTO_TEST_CASE (startOfPeriodShouldBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 10), hours(10) + minutes(30))));
}


BOOST_AUTO_TEST_CASE (oneSecondBeforeStartOfPeriodShouldNotBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 10), hours(10) + minutes(30) - seconds(1))));
}


BOOST_AUTO_TEST_CASE (endOfPeriodShouldBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 20), hours(22) + minutes(30))));
}


BOOST_AUTO_TEST_CASE (oneSecondAfterEndOfPeriodShouldNotBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 20), hours(22) + minutes(30) + seconds(1))));
}


BOOST_AUTO_TEST_CASE (middleOfHourRangeShouldBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)),
										   hours(14), hours(18));
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 15), hours(16))));
}

BOOST_AUTO_TEST_CASE (startOfHourRangeShouldBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)),
										   hours(14), hours(18));
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 15), hours(14))));
}

BOOST_AUTO_TEST_CASE (oneSecondBeforeStartOfHourRangeShouldNotBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)),
										   hours(14), hours(18));
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 15), hours(14) - seconds(1))));
}


BOOST_AUTO_TEST_CASE (endOfHourRangeShouldBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)),
										   hours(14), hours(18));
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 15), hours(18))));
}


BOOST_AUTO_TEST_CASE (oneSecondAfterEndOfHourRangeShouldNotBeInsideApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)),
										   hours(14), hours(18));
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isInside(ptime(date(2015, Jul, 15), hours(18) + seconds(1))));
}

BOOST_AUTO_TEST_CASE (anyMomentShouldBeInsideInfiniteApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(not_a_date_time,
										   not_a_date_time);
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isInside(ptime(date(2035, Jul, 15))));
}

BOOST_AUTO_TEST_CASE (outOfHourRangeShouldNotBeInsideInfiniteApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(not_a_date_time,
										   not_a_date_time,
										   hours(14), hours(18));
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isInside(ptime(date(2035, Jul, 15), hours(19))));
}


BOOST_AUTO_TEST_CASE (startOfPeriodShouldNotBeAfterApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isAfter(ptime(date(2015, Jul, 10), hours(10) + minutes(30))));
}


BOOST_AUTO_TEST_CASE (middleOfPeriodShouldNotBeAfterApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isAfter(ptime(date(2015, Jul, 15))));
}


BOOST_AUTO_TEST_CASE (endOfPeriodShouldNotBeAfterApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isAfter(ptime(date(2015, Jul, 20), hours(22) + minutes(30))));
}


BOOST_AUTO_TEST_CASE (oneSecondAfterEndOfPeriodShouldBeAfterApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)));
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isAfter(ptime(date(2015, Jul, 20), hours(22) + minutes(30) + seconds(1))));
}


BOOST_AUTO_TEST_CASE (anyMomentShouldNotBeAfterInfiniteApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(not_a_date_time,
										   not_a_date_time);
	BOOST_CHECK_EQUAL(false, messageApplicationPeriod->isAfter(ptime(date(2035, Jul, 15))));
}


BOOST_AUTO_TEST_CASE (oneSecondAfterEndOfHourRangeOnLastDayOfPeriodShouldBeAfterApplicationPeriod)
{
	boost::shared_ptr<MessageApplicationPeriod> messageApplicationPeriod =
			createMessageApplicationPeriod(ptime(date(2015, Jul, 10), hours(10) + minutes(30)),
										   ptime(date(2015, Jul, 20), hours(22) + minutes(30)),
										   hours(14), hours(18));
	BOOST_CHECK_EQUAL(true, messageApplicationPeriod->isAfter(ptime(date(2015, Jul, 20), hours(18) + seconds(1))));
}


