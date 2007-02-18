
#include <sstream>

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/Interface.h"
#include "11_interfaces/RedirRequest.h"
#include "11_interfaces/SimplePageRequest.h"

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

			if (!requestKey.empty())
			{
				try
				{
					SimplePageRequest* redirRequest = Factory<Request>::create<SimplePageRequest>();
					redirRequest->copy(request);
					redirRequest->setPage(_page->getInterface()->getPage(requestKey));
					redirRequest->setAction(Factory<Action>::create<LogoutAction>());
					stream << redirRequest->getHTMLLink(_content->getValue(parameters, NULL, request));
					delete redirRequest;
				}
				catch (InterfacePageException e)
				{
					Log::GetInstance().debug("No such interface page "+ requestKey);
				}
			}
			if (url.size())
			{
				RedirRequest* redirRequest = Factory<Request>::create<RedirRequest>();
				redirRequest->copy(request);
				redirRequest->setUrl(url);
				redirRequest->setAction(Factory<Action>::create<LogoutAction>());
				stream << redirRequest->getHTMLLink(_content->getValue(parameters, NULL, request));
				delete redirRequest;
			}
			return stream.str();
		}

	}
}
