
#include "AdminRequest.h"
#include "01_util/Conversion.h"
#include "RequestException.h"
#include "AdminInterfaceElement.h"
#include "01_util/FactoryException.h"
#include "AdminInterfacePage.h"
#include "Interface.h"

namespace synthese
{
	using namespace util;

	namespace interfaces
	{
		const std::string AdminRequest::PARAMETER_PAGE = "rub";
		const std::string AdminRequest::PARAMETER_OBJECT_ID = "id";

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
			if (it == map.end())
				throw RequestException("Page to display not specified");
			try
			{
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
			aip->display(stream, _page, _object_id, _site);
		}

		AdminRequest::~AdminRequest()
		{
			delete _page;
		}
	}
}