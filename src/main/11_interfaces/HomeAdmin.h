
#ifndef SYNTHESE_DISPLAY_HOME_ADMIN_H
#define SYNTHESE_DISPLAY_HOME_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Page d'accueil de la console d'administration.
			@ingroup m34

			@image html cap_admin_home.png
			@image latex cap_admin_home.png "Accueil de la console d'administration" width=14cm
			
			La console d'administration est divis�e en 4 parties :
				- la <i>zone utilisateur</i> situ�e en haut � gauche indique :
					- le nom de l'utilisateur connect�
					- le nom de l'application "SYNTHESE Admin"
					- un bouton @c D�connexion qui d�connecte l'utilisateur
				- la <i>barre de navigation</i> situ�e en haut � droite indique l'emplacement de la page dans l'arborescence des pages de la console d'administration. Les pages situ�es �n niveau sup�rieur � la page courante font l'objet d'un lien qui permet ainsi une navigation ais�e.
				- le <i>volet d'arborescence</i> situ� en bas � gauche pr�sente de mani�re structur�e l'ensemble des pages d'administration int�gr�es. La page en cours est marqu�e sur fond bleu, et les autres pages font l'objet d'un lien qui permet une navigation ais�e.
				- la <i>zone de contenus</i> situ�e en bas � droite est utilis�e par la page d'administration pour afficher son contenus (formulaires, tableaux, etc.)

			Une page d'administration est imprimable grace � une feuille de style d�di�e � l'impression : seule la zone de contenus est alors imprim�e. Si une page d'administration contient des formulaires, les boutons ne sont pas imprim�s.
		*/
		class HomeAdmin : public AdminInterfaceElement
		{
		protected:
			static const std::string _superior;
			static const bool _ever_displayed;

		public:
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const Site* site = NULL) const;
		};
	}
}

#endif