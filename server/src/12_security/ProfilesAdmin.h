
/** ProfilesAdmin class header.
	@file ProfilesAdmin.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_PROFILES_ADMIN_H
#define SYNTHESE_PROFILES_ADMIN_H

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Ecran de gestion des profils utilisateurs.
			@ingroup m12Admin refAdmin

			@image html cap_admin_users_profiles.png
			@image latex cap_admin_users_profiles.png "Maquette de l'écran d'administration des profils utilisateurs" width=14cm

			La <i>Barre de navigation</i> propose deux liens :
				- vers la page d'accueil du module d'administration
				- vers la page de gestion des utilisateurs

			<i>Zone de contenu</i> : L'écran de gestion des profils utilisateurs est constitué de trois parties :
				-# <b>formulaire de recherche</b> comportant deux critères :
					-# Recherche sur le nom
					-# Recherche sur une habilitation : seuls les profils comportant une habilitation de la classe spécifiée ou héritant d'un tel profil sont affichées.
				-# <b>Tableau des résultats</b> dont chaque ligne représente un profil utilisateurs, constitué des colonnes suivantes :
					-# <tt>Sel</tt> : Colonne de sélection : permet à l'utilisateur de sélectionner un ou plusieurs profils pour les supprimer ou pour les copier
					-# <tt>Nom</tt> : Nom du profil
					-# <tt>Résumé</tt> : Résumé des habilitations associées au profil. Ce résumé est une liste à puces :
						-# le profil parent
						-# chaque habilitation, décrite par :
							-# le niveau de droit
							-# le statut public/privé
							-# la désignation de l'habilitation
							-# le périmètre de l'habilitation (selon spécificités de l'habilitation)
					-# bouton <tt>OuvrirModifier</tt> dans les lignes de profil existant, si le profil est modifiable
					-# bouton <tt>Ajouter</tt> : permet de créer un nouveau profil. Un nom non vide doit être entré.
				-# <b>Bouton <tt>Supprimer</tt></b> : permet de supprimer les profils sélectionnés, uniquement si ceux-ci ne sont pas utilisés

			Les profils affichés sont sélectionnés comme suit :
				- Lors du premier affichage de la page, tous les profils sont affichés
				- En cas de saisie d'un nom dans le formulaire de recherche, seuls les profils dont le nom inclut le texte entré sont affichés.
				- En cas de sélection d'un module dans le formulaire de recherche, seuls les profils comportant au moins une habilitation concernant le module sélectionné sont affichés.

			Un tableau ne peut en aucun cas dépasser 50 profils. Si plus de 50 profils correspondent aux critères courants de recherche, alors il est proposé de passer à l'écran suivant par un lien.

			<i>Sécurité</i> :
				- Une habilitation SecurityRights de niveau public WRITE est nécessaire pour accéder à la page. Le résultat de la recherche dépend du profil de l'habilitation : seuls les profils inférieurs ou égaux au profil de l'habilitation sont affichés.
				- Une habilitation SecurityRights de niveau public DELETE est nécessaire pour pouvoir supprimer un profil. Seuls les profils inutilisés inférieurs ou égaux au profil de l'habilitation peuvent être supprimés.

			<i>Journaux</i> : Les événements suivants entrainent la création d'une entrée dans le journal de sécurité :
				- INFO : Création de profil
				- INFO : Suppression de profil
				- WARNING : Tentative de suppression de profil utilisé
		*/
		class ProfilesAdmin : public admin::AdminInterfaceElementTemplate<ProfilesAdmin>
		{
			static const std::string PARAMETER_SEARCH_NAME;
			static const std::string PARAMETER_SEARCH_RIGHT;

			std::string _searchName;
			std::string _searchRightName;

			html::ResultHTMLTable::RequestParameters	_requestParameters;

		public:
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
				*/
			virtual util::ParametersMap getParametersMap() const;



			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;

			bool isAuthorized(
				const security::User& user
			) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@return PageLinks A link to the page if the parent is Users
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;
		};
	}
}

#endif
