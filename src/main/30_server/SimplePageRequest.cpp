
#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/DisplayableElement.h"
#include "11_interfaces/InterfacePageException.h"

#include "30_server/RequestException.h"
#include "30_server/SimplePageRequest.h"

namespace synthese
{
	using namespace interfaces;

	namespace server
	{
		const std::string SimplePageRequest::PARAMETER_PAGE = "page";

		SimplePageRequest::SimplePageRequest()
			: Request(Request::DONT_NEED_SESSION) {}

		void SimplePageRequest::run( std::ostream& stream ) const
		{
			ParametersVector pv;
			for (Request::ParametersMap::const_iterator it = _parameters.begin(); it != _parameters.end(); ++it)
				pv.push_back(it->second);
			_page->display(stream, pv, NULL, this);
		}

		void SimplePageRequest::setFromParametersMap( const server::Request::ParametersMap& map )
		{
			_parameters = map;

			Request::ParametersMap::iterator it = _parameters.find(PARAMETER_PAGE);
			if (it == _parameters.end())
				throw RequestException("Page parameter not found in simple page query");
			try
			{
				_page = _site->getInterface()->getPage(it->second);
				_parameters.erase(it);
			}
			catch (InterfacePageException e)
			{
				throw RequestException("No such interface page : "+ it->second);
			}
		}

		server::Request::ParametersMap SimplePageRequest::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			return map;
		}

		void SimplePageRequest::setPage( const interfaces::InterfacePage* page )
		{
			_page = page;
		}
	}
}