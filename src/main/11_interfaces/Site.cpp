
#include "Site.h"
#include "15_env/Environment.h"
#include "04_time/DateTime.h"

namespace synthese
{
	using namespace time;
	using namespace std;
	using namespace util;
	using namespace env;

	namespace interfaces
	{
		Site::Site( const uid& id, const std::string& key )
			: _key(key)
			, Registrable<uid, Site>(id)
		{
			
		}

		const Interface* Site::getInterface() const
		{
			return _interface;
		}

		const synthese::env::Environment* Site::getEnvironment() const
		{
			return _env;
		}

		bool Site::onlineBookingAllowed() const
		{
			return _onlineBookingAllowed;
		}

		Date Site::dateInterpretee( const string& text ) const
		{
			Date tempDate;
			if ( !text.size () )
				tempDate.updateDate( TIME_CURRENT );

			else if ( text.size () == 1 )
				switch ( text[ 0 ] )
			{
				case TEMPS_MIN_CIRCULATIONS:
					tempDate = _env->getMinDateInUse ();
					break;
				case TEMPS_MAX_CIRCULATIONS:
					tempDate = _env->getMaxDateInUse ();
					break;
				default:
					tempDate.updateDate(text[ 0 ] );
			}
			else
				tempDate.FromString(text);
			return tempDate;
		}

		bool Site::dateControl() const
		{
			DateTime tempDate;
			return tempDate.getDate() >= _startValidityDate && tempDate.getDate() <= _endValidityDate;
		}
	}
}
