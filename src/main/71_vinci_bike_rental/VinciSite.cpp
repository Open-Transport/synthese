
#include "71_vinci_bke_rental/VinciSite.h"

namespace synthese
{
	namespace vinci
	{
		VinciSite::VinciSite(uid id=0)
			: Registrable<uid, VinciSite>(uid)
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