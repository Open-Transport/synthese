#include "15_env/Calendar.h"
#include "04_time/Date.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;
using namespace synthese::env;
using synthese::time::Date;


BOOST_AUTO_TEST_CASE (testCalendarSimpleMarking)
{


    Calendar calendar;
    
    BOOST_CHECK_EQUAL (0, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (Date::UNKNOWN_DATE, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (Date::UNKNOWN_DATE, calendar.getLastMarkedDate ());

    Date d02042006 (2, 4, 2006);
    Date d14072006 (14, 7, 2006);
    Date d16072006 (16, 7, 2006);
    Date d17072006 (17, 7, 2006);
    Date d18072006 (18, 7, 2006);
    Date d06082006 (6, 8, 2006);

    calendar.mark (d16072006);
    BOOST_CHECK (calendar.isMarked (d16072006));
    BOOST_CHECK_EQUAL (1, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (d16072006, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (d16072006, calendar.getLastMarkedDate ());
    
    calendar.mark (d18072006);
    BOOST_CHECK (calendar.isMarked (d16072006));
    BOOST_CHECK (calendar.isMarked (d17072006) == false);
    BOOST_CHECK (calendar.isMarked (d18072006));
    BOOST_CHECK_EQUAL (2, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (d16072006, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d14072006);
    calendar.mark (d17072006);
    BOOST_CHECK (calendar.isMarked (d14072006));
    BOOST_CHECK (calendar.isMarked (d16072006));
    BOOST_CHECK (calendar.isMarked (d17072006));
    BOOST_CHECK (calendar.isMarked (d18072006));
    BOOST_CHECK_EQUAL (4, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (d14072006, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d02042006);
    BOOST_CHECK (calendar.isMarked (d02042006));
    BOOST_CHECK (calendar.isMarked (d14072006));
    BOOST_CHECK (calendar.isMarked (d16072006));
    BOOST_CHECK (calendar.isMarked (d17072006));
    BOOST_CHECK (calendar.isMarked (d18072006));
    BOOST_CHECK_EQUAL (5, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (d02042006, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d14072006, false);
    calendar.mark (d16072006, false);
    BOOST_CHECK (calendar.isMarked (d02042006));
    BOOST_CHECK (calendar.isMarked (d17072006));
    BOOST_CHECK (calendar.isMarked (d18072006));
    BOOST_CHECK_EQUAL (3, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (d02042006, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d02042006, false);
    BOOST_CHECK (calendar.isMarked (d17072006));
    BOOST_CHECK (calendar.isMarked (d18072006));
    BOOST_CHECK_EQUAL (2, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (d17072006, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (d18072006, calendar.getLastMarkedDate ());

    calendar.mark (d06082006);
    calendar.mark (d18072006, false);
    BOOST_CHECK (calendar.isMarked (d17072006));
    BOOST_CHECK (calendar.isMarked (d06082006));
    BOOST_CHECK_EQUAL (2, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (d17072006, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (d06082006, calendar.getLastMarkedDate ());

    calendar.mark (d06082006, false);
    BOOST_CHECK (calendar.isMarked (d17072006));
    BOOST_CHECK_EQUAL (1, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (d17072006, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (d17072006, calendar.getLastMarkedDate ());

    calendar.mark (d17072006, false);
    BOOST_CHECK_EQUAL (0, calendar.getNbMarkedDates ());
    BOOST_CHECK_EQUAL (Date::UNKNOWN_DATE, calendar.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (Date::UNKNOWN_DATE, calendar.getLastMarkedDate ());

}



BOOST_AUTO_TEST_CASE (testCalendarLogicalAnd)
{
    Calendar calendar1;
    calendar1.mark (Date (2, 4, 2006));
    calendar1.mark (Date (4, 4, 2006));
    calendar1.mark (Date (5, 4, 2006));

    Calendar calendar2;
    calendar2.mark (Date (1, 4, 2006));
    calendar2.mark (Date (4, 4, 2006));
    calendar2.mark (Date (5, 4, 2006));
    calendar2.mark (Date (6, 4, 2006));

	Calendar calendar1and2 = calendar1 & calendar2;
    BOOST_CHECK_EQUAL (2, calendar1and2.getNbMarkedDates ());
    BOOST_CHECK (calendar1and2.isMarked (Date (4, 4, 2006)));
    BOOST_CHECK (calendar1and2.isMarked (Date (5, 4, 2006)));
    BOOST_CHECK_EQUAL (Date (4, 4, 2006), calendar1and2.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (Date (5, 4, 2006), calendar1and2.getLastMarkedDate ());

	Calendar calendar2and1 = calendar2 & calendar1;
    BOOST_CHECK_EQUAL (2, calendar2and1.getNbMarkedDates ());
    BOOST_CHECK (calendar2and1.isMarked (Date (4, 4, 2006)));
    BOOST_CHECK (calendar2and1.isMarked (Date (5, 4, 2006)));
    BOOST_CHECK_EQUAL (Date (4, 4, 2006), calendar2and1.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (Date (5, 4, 2006), calendar2and1.getLastMarkedDate ());

}



BOOST_AUTO_TEST_CASE (testCalendarLogicalOr)
{
    Calendar calendar1;
    calendar1.mark (Date (2, 4, 2006));
    calendar1.mark (Date (4, 4, 2006));
    calendar1.mark (Date (5, 4, 2006));

    Calendar calendar2;
    calendar2.mark (Date (1, 4, 2006));
    calendar2.mark (Date (4, 4, 2006));
    calendar2.mark (Date (5, 4, 2006));
    calendar2.mark (Date (6, 4, 2006));

	Calendar calendar1or2 = calendar1 | calendar2;
    BOOST_CHECK_EQUAL (5, calendar1or2.getNbMarkedDates ());
    BOOST_CHECK (calendar1or2.isMarked (Date (1, 4, 2006)));
    BOOST_CHECK (calendar1or2.isMarked (Date (2, 4, 2006)));
    BOOST_CHECK (calendar1or2.isMarked (Date (4, 4, 2006)));
    BOOST_CHECK (calendar1or2.isMarked (Date (5, 4, 2006)));
    BOOST_CHECK (calendar1or2.isMarked (Date (6, 4, 2006)));
    BOOST_CHECK_EQUAL (Date (1, 4, 2006), calendar1or2.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (Date (6, 4, 2006), calendar1or2.getLastMarkedDate ());

	Calendar calendar2or1 = calendar2 | calendar1;
    BOOST_CHECK_EQUAL (5, calendar2or1.getNbMarkedDates ());
    BOOST_CHECK (calendar2or1.isMarked (Date (1, 4, 2006)));
    BOOST_CHECK (calendar2or1.isMarked (Date (2, 4, 2006)));
    BOOST_CHECK (calendar2or1.isMarked (Date (4, 4, 2006)));
    BOOST_CHECK (calendar2or1.isMarked (Date (5, 4, 2006)));
    BOOST_CHECK (calendar2or1.isMarked (Date (6, 4, 2006)));
    BOOST_CHECK_EQUAL (Date (1, 4, 2006), calendar2or1.getFirstMarkedDate ());
    BOOST_CHECK_EQUAL (Date (6, 4, 2006), calendar2or1.getLastMarkedDate ());



}



