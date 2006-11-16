#ifndef SYNTHESE_MESSAGES_LIBRARY_LOG
#define SYNTHESE_MESSAGES_LIBRARY_LOG

namespace synthese
{
	namespace util
	{
		/** Journal concernant la bibliothèque de messages.

			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de modèle de message prioritaire)
				- UID objet concerné : lien vers le message
		*/
		class MessagesLibraryLog : public DBLog
		{
		}
	}
}

#endif
