
#include "11_interfaces/Interface.h"

#include "30_server/RedirectInterfacePage.h"
#include "30_server/ActionOnlyRequest.h"

namespace synthese
{
	namespace server
	{
		
		const std::string ActionOnlyRequest::PARAMETER_URL = "url";


		server::Request::ParametersMap ActionOnlyRequest::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_URL, _url));
			return map;
		}

		
		void ActionOnlyRequest::setFromParametersMap( const server::Request::ParametersMap& map )
		{
			Request::ParametersMap::const_iterator it;
			it = map.find(PARAMETER_URL);
			if (it != map.end())
				_url = it->second;
		}

		
		void ActionOnlyRequest::run( std::ostream& stream ) const
		{
			const RedirectInterfacePage* page = _site->getInterface()->getPage<RedirectInterfacePage>();
			page->display(stream, _url, this);
		}

		void ActionOnlyRequest::setURL( const std::string& url )
		{
			_url = url;
		}

		ActionOnlyRequest::~ActionOnlyRequest()
		{

		}
	}
}