#ifndef SYNTHESE_SECURITY_LOG
#define SYNTHESE_SECURITY_LOG

namespace synthese
{
	namespace util
	{
		/** Journal de s�curit�.

			Enregistre tous les �v�nements li�s � la s�curit� et � l'administration des utilisateurs.

			Les entr�es du journal de s�curit� sont :
				- actions d'administration sur les utilisateurs
				- actions d'administration sur les profils
				- connexions d'utilisateurs

			Les colonnes additionnelles du journal de s�curit� sont :
				- Action : d�crit l'action effectu�e (ex : cr�ation de profil)
				- UID objet concern� : 
					- lien vers l'utilisateur trait� pour les actions d'administration sur les utilisateurs (diff�rent de l'utilisateur connect�)
					- lien vers le profil trait� pour les actions d'administration sur les profils
					- rien pour les entr�es concernant les connexions d'utilisateurs

		*/
		class SecurityLog : public DBLog
		{
		}
	}
}

#endif
