
#include "HomeAdmin.h"

namespace synthese
{
	namespace interfaces
	{
		const std::string HomeAdmin::_superior = "";
		const bool HomeAdmin::_ever_displayed = true;


		void HomeAdmin::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const Site* site /*= NULL*/ ) const
		{
			stream << "Bienvenue sur le module d'interfaces";
		}
	}
}