#ifndef SYNTHESE_MESSAGES_LOG
#define SYNTHESE_MESSAGES_LOG

namespace synthese
{
	namespace util
	{
		/** Journal des envois de messages.

			Les entr�es du journal de messages sont :
				- envois de messages
				- envois selon sc�narios de diffusion
				
			Les colonnes additionnelles du journal de s�curit� sont :
				- Action : d�crit l'action effectu�e (ex : cr�ation de profil)
				- UID objet concern� : 
					- lien vers le message si envoi de message unique
					- lien vers le sc�nario si envoi selon un sc�nario de diffusion

		*/
		class MessagesLog : public DBLog
		{
		}
	}
}

#endif
