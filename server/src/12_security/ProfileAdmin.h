
/** ProfileAdmin class header.
	@file ProfileAdmin.h

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

#ifndef SYNTHESE_SECURITY_PROFILE_ADMIN_H
#define SYNTHESE_SECURITY_PROFILE_ADMIN_H

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Ecran d'édition de profil.
			@ingroup m12Admin refAdmin

			@image html cap_admin_profile.png
			@image latex cap_admin_profile.png "Maquette d'écran d'édition de profil" width=14cm

			<i>Barre de navigation</i> :
				- Lien vers synthese::admin::HomeAdmin
				- Lien vers synthese::security::ProfilesAdmin
				- Nom du profil

			<i>Zone de contenus</i> :
				-# Champ de <b>modification du nom</b>, accompagné d'un bouton <tt>Supprimer</tt>. Champ obligatoire. La tentative d'enregistrement d'un nom vide produit le message d'erreur suivant :
					@code Un profil doit posséder un nom @endcode
					Le nom doit être unique. En cas de doublon, la modification du nom est refusée et le message d'erreur suivant s'affiche :
					@code Un profil portant le même nom existe déjà @endcode
				-# <b>Tableau des habilitations</b> : représente chaque habilitation définissant le profil, par les colonnes suivantes :
					-# <tt>Module</tt> : Module sur lequel porte l'habilitation (Tous = tous les modules à la fois).
					-# <tt>Opération</tt> : Opération du module sur laquelle porte l'habilitation (Tous = toutes les opérations du modules, y compris administration)
					-# <tt>Périmètre</tt> : Texte représentant le périmètre sur lequel porte l'habilitation, dont le formalisme dépend du module et de l'opération. Un caractère * signifie "peut être remplacé par tout texte".
					-# <tt>Droit</tt> : Droit obtenu pour le module, l'opération, et le périmètre de l'habilitation
					-# <tt>Actions</tt> : contient un bouton supprimer pour chaque habilitation, permettant de supprimer la ligne du tableau. L'appui sur le bouton entraine une confirmation par boite de dialogue standard. La suppression est ensuite définitive.
				-# <b>Formulaire d'ajout d'habilitation</b> :
					-# <tt>Module</tt> : Liste de choix contenant tous les modules de SYNTHESE, plus un choix <tt>Tous</tt> placé en tête et proposé par défaut. Le changement de valeur sur la liste entraine le rafraichissement de la page pour mettre à jour la liste suivante.
					-# <tt>Opération</tt> : Liste des opérations disponibles pour le module sélectionné, plus un choix <tt>Toutes</tt> placé en tête et proposé par défaut. Le changement de valeur sur la liste entraîne le rafraichissement de la page pour mettre à jour la liste suivante.
					-# <tt>Périmètre</tt> : Liste des périmètres possibles pour l'opération sélectionnées, plus un choix <tt>Tout</tt> placé en tête et proposé par défaut.
					-# <tt>Droit</tt> : Liste des niveaux de droit possibles :
						- Utilisation
						- Lecture
						- Modification
						- Ecriture
						- Suppression
					-# <tt>Ajouter</tt> : bouton d'ajout d'habilitation. L'ajout se fait directement sans confirmation.

			<i>Sécurité</i>
				- Une habilitation SecurityRights de niveau public WRITE est nécessaire pour accéder à la page, pour laquelle le profil demandé est inférieur ou égal.

			<i>Journaux</i> : Les opérations suivantes sont consignées dans le journal de sécurité SecurityLog :
				- INFO : Modification du nom du profil
				- INFO : Ajout d'habilitation au profil
				- INFO : Suppression d'habilitation au profil
		*/
		class ProfileAdmin : public admin::AdminInterfaceElementTemplate<ProfileAdmin>
		{
			static const std::string PARAM_PROFILE_ID;

			boost::shared_ptr<const Profile> _profile;
			bool _profileError;

		public:
			ProfileAdmin();

			void setProfile(boost::shared_ptr<Profile> value);
			boost::shared_ptr<const Profile> getProfile() const;

			/** Initialization of the parameters from a request.
			@param request The request to use for the initialization.
			*/
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

			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;


			bool isAuthorized(
				const security::User& user
			) const;

			virtual std::string getTitle() const;


			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

		};
	}
}

#endif // SYNTHESE_SECURITY_PROFILE_ADMIN_H
