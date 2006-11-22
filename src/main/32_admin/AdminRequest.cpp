
#include "01_util/Conversion.h"
#include "01_util/FactoryException.h"

#include "11_interfaces/Interface.h"

#include "30_server/RequestException.h"

#include "32_admin/HomeAdmin.h"
#include "32_admin/AdminInterfacePage.h"
#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminRequest.h"

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace interfaces;

	namespace admin
	{
		const std::string AdminRequest::PARAMETER_PAGE = "rub";
		const std::string AdminRequest::PARAMETER_OBJECT_ID = "id";

		AdminRequest::AdminRequest()
			: Request(Request::NEEDS_SESSION) {}

		AdminRequest::ParametersMap AdminRequest::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			map.insert(make_pair(PARAMETER_OBJECT_ID, Conversion::ToString(_object_id)));
			return map;
		}

		void AdminRequest::setFromParametersMap( const ParametersMap& map )
		{
			ParametersMap::const_iterator it;
			
			// Page
			it = map.find(PARAMETER_PAGE);
			try
			{
				if (it == map.end())
					_page = new HomeAdmin;
				else
					_page = Factory<AdminInterfaceElement>::create(it->second);
			}
			catch (FactoryException<AdminInterfaceElement> e)
			{
				throw RequestException("Page not found");
			}

			// Object ID
			it = map.find(PARAMETER_OBJECT_ID);
			_object_id = (it == map.end()) ? 0 : Conversion::ToLongLong(it->second);
			
		}

		void AdminRequest::run( std::ostream& stream ) const
		{
			const AdminInterfacePage* aip;
			try
			{
				aip = _site->getInterface()->getPage<AdminInterfacePage>();
			}
			catch (Exception e)
			{
				throw RequestException("Admin interface page not implemented in database");
			}
			aip->display(stream, _page, _object_id, this);
		}

		AdminRequest::~AdminRequest()
		{
			delete _page;
		}
	}
}