
#include "HomeAdmin.h"

namespace synthese
{
	namespace admin
	{
		HomeAdmin::HomeAdmin()
			: AdminInterfaceElement("", AdminInterfaceElement::EVER_DISPLAYED)
		{

		}

		void HomeAdmin::display( std::ostream& stream, const interfaces::ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stream << "Bienvenue sur le module d'interfaces";
		}

		std::string HomeAdmin::getTitle() const
		{
			return "Accueil";
		}
	}
}