
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
			@image latex cap_admin_users_profiles.png "Maquette de l'�cran d'administration des profils utilisateurs" width=14cm

			La <i>Barre de navigation</i> propose deux liens :
				- vers la page d'accueil du module d'administration
				- vers la page de gestion des utilisateurs

			<i>Zone de contenu</i> : L'�cran de gestion des profils utilisateurs est constitu� de trois parties :
				-# Un formulaire de recherche comportant deux crit�res :
					-# Recherche sur le nom
					-# Recherche sur un module d'application des habilitations
				-# Un tableau dont chaque ligne repr�sente un profil utilisateurs, constitu� des colonnes suivantes :
					-# @b Sel : Colonne de s�lection : permet � l'utilisateur de s�lectionner un ou plusieurs profils pour les supprimer ou pour les copier
					-# @b Nom : Nom du profil
					-# @b R�sum� : R�sum� des habilitations associ�es au profil. Ce r�sum� est une liste � puces de chaque habilitation d�crite par :
						-# le niveau de droit
						-# le module couvert
						-# le p�rim�tre de l'habilitation (code SYNTHESE de r�seau/ligne/parcours type/circulation, ou nom de commune ou d'arr�t)
					-# @b Actions : Boutons d'action :
						-# Bouton modifier dans les lignes de profil existant, si le profil est modifiable

			Les profils affich�s sont s�lectionn�s comme suit :
				- Lors du premier affichage de la page, tous les profils sont affich�s
				- En cas de saisie d'un nom dans le formulaire de recherche, seuls les profils dont le nom inclut le texte entr� sont affich�s.
				- En cas de s�lection d'un module dans le formulaire de recherche, seuls les profils comportant au moins une habilitation concernant le module s�lectionn� sont affich�s.

			Un tableau ne peut en aucun cas d�passer 50 profils. Si plus de 50 profils correspondent aux crit�res courants de recherche, alors il est propos� de passer � l'�cran suivant par un lien.

			<i>S�curit�</i>
				- Une habilitation Lecture sur le module S�curit� est n�cessaire pour afficher la page en mode consultation. Tous les boutons sont alors retir�s.
				- Une habilitation Ecriture sur le module S�curit� est n�cessaire pour afficher la page en mode modification.
		*/
		class ProfilesAdmin : public synthese::interfaces::AdminInterfaceElement
		{

		};
	}
}

#endif