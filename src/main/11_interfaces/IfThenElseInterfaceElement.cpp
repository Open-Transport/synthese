
#include "IfThenElseInterfaceElement.h"
#include "ValueElementList.h"
#include "StaticValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void IfThenElseInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const Site* site /*= NULL*/ ) const
		{
			std::string result = _criteria->getValue( parameters );
			return ( result.size() == 0 || result == "0" )
				? _to_do_if_false->display( stream, parameters, object, site )
				: _to_do_if_true->display( stream, parameters, object, site );
		}

		void IfThenElseInterfaceElement::parse( const std::string& text )
		{
			ValueElementList vei( text );

			// Criteria
			_criteria = vei.front();

			// Keyword THEN
			delete vei.front();

			// To do if true
			ValueInterfaceElement* vie = vei.front();
			StaticValueInterfaceElement* svie = dynamic_cast<StaticValueInterfaceElement*>(vie);
			if ( svie != NULL )
			{
				_to_do_if_true = LibraryInterfaceElement::create( svie->getValue( ParametersVector() ) );
				delete vie;
			}
			else
				_to_do_if_true = vie;

			// Keyword ELSE
			delete vei.front();

			// To do if false
			vie = vei.front();
			svie = dynamic_cast<StaticValueInterfaceElement*>(vie);
			if ( svie != NULL )
			{
				_to_do_if_false = LibraryInterfaceElement::create( svie->getValue( ParametersVector() ) );
				delete vie;
			}
			else
				_to_do_if_false = vie;
		}

		IfThenElseInterfaceElement::~IfThenElseInterfaceElement()
		{
			delete _criteria;
			delete _to_do_if_false;
			delete _to_do_if_true;
		}
	}
}
