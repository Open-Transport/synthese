
#include <vector>

#include "01_util/Conversion.h"
#include "01_util/Exception.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/StaticValueInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"

namespace synthese
{
	using namespace std;
	using namespace util;

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

		size_t ValueElementList::size() const
		{
			return _elements.size();
		}

		void ValueElementList::parse( const std::string& text )
		{
			size_t position = 0;
			std::vector<std::string> elements;

			// Search end of current word ignoring spaces within braces
			size_t wordEndPos;
			size_t numberOfOpenedBraces = 0;
			for (; position < text.size(); position = wordEndPos+1)
			{
				// Purge initial spaces
				for (; position < text.size() && text[position]==' '; ++position );

				if (position >= text.size())
					break;

				numberOfOpenedBraces = 0;

				for (wordEndPos = position;
					wordEndPos < text.size() && (numberOfOpenedBraces != 0 || text[wordEndPos] != ' ');
					++wordEndPos)
				{
					if (text[wordEndPos] == '{')
						++numberOfOpenedBraces;
					if (text[wordEndPos] == '}')
						--numberOfOpenedBraces;
					if (numberOfOpenedBraces < 0)
						throw InterfacePageException("Parse error : too much closing braces at "+ Conversion::ToString(wordEndPos) +" in "+ text);
				}

				elements.push_back(text.substr(position, wordEndPos - position));
			}

			if (numberOfOpenedBraces > 0)
				throw InterfacePageException("Parse error : too much opening braces in "+ text);

			// Registering each word as ValueInterfaceElement
			for (vector<string>::const_iterator it = elements.begin(); it != elements.end(); ++it)
			{
				ValueInterfaceElement* vie = NULL;
				const std::string& str = *it;

				// Case 1 : single word
				if (str.at(0) != '{')
				{
					vie = new StaticValueInterfaceElement(str);
				} 
				
				// Case 2 : multiple word
				else if(str.size() > 1 && str.at(0) == '{' && str.at(1) != '{')
				{
					vie = new StaticValueInterfaceElement(str.substr(1, str.size() - 2));
				}

				// Case 3 : recursive call
				else if(str.size() > 4 && str.substr(0,2) == "{{" && str.substr(str.size() -2, 2) == "}}")
				{
					// Search of the end of the first word
					for (position = 2; position < str.size() - 2 && str.at(position) !=' '; ++position);

					try
					{
						vie = Factory<ValueInterfaceElement>::create(str.substr(2, position - 2));
						ValueElementList vel;
						if (position < str.size() - 3)
							vel.parse(str.substr(position + 1, str.size() - position - 3));
						vie->storeParameters(vel);
					}
					catch(Exception e)
					{
						throw InterfacePageException("Function not found in "+ text);
					}
				}
				else
					throw InterfacePageException("Unspecified parse error in "+ text);

				_elements.push_back(vie);
			}
		}

		ValueElementList::~ValueElementList()
		{
			for (ElementsList::const_iterator it = _elements.begin(); it != _elements.end(); ++it)
				delete *it;
		}

		bool ValueElementList::isEmpty() const
		{
			return _elements.size() == 0;
		}
	}
}


