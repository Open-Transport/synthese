#ifndef SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H

#include "DisplayableElement.h"
#include "01_util/Factory.h"
#include <string>

using synthese::util::Factory;

namespace synthese
{
	    
	namespace interfaces
	{
		/** Composant d'administration.
			@ingroup m11
			
			Un composant d'administration est un formulaire paramétrable, destiné à effectuer une écriture en temps réel dans la base SYNTHESE, dans un but de paramétrage de l'application (il ne s'agit pas du seul moyen d'écriture).
			Une même donnée peut être mise à jour par autant de composants que nécessaire, défini avant tout sur la base d'une ergonomie de qualité.
			Chaque composant est rattaché au module correspondant aux données à modifier sous forme d'une sous-classe de AdminInterfaceElement. 
			Le comportement des composants d'administration est en général défini d'après les @ref defRight "habilitations" de l'utilisateur connecté.
			Par exemple :
				- un utilisateur anonyme n'a accès à aucun composant d'administration
				- un administrateur a accès à tous les composants d'administration.
			
		*/
		class AdminInterfaceElement : public DisplayableElement
		{
		private:
			static const bool _registered;

		protected:

			/** Constructor.
			*/
			AdminInterfaceElement();

		public:

			//static const Right* requiredRight();

		};
	}
}

#endif