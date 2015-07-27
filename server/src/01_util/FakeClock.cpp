#include "FakeClock.hpp"

namespace synthese
{
namespace util
{

FakeClock::FakeClock(const boost::posix_time::ptime& localTime)
	: _localTime(localTime)
{

}


boost::posix_time::ptime
FakeClock::getLocalTime() const
{
	return _localTime;
}

void
FakeClock::setLocalTime(const boost::posix_time::ptime& localTime)
{
	_localTime = localTime;
}

}
}
