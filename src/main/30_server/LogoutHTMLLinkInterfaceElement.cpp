
#include <sstream>

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/Interface.h"

#include "30_server/ActionOnlyRequest.h"
#include "30_server/SimplePageRequest.h"
#include "30_server/LogoutAction.h"
#include "30_server/LogoutHTMLLinkInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	namespace server
	{
		void LogoutHTMLLinkInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 3)
				throw InterfacePageException("Not enough parameters for logout HTML link");
			_redirectionURL = vel.front();
			_page_key = vel.front();
			_content = vel.front();
		}

		string LogoutHTMLLinkInterfaceElement::getValue(const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stringstream stream;
			std::string url = _redirectionURL->getValue(parameters, rootObject, request);
			std::string requestKey = _page_key->getValue(parameters, rootObject, request);

			Request* redirRequest = NULL;
			if (requestKey != "")
			{
				try
				{
					InterfacePage* page = request->getSite()->getInterface()->getPage(requestKey);
					SimplePageRequest* spr = new SimplePageRequest;
					spr->setPage(page);
					redirRequest = (Request*) spr;
				}
				catch (InterfacePageException e)
				{
					Log::GetInstance().debug("No such interface page "+ requestKey);
				}
			}
			if (redirRequest == NULL)
			{
				ActionOnlyRequest* aor = new ActionOnlyRequest;
				aor->setURL(url);
				redirRequest = (Request*) aor;
			}
			redirRequest->copy(request);
			redirRequest->setAction(new LogoutAction);

			stream << redirRequest->getHTMLLink(_content->getValue(parameters, NULL, request));

			delete redirRequest;
			return stream.str();
		}

	}
}