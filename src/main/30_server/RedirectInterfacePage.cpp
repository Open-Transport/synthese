
#include <sstream>

#include "30_server/Request.h"
#include "30_server/RedirectInterfacePage.h"

namespace synthese
{
	using namespace interfaces;

	namespace server
	{
		void RedirectInterfacePage::display( std::ostream& stream, const std::string& url, const server::Request* request /*= NULL*/ ) const
		{
			ParametersVector pv;
			pv.push_back(url);

			InterfacePage::display(stream, pv, NULL, request);
		}

		void RedirectInterfacePage::display( std::ostream& stream, const server::Request* request /*= NULL*/ ) const
		{
			std::stringstream url;
			url << request->getClientURL() << Request::PARAMETER_STARTER << request->getQueryString();
			display(stream, url.str(), request);
		}
	}
}
