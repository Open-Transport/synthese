
#ifndef SYNTHESE_ValueInterfaceElement_H__
#define SYNTHESE_ValueInterfaceElement_H__

#include <string>

#include "01_util/Factorable.h"

#include "11_interfaces/DisplayableElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueElementList;

		/** Code defined interface element which produces a string value at runtime.
		*/
		class ValueInterfaceElement : public util::Factorable
		{
		public:
			virtual void		storeParameters(ValueElementList& vel) = 0;
			virtual std::string	getValue( const ParametersVector&, const void* object = NULL, const server::Request* request = NULL) const = 0;
		};
	}
}
#endif // SYNTHESE_ValueInterfaceElement_H__

/** @todo implement and register this
case EI_ATTRIBUT_Ligne_LibelleSimple:
return ( ( const Line* ) __Objet ) ->getShortName ();

case EI_ATTRIBUT_Ligne_Image:
return ( ( const Line* ) __Objet ) ->getImage();

case EI_ATTRIBUT_Ligne_LibelleComplet:
return ( ( const Line* ) __Objet ) ->getLongName ();

case EI_ATTRIBUT_Ligne_Style:
return ( ( const Line* ) __Objet ) ->getStyle();

case EI_ATTRIBUT_Ligne_ArticleMateriel:
return ( ( const Line* ) __Objet ) ->getLongName ();
// MJ TODO return ( ( const Line* ) __Objet ) ->Materiel() ->getArticle();

case EI_ATTRIBUT_Ligne_LibelleMateriel:
return ( ( const Line* ) __Objet ) ->getLongName ();
// MJ TODO return ( ( const Line* ) __Objet ) ->Materiel() ->getLibelleSimple();
*/


