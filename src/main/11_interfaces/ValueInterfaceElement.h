
#ifndef SYNTHESE_ValueInterfaceElement_H__
#define SYNTHESE_ValueInterfaceElement_H__


#include "11_interfaces/LibraryInterfaceElement.h"
#include "01_util/Factory.h"
#include <string>

using synthese::util::Factory;

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement : public LibraryInterfaceElement
		{
		public:
			virtual const std::string& getValue( const ParametersVector&, const void* object = NULL, const Site* site = NULL ) const = 0;
			void display( std::ostream&, const ParametersVector&, const void* object = NULL, const Site* site = NULL ) const;
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