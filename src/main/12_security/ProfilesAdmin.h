
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
					-# bouton <tt>Modifier</tt> dans les lignes de profil existant, si le profil est modifiable
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
		class ProfilesAdmin : public synthese::interfaces::AdminInterfaceElement
		{

		};
	}
}

#endif