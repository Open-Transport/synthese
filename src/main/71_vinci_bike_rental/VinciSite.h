
#ifndef SYNTHESE_VinciSite_H__
#define SYNTHESE_VinciSite_H__

#include "01_util/Registrable.h"

namespace synthese
{
	namespace vinci
	{
		class VinciSite : public util::Registrable<uid, VinciSite>
		{
		private:
			std::string _name;
		};
	}
}

#endif // SYNTHESE_VinciSite_H__
