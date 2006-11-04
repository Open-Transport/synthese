#ifndef SYNTHESE_TRANSPORT_NETWORK_DATA_LOG
#define SYNTHESE_TRANSPORT_NETWORK_DATA_LOG

namespace synthese
{
	namespace util
	{
		/** Journal des modifications de la base transport.

			Les entr�es du journal de modification de la base transport sont toutes les modifications effectu�es � chaud sur les objets de description du r�seau (places, lines, etc.)
				
			Les colonnes additionnelles du journal de s�curit� sont :
				- Action : d�crit l'action effectu�e (ex : cr�ation de profil)
				- UID objet concern�
				- Classe objet concern�
				- Description de l'action

		*/
		class TransportNetworkDataLog : public DBLog
		{
		}
	}
}

#endif
