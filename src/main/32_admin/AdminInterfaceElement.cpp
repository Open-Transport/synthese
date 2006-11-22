
#include "AdminInterfaceElement.h"

namespace synthese
{
	namespace admin
	{
		AdminInterfaceElement::AdminInterfaceElement(const std::string& superior, AdminInterfaceElement::DisplayMode everDisplayed)
			: _superior(superior)
			, _everDisplayed(everDisplayed)
			, DisplayableElement()
		{		}

		const AdminInterfaceElement::DisplayMode AdminInterfaceElement::getDisplayMode() const
		{
			return _everDisplayed;
		}

		const std::string& AdminInterfaceElement::getSuperior() const
		{
			return _superior;
		}
	}
}

