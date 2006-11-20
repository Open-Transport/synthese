
#include "12_security/Profile.h"
#include "12_security/Right.h"

namespace synthese
{
	using namespace util;

	namespace security
	{


		Profile::Profile( uid id )
			: Registrable<uid, Profile>(id)
		{

		}

		Profile::~Profile()
		{
			cleanRights();
		}

		void Profile::setName( const std::string& name )
		{
			_name = name;
		}


		void Profile::setRights( const RightsVector& rightsvector )
		{
			cleanRights();
			_rights = rightsvector;
		}

		void Profile::cleanRights()
		{
			for (RightsVector::iterator it = _rights.begin(); it != _rights.end(); ++it)
				delete *it;
		}

		void Profile::setParent(Profile* parent )
		{
			_parent = parent;
		}
	}
}