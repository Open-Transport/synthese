
#ifndef SYNTHESE_SECURITY_RIGHT_H
#define SYNTHESE_SECURITY_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la gestion de la s�curit�.
			@ingroup m15

			Les niveaux de droit utilis�s sont les suivants :

			Habilitations priv�es :
				- USE : Autorise � l'utilisateur � se connecter
				- READ : Permet � l'utilisateur de consulter les propri�t�s de son compte, sauf celles de la cat�gorie "Droits"
				- WRITE : Permet � l'utilisateur de consulter les propri�t�s de son compte, sauf celles de la cat�gorie "Droits"

			Habilitations publiques :
				- READ : Permet de consulter toutes les propri�t�s de tous les comptes
				- WRITE : Permet de modifier toutes les propri�t�s de tous les comptes, d'administrer les profils utilisateurs
				- DELETE : Permet de supprimer un compte, un profil utilisateur

			D�finition du p�rim�tre :
				- Profile : Autorisation portant sur les utilisateurs du profil sp�cifi�
				- NB : Une habilitation sans p�rim�tre reste restreinte � l'ensemble des profils inf�rieurs ou �gaux, au sens de l'op�rateur <= sur les profils.
		*/
		class SecurityRight : public Right
		{
		private:
			static bool _registered;

		};
	}
}

#endif