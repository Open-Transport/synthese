
#include "01_util/Log.h"

#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/LineLabelInterfaceElement.h"
#include "11_interfaces/CommentInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"

namespace synthese
{
	using namespace util;

	namespace interfaces
	{

		void InterfacePage::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request) const
		{
			std::string label_to_go = "";
			for (PageComponentsVector::const_iterator it = _components.begin(); it != _components.end(); ++it)
			{
				if (label_to_go == "" || it->first == label_to_go)
				{
					it->second->display(stream, parameters, object, request);
					const LineLabelInterfaceElement* llie = dynamic_cast<const LineLabelInterfaceElement*>(it->second);
					label_to_go = (llie == NULL) ? "" : llie->getLabel();
				}
			}
		}

		void InterfacePage::clear()
		{
			for ( PageComponentsVector::iterator it = _components.begin(); it != _components.end(); ++it )
				delete it->second;
			_components.clear();
		}

		void InterfacePage::parse( const std::string& text )
		{
			size_t start_pos;
			size_t end_pos;
			std::string last_label = "";
			for (start_pos = 0; start_pos < text.size(); start_pos = end_pos + 1)
			{
				for (end_pos = start_pos; end_pos < text.size() && text[end_pos] != '\n'; ++end_pos);

				std::string line = text.substr(start_pos, end_pos - start_pos);
				LibraryInterfaceElement* lie;
				try
				{
					lie = LibraryInterfaceElement::create(line);
				}
				// Jump interface elements with parse errors
				catch (InterfacePageException e)
				{
					Log::GetInstance().warn("Interface page parsing error on " + line + "\n", e);
					continue;
				}
				// Handle line labels
				LineLabelInterfaceElement* llie = dynamic_cast<LineLabelInterfaceElement*>(lie);
				if ( llie != NULL )
				{
					last_label = llie->getLabel();
					delete lie;
					continue;
				}
				// Jump over a comment
				if (dynamic_cast<CommentInterfaceElement*>(lie) != NULL)
				{
					delete lie;
					continue;
				}
				// Store other types of interface elements
				_components.push_back( make_pair( last_label, lie ));
				last_label = "";
			}
		}
	}
}
