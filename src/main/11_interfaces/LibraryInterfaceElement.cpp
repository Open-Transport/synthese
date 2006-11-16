
#include "LibraryInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		LibraryInterfaceElement* LibraryInterfaceElement::create( const std::string & text )
		{
			// Trim the left spaces
			size_t start_pos;

			for (start_pos = 0; start_pos < text.size() && text[start_pos] != ' '; ++start_pos);

			if (start_pos < text.size())
			{
				// Search for the end of the keyword
				size_t word_end_pos = start_pos;
				for (; word_end_pos < text.size() && text[word_end_pos] != ' '; ++word_end_pos);

				LibraryInterfaceElement* lie = Factory<LibraryInterfaceElement>::create(text.substr(start_pos, word_end_pos - start_pos));
				lie->parse( text.substr(word_end_pos + 1, text.size() - word_end_pos - 1) );

				return lie;
			}
			return NULL;
		}
	}
}
