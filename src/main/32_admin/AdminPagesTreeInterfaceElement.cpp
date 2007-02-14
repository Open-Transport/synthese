
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
			return getSubPages("", (const AdminInterfaceElement*) object, (const AdminRequest*) request);
		}

		AdminPagesTreeInterfaceElement::~AdminPagesTreeInterfaceElement()
		{
			/// @todo Destroy all the parameters
			//delete _parameter1;
		}

		/** @todo Put the html code as parameters */
		std::string AdminPagesTreeInterfaceElement::getSubPages( const std::string& page, const AdminInterfaceElement* currentPage, const AdminRequest* request)
		{
			stringstream str;
			for (Factory<AdminInterfaceElement>::Iterator it = Factory<AdminInterfaceElement>::begin(); it != Factory<AdminInterfaceElement>::end(); ++it)
			{
				if (it->getSuperior() == page 
					&& ((it->getDisplayMode() == AdminInterfaceElement::EVER_DISPLAYED)
					|| (it->getDisplayMode() == AdminInterfaceElement::DISPLAYED_IF_CURRENT && it.getKey() == currentPage->getFactoryKey()))
				){
					str << "<li>";
					if (it.getKey() == currentPage->getFactoryKey())
					{
						str << currentPage->getTitle();
					}
					else
					{
						str << it->getHTMLLink(request);	
					}
					str << "</li>";

					string sp = getSubPages(it->getFactoryKey(), currentPage, request);

					if (sp.size() > 0)
					{
						str << "<ul style=\"margin:0px 0px 0px 15px\">" << sp << "</ul>";
					}
				}
			}
			return str.str();
		}
	}
}

