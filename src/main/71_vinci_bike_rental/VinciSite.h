
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

		public:
			VinciSite(uid id=0);

			void setName(const std::string& name);

			const std::string& getName() const;
		};
	}
}

#endif // SYNTHESE_VinciSite_H__
