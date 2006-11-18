#ifndef SYNTHESE_SECURITY_LOG
#define SYNTHESE_SECURITY_LOG

namespace synthese
{
	namespace util
	{
		/** Journal de sécurité.

			Enregistre tous les événements liés à la sécurité et à l'administration des utilisateurs.

			Les entrées du journal de sécurité sont :
				- actions d'administration sur les utilisateurs
				- actions d'administration sur les profils
				- connexions d'utilisateurs

			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de profil)
				- UID objet concerné : 
					- lien vers l'utilisateur traité pour les actions d'administration sur les utilisateurs (différent de l'utilisateur connecté)
					- lien vers le profil traité pour les actions d'administration sur les profils
					- rien pour les entrées concernant les connexions d'utilisateurs

		*/
		class SecurityLog : public DBLog
		{
		}
	}
}

#endif
