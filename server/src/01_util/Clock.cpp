#include "Clock.hpp"

namespace synthese
{
namespace util
{

Clock Clock::DEFAULT_CLOCK;


Clock::Clock()
{

}


Clock::~Clock()
{

}


boost::posix_time::ptime
Clock::getLocalTime() const
{
	return boost::posix_time::second_clock::local_time();
}


}
}
