#ifndef SYNTHESE_MESSAGES_LIBRARY_LOG
#define SYNTHESE_MESSAGES_LIBRARY_LOG

namespace synthese
{
	namespace util
	{
		/** Journal concernant la biblioth�que de messages.

			Les colonnes additionnelles du journal de s�curit� sont :
				- Action : d�crit l'action effectu�e (ex : cr�ation de mod�le de message prioritaire)
				- UID objet concern� : lien vers le message
		*/
		class MessagesLibraryLog : public DBLog
		{
		}
	}
}

#endif
