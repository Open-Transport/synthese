#ifndef SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H

#include "PageComponent.h"


namespace synthese
{
    
namespace interfaces
{
	/** Composant d'administration.
		@ingroup m11
		
		Un composant d'administration est un formulaire param�trable, destin� � effectuer une �criture en temps r�el dans la base SYNTHESE, dans un but de param�trage de l'application (il ne s'agit pas du seul moyen d'�criture).
		Une m�me donn�e peut �tre mise � jour par autant de composants que n�cessaire, d�fini avant tout sur la base d'une ergonomie de qualit�.
		Chaque composant est rattach� au module correspondant aux donn�es � modifier sous forme d'une sous-classe de AdminInterfaceElement. 
		Le comportement des composants d'administration est en g�n�ral d�fini d'apr�s les @ref defRight "habilitations" de l'utilisateur connect�.
		Par exemple :
			- un utilisateur anonyme n'a acc�s � aucun composant d'administration
			- un administrateur a acc�s � tous les composants d'administration.
		
	*/
	class AdminInterfaceElement : public LibraryInterfaceElement
	{
	protected:

		/** Contructor.
		*/
		LibraryInterfaceElement()
			: PageComponent() 
		{ }

	public:

		/** Display method.
			@param stream Stream to write the output in
			@param parameters Parameters vector
			@return Name of the next line to display (empty = next line)
		*/
		std::string display(std::ostream& stream, const ParametersVector& parameters) = 0;
		//static const Right* requiredRight();

	}
}
}

#endif