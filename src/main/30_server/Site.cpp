
#include "Site.h"
#include "15_env/Environment.h"
#include "04_time/DateTime.h"

namespace synthese
{
	using namespace time;
	using namespace std;
	using namespace util;
	using namespace env;
	using namespace interfaces;

	namespace server
	{
		Site::Site( const uid& id)
			: Registrable<uid, Site>(id)
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

		void Site::setEnvironment( synthese::env::Environment* environment )
		{
			_env = environment;
		}

		void Site::setInterface( const synthese::interfaces::Interface* interf )
		{
			_interface = interf;
		}

		void Site::setStartDate( const synthese::time::Date& dateDebut )
		{
			_startValidityDate = dateDebut;
		}

		void Site::setEndDate( const synthese::time::Date& dateFin )
		{
			_endValidityDate = dateFin;
		}

		void Site::setOnlineBookingAllowed( const bool valeur )
		{
			_onlineBookingAllowed = valeur;
		}

		void Site::setClientURL( const std::string& clientURL )
		{
			_clientURL = clientURL;
		}

		void Site::setPastSolutionsDisplayed( bool pastSolutions)
		{
			_pastSolutionsDisplayed = pastSolutions;
		}

		void Site::setName( const std::string& name )
		{
			_name = name;
		}
	}
}
