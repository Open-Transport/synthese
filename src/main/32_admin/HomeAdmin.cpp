
#include "HomeAdmin.h"

namespace synthese
{
	namespace admin
	{
		const std::string HomeAdmin::_superior = "";
		const bool HomeAdmin::_ever_displayed = true;


		void HomeAdmin::display( std::ostream& stream, const interfaces::ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stream << "Bienvenue sur le module d'interfaces";
		}
	}
}