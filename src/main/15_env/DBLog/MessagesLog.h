#ifndef SYNTHESE_MESSAGES_LOG
#define SYNTHESE_MESSAGES_LOG

namespace synthese
{
	namespace util
	{
		/** Journal des envois de messages.

			Les entrées du journal de messages sont :
				- envois de messages
				- envois selon scénarios de diffusion
				
			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de profil)
				- UID objet concerné : 
					- lien vers le message si envoi de message unique
					- lien vers le scénario si envoi selon un scénario de diffusion

		*/
		class MessagesLog : public DBLog
		{
		}
	}
}

#endif
