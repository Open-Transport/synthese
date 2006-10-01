
#ifndef SYNTHESE_PROFILES_ADMIN_H
#define SYNTHESE_PROFILES_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran de gestion des profils utilisateurs.
			@ingroup m34

			@image html cap_admin_users_profiles.png
			@image latex cap_admin_users_profiles.png "Maquette de l'écran d'administration des profils utilisateurs" width=14cm

			La <i>Barre de navigation</i> propose deux liens :
				- vers la page d'accueil du module d'administration
				- vers la page de gestion des utilisateurs

			<i>Zone de contenu</i> : L'écran de gestion des profils utilisateurs est constitué de trois parties :
				-# Un formulaire de recherche comportant deux critères :
					-# Recherche sur le nom
					-# Recherche sur un module d'application des habilitations
				-# Un tableau dont chaque ligne représente un profil utilisateurs, constitué des colonnes suivantes :
					-# @b Sel : Colonne de sélection : permet à l'utilisateur de sélectionner un ou plusieurs profils pour les supprimer ou pour les copier
					-# @b Nom : Nom du profil
					-# @b Résumé : Résumé des habilitations associées au profil. Ce résumé est une liste à puces de chaque habilitation décrite par :
						-# le niveau de droit
						-# le module couvert
						-# le périmètre de l'habilitation (code SYNTHESE de réseau/ligne/parcours type/circulation, ou nom de commune ou d'arrêt)
					-# @b Actions : Boutons d'action :
						-# Bouton modifier dans les lignes de profil existant, si le profil est modifiable

			Les profils affichés sont sélectionnés comme suit :
				- Lors du premier affichage de la page, tous les profils sont affichés
				- En cas de saisie d'un nom dans le formulaire de recherche, seuls les profils dont le nom inclut le texte entré sont affichés.
				- En cas de sélection d'un module dans le formulaire de recherche, seuls les profils comportant au moins une habilitation concernant le module sélectionné sont affichés.

			Un tableau ne peut en aucun cas dépasser 50 profils. Si plus de 50 profils correspondent aux critères courants de recherche, alors il est proposé de passer à l'écran suivant par un lien.

			<i>Sécurité</i>
				- Une habilitation Lecture sur le module Sécurité est nécessaire pour afficher la page en mode consultation. Tous les boutons sont alors retirés.
				- Une habilitation Ecriture sur le module Sécurité est nécessaire pour afficher la page en mode modification.
		*/
		class ProfilesAdmin : public synthese::interfaces::AdminInterfaceElement
		{

		};
	}
}

#endif