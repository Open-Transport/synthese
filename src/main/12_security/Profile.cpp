
#include "01_util/Exception.h"

#include "12_security/Profile.h"
#include "12_security/Right.h"

namespace synthese
{
	using namespace util;

	namespace security
	{


		Profile::Profile( uid id )
			: Registrable<uid, Profile>(id)
			, _parentId(0)
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

		void Profile::setRights( const std::string& text )
		{
			/// @todo Implement it
		}
		void Profile::cleanRights()
		{
			for (RightsVector::iterator it = _rights.begin(); it != _rights.end(); ++it)
				delete *it;
		}

		void Profile::setParent(uid id)
		{
			_parentId = id;
		}

		const std::string& Profile::getName() const
		{
			return _name;
		}

		const uid Profile::getParentId() const
		{
			return _parentId;
		}

		const std::string Profile::getRightsString() const
		{
			/// @todo Implementation
			return "*,*,100,100";
		}

		void Profile::removeRight( const std::string& key )
		{
			for (Profile::RightsVector::iterator it = _rights.begin(); it != _rights.end(); ++it)
				if ((*it)->getFactoryKey() == key)
				{
					_rights.erase(it);
					return;
				}
			throw Exception("Right not found");
		}

		Right* Profile::getRight( const std::string& key )
		{
			for (Profile::RightsVector::iterator it = _rights.begin(); it != _rights.end(); ++it)
				if ((*it)->getFactoryKey() == key)
					return *it;
			throw Exception("Right not found");
		}

		const Profile::RightsVector& Profile::getRights() const
		{
			return _rights;
		}
	}
}
