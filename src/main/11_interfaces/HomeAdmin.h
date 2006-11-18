
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
			
			La console d'administration est divisée en 4 parties :
				- la <i>zone utilisateur</i> située en haut à gauche indique :
					- le nom de l'utilisateur connecté
					- le nom de l'application "SYNTHESE Admin"
					- un bouton @c Déconnexion qui déconnecte l'utilisateur
				- la <i>barre de navigation</i> située en haut à droite indique l'emplacement de la page dans l'arborescence des pages de la console d'administration. Les pages situées àn niveau supérieur à la page courante font l'objet d'un lien qui permet ainsi une navigation aisée.
				- le <i>volet d'arborescence</i> situé en bas à gauche présente de manière structurée l'ensemble des pages d'administration intégrées. La page en cours est marquée sur fond bleu, et les autres pages font l'objet d'un lien qui permet une navigation aisée.
				- la <i>zone de contenus</i> située en bas à droite est utilisée par la page d'administration pour afficher son contenus (formulaires, tableaux, etc.)

			Une page d'administration est imprimable grace à une feuille de style dédiée à l'impression : seule la zone de contenus est alors imprimée. Si une page d'administration contient des formulaires, les boutons ne sont pas imprimés.
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