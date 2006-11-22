
#include "01_util/FactoryException.h"

#include "11_interfaces/LibraryInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

namespace synthese
{
	using namespace util;

	namespace interfaces
	{
		LibraryInterfaceElement* LibraryInterfaceElement::create( const std::string & text )
		{
			// Trim the left spaces
			size_t start_pos;

			for (start_pos = 0; start_pos < text.size() && text[start_pos] == ' '; ++start_pos);

			if (start_pos >= text.size())
				throw InterfacePageException("Parse error : empty line");

			// Search for the end of the keyword
			size_t word_end_pos = start_pos;
			for (; word_end_pos < text.size() && text[word_end_pos] != ' '; ++word_end_pos);

			LibraryInterfaceElement* lie = NULL;
			try
			{
				lie = Factory<LibraryInterfaceElement>::create(text.substr(start_pos, word_end_pos - start_pos));
			}
			catch (FactoryException<LibraryInterfaceElement> e)
			{
				throw InterfacePageException("Specified interface function not found " + text.substr(start_pos, word_end_pos - start_pos));
			}
			lie->parse((text.size() > word_end_pos + 1)
				? text.substr(word_end_pos + 1, text.size() - word_end_pos - 1) 
				: "");

			return lie;
		}

		void LibraryInterfaceElement::parse( const std::string& text )
		{
			ValueElementList vai(text);
			storeParameters(vai);
		}
	}
}
