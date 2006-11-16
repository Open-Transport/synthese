#ifndef SYNTHESE_TRANSPORT_NETWORK_DATA_LOG
#define SYNTHESE_TRANSPORT_NETWORK_DATA_LOG

namespace synthese
{
	namespace util
	{
		/** Journal des modifications de la base transport.

			Les entrées du journal de modification de la base transport sont toutes les modifications effectuées à chaud sur les objets de description du réseau (places, lines, etc.)
				
			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de profil)
				- UID objet concerné
				- Classe objet concerné
				- Description de l'action

		*/
		class TransportNetworkDataLog : public DBLog
		{
		}
	}
}

#endif
