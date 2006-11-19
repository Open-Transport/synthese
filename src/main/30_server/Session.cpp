
#include "01_util/Conversion.h"

#include "04_time/DateTime.h"

#include "30_server/Session.h"
#include "30_server/SessionException.h"

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace server
	{
		const size_t Session::KEY_LENGTH = 20;
		const int Session::MAX_MINUTES_DURATION = 10;

		Session::Session(const std::string& ip)
			: _ip(ip)
			, _key(generateKey())
		{

		}
		void Session::controlAndRefresh(const std::string& ip)
		{
			if (ip != _ip)
				throw SessionException("IP has changed during the session.");

			DateTime now;
			if (now - _lastUse > MAX_MINUTES_DURATION)
				throw SessionException("Session is too old");

			_lastUse = now;
		}

		std::string Session::generateKey()
		{
			std::string key;
			for (size_t i=0; i< Session::KEY_LENGTH; ++i)
			{
				key += Conversion::ToString(rand());
			}
			return key;
		}
	}
}