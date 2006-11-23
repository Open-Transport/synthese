
#include <sstream>

#include "11_interfaces/ValueElementList.h"

#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminPagesTreeInterfaceElement.h"

namespace synthese
{
	using namespace std;
	using namespace interfaces;
	using namespace util;
	using namespace server;

	namespace admin
	{
		void AdminPagesTreeInterfaceElement::storeParameters(ValueElementList& vel)
		{
			//_parameter1 = vel.front();
			/// @todo control and Fill the parameters init
		}

		std::string AdminPagesTreeInterfaceElement::getValue( const ParametersVector&, const void* object /* = NULL */, const server::Request* request /* = NULL */ ) const
		{
			return getSubPages("", ((const AdminInterfaceElement*) object)->getFactoryKey(), request);
		}

		AdminPagesTreeInterfaceElement::~AdminPagesTreeInterfaceElement()
		{
			/// @todo Destroy all the parameters
			//delete _parameter1;
		}

		std::string AdminPagesTreeInterfaceElement::getSubPages( const std::string& page, const std::string& currentPage, const Request* request )
		{
			stringstream str;
			for (Factory<AdminInterfaceElement>::Iterator it = Factory<AdminInterfaceElement>::begin(); it != Factory<AdminInterfaceElement>::end(); ++it)
			{
				if (it->getSuperior() == page && it->getDisplayMode() == AdminInterfaceElement::EVER_DISPLAYED)
				{
					str << "<li>";
					if (it.getKey() != currentPage)
					{
						str << it->getHTMLLink(request);
					}
					else
					{
						str << it->getTitle();
					}
					str << "</li>"
						<< getSubPages(it->getFactoryKey(), currentPage, request);
				}
			}
			return str.str();
		}
	}
}
