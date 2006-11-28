
#ifndef SYNTHESE_VinciRate_H__
#define SYNTHESE_VinciRate_H__

#include <string>

namespace synthese
{
	namespace vinci
	{
		class VinciRate : public util::Registrable<uid, VinciRate>
		{
		private:
			std::string _name;
			
		};
	}
}

#endif // SYNTHESE_VinciRate_H__
