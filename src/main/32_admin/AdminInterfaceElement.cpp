
#include <sstream>

#include "30_server/Request.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/AdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace server;

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

		std::string AdminInterfaceElement::getHTMLLink(const Request* request) const
		{
			stringstream str;
			Request* linkRequest = Factory<Request>::create<AdminRequest>();
			linkRequest->copy(request);
			((AdminRequest* ) linkRequest)->setPage(this);
			str << linkRequest->getHTMLLink(getTitle());
			((AdminRequest* ) linkRequest)->setPage(NULL); // To avoid the deletion of the page which can be used later
			delete linkRequest;
			return str.str();
		}
	}
}

