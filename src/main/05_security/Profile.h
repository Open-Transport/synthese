
#ifndef SYNTHESE_SECURITY_PROFILE_H
#define SYNTHESE_SECURITY_PROFILE_H

namespace synthese
{
	namespace security
	{
		/** Profil utilisateur.
			@ingroup m05
		
			Un profil utilisateur est un "modèle" de droits utilisateurs consistant en une liste d'@ref synthese::05_security::Right "habilitations" prédéfinies.

			Exemples de profils utilisateurs :
				- Utilisateur non identifié : peut utiliser les services grand public
				- Utilisateur classique : mêmes droits qu'Anonymous, ainsi que les fonctionnalités de paramétrage utilisateur
				- Utilisateur identifié comme client du système, d'un réseau de transport : mêmes droits qu'User, et peut de plus utiliser la fonction de réservation
				- Conducteur de ligne de transport : même droits qu'User, et peut en outre consulter les listes de réservations
				- Opérateur système : peut gérer les utilisateurs, prendre les réservations, consulter les listes de réservations
				- Responsable de ligne : peut prendre les réservations, consulter les listes de réservations, éditer des messages sur la ligne
				- Administrateur : tous les droits

		*/
		class Profile
		{
		};
	}
}