/** 04_time module wide definitions
   @file module.h
*/

#ifndef SYNTHESE_TIME_MODULE_H
#define SYNTHESE_TIME_MODULE_H

namespace synthese
{

/** @defgroup m04 04 Time classes

@{
*/

/** 04_time namespace */
namespace time
{

static const int LEAP_YEAR_DAYS_COUNT = 366;
static const int NON_LEAP_YEAR_DAYS_COUNT = 365;

static const int MONTHS_PER_YEAR = 12;
static const int HOURS_PER_DAY = 24;
static const int MINUTES_PER_HOUR = 60;
static const int MINUTES_PER_DAY = 1440;

static const char TIME_MIN = 'm';
static const char TIME_MAX = 'M';
static const char TIME_UNCHANGED = '_';
static const char TIME_SAME = 'I';
static const char TIME_CURRENT = 'A';

}

/** @} */

}

#endif
