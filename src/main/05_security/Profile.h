
#ifndef SYNTHESE_SECURITY_PROFILE_H
#define SYNTHESE_SECURITY_PROFILE_H

namespace synthese
{
	namespace security
	{
		/** Profil utilisateur.
			@ingroup m05
		
			Un profil utilisateur est un "mod�le" de droits utilisateurs consistant en une liste d'@ref synthese::05_security::Right "habilitations" pr�d�finies.

			Exemples de profils utilisateurs :
				- Utilisateur non identifi� : peut utiliser les services grand public
				- Utilisateur classique : m�mes droits qu'Anonymous, ainsi que les fonctionnalit�s de param�trage utilisateur
				- Utilisateur identifi� comme client du syst�me, d'un r�seau de transport : m�mes droits qu'User, et peut de plus utiliser la fonction de r�servation
				- Conducteur de ligne de transport : m�me droits qu'User, et peut en outre consulter les listes de r�servations
				- Op�rateur syst�me : peut g�rer les utilisateurs, prendre les r�servations, consulter les listes de r�servations
				- Responsable de ligne : peut prendre les r�servations, consulter les listes de r�servations, �diter des messages sur la ligne
				- Administrateur : tous les droits

		*/
		class Profile
		{
		};
	}
}