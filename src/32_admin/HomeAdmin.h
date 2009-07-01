
/** HomeAdmin class header.
	@file HomeAdmin.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_DISPLAY_HOME_ADMIN_H
#define SYNTHESE_DISPLAY_HOME_ADMIN_H

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace admin
	{
		/** Page d'accueil de la console d'administration.
			@ingroup m14Admin refAdmin

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
		class HomeAdmin : public AdminInterfaceElementTemplate<HomeAdmin>
		{
		public:
			HomeAdmin();

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(
				const server::ParametersMap& map,
				bool doDisplayPreparationActions = true
			);
			
			
			
			/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
			virtual server::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables) const;


			virtual AdminInterfaceElement::PageLinks getSubPages(
				boost::shared_ptr<const AdminInterfaceElement> currentPage
			) const;

			bool isAuthorized() const;
		};
	}
}

#endif
