
#include "ValueElementList.h"

namespace synthese
{
	namespace interfaces
	{
		/** Empty constructor.		
		*/
		ValueElementList::ValueElementList(const std::string text)
		{
			parse(text);
		}

		ValueElementList::ValueElementList( ValueElementList& vai )
		{
			for (; vai._elements.size() > 0; vai._elements.pop_back())
				_elements.push_front(vai._elements.back());
		}

		const ParametersVector ValueElementList::fillParameters( const ParametersVector& parameters ) const
		{
			ParametersVector pv;
			for (ElementsList::const_iterator it = _elements.begin(); it != _elements.end(); ++it)
			{
				pv.push_back((*it)->getValue(parameters));
			}
			return pv;
		}

		ValueInterfaceElement* ValueElementList::front()
		{
			if (_elements.size() > 0)
			{
				ValueInterfaceElement* vie = _elements.front();
				_elements.pop_front();
				return vie;
			}
			else
				return NULL;
		}


		/** Parsing a string for building the element list.
		@param text Text to parse. Must be on one line without the <tt>\n</tt> character.

		The string must be made up of a suit of elements separated by spaces. The elements are made up of :
		- a keyword corresponding to a registered ValueInterfaceElement subclass key
		- the : character
		- the parameter of the subclass in accordance to the parsing rules of the selected subclass. If the parameter is text containing spaces, it must be enclosed in braces

		Examples :
		date:today,ymd
		text:ok
		text:{it's ok}
		param:10

		*/
		void ValueElementList::parse( const std::string& text )
		{
			for ( size_t position = 0; position < text.size (); ++position )
			{
				if (text[position] == ' ')
					continue;

				size_t dot_pos;
				for ( dot_pos = position + 1;
					dot_pos < text.size() && text[dot_pos] != ':' && text[dot_pos] != ' ';
					++dot_pos );

				for (Factory<ValueInterfaceElement>::Iterator it = Factory<ValueInterfaceElement>::begin();
					it != Factory<ValueInterfaceElement>::end(); ++it)
				{
					if (text.substr( position, dot_pos - position ) == it.getKey() )
					{
						ValueInterfaceElement* vie = it.getObject();
						size_t end_pos = dot_pos + 1;

						if (dot_pos < text.size() - 1 && text[dot_pos] == ':')
						{
							if ( text[dot_pos]+1 == '{' && dot_pos < text.size() - 2)
							{
								for (; end_pos < text.size() && text[end_pos] != '}'; ++end_pos );

								vie->parse( text.substr(dot_pos + 2, end_pos - dot_pos - 2) );
							}
							else
							{
								for (; end_pos < text.size() && text[end_pos] != ' '; ++end_pos );

								vie->parse( text.substr(dot_pos + 1, end_pos - dot_pos - 1) );
							}
						}
						_elements.push_back(vie);
						position += end_pos - 1;
					}
				}
			}

		}

		ValueElementList::~ValueElementList()
		{
			for (ElementsList::const_iterator it = _elements.begin(); it != _elements.end(); ++it)
				delete *it;
		}
	}
}

