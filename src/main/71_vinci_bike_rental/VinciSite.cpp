
#include "71_vinci_bike_rental/VinciSite.h"

namespace synthese
{
	using namespace util;

	namespace vinci
	{
		VinciSite::VinciSite(uid id)
			: Registrable<uid, VinciSite>(id)
		{ }


		void VinciSite::setName(const std::string& name)
		{
			_name = name;
		}

		const std::string& VinciSite::getName() const
		{
			return _name;
		}
	}
}