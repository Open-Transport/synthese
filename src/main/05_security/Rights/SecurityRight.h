
#ifndef SYNTHESE_SECURITY_RIGHT_H
#define SYNTHESE_SECURITY_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la gestion de la sécurité.
			@ingroup m15

			Les niveaux de droit utilisés sont les suivants :

			Habilitations privées :
				- USE : Autorise à l'utilisateur à se connecter
				- READ : Permet à l'utilisateur de consulter les propriétés de son compte, sauf celles de la catégorie "Droits"
				- WRITE : Permet à l'utilisateur de consulter les propriétés de son compte, sauf celles de la catégorie "Droits"

			Habilitations publiques :
				- READ : Permet de consulter toutes les propriétés de tous les comptes
				- WRITE : Permet de modifier toutes les propriétés de tous les comptes, d'administrer les profils utilisateurs
				- DELETE : Permet de supprimer un compte, un profil utilisateur

			Définition du périmètre :
				- Profile : Autorisation portant sur les utilisateurs du profil spécifié
				- NB : Une habilitation sans périmètre reste restreinte à l'ensemble des profils inférieurs ou égaux, au sens de l'opérateur <= sur les profils.
		*/
		class SecurityRight : public Right
		{
		private:
			static bool _registered;

		};
	}
}

#endif