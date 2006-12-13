
#ifndef SYNTHESE_MESSAGES_RIGHT_H
#define SYNTHESE_MESSAGES_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la gestion de messages.
			@ingroup m15

			Les niveaux de droit priv�s utilis�s sont les suivants :
				- READ : Utilisation de la console d'administration des messages en mode lecture, recherche possible. Seuls les messages cr��s par l'utilisateur sont affich�s
				- WRITE : Permet de d�clarer de nouveaux messages et de les modifier par la suite. La modification ainsi que la lecture des messages existants n'est permise que pour les messages cr��s par l'utilisateur.

			Les niveaux de droit public utilis�s sont les suivants :
				- READ : Utilisation de la console d'administration des messages en mode lecture, recherche possible.
				- WRITE : Permet d'�diter des messages
				

			D�finition du p�rim�tre :
				- TransportNetwork : Autorisation portant sur les messages diffus�s sur un point de diffusion desservi par une ligne du r�seau sp�cifi�
				- Line : Autorisation portant sur les messages diffus�s sur un arr�t desservi par la ligne sp�cifi�e
				- Place : Autorisation portant sur les messages diffus�s sur un arr�t appartenant au lieu logique sp�cifi�
				- BroadcastPoint : Autorisation portant sur les messages diffus�s sur le point de diffusion sp�cifi�
				- NB : le p�rim�tre est construit par ajout : si un message est diffus� par un arr�t desservi par une ligne autoris�e alors il entre dans le champ de l'habilitation, m�me s'il est diffus� sur un autre arr�t dont la ligne n'y figure pas.
				- NB : la notion de desserte d'un point de diffusion par une ligne est d�finie de deux mani�res :
					- si le point de diffusion est un arr�t physique, alors il doit �tre desservi par la ligne
					- si le point de diffusion n'est pas un arr�t physique, alors au moins un arr�t physique du lieu auquel appartient le point de diffusion doit �tre desservi par la ligne

			Cas d'utilisation :
				- Permission de lire tous les messages, d'en cr�er, mais interdiction de modifier ceux cr��s par les autres utilisateurs : cr�er une READ et une WRITE/belong
				- Permission de lire tous les messages diffus�s sur un arr�t desservi par les lignes 12 et 14, permission de cr�er un message sur la 12 uniquement : cr�er une WRITE/belong sur la ligne 12, READ sur la ligne 12, READ sur la ligne 14
				- Permission de lire tous les messages diffus�s sur un r�seau, permission de cr�er un message sur la ligne 2, permission de cr�er un message et d'�diter ceux des autres utilisateurs sur la ligne 8 : cr�er un READ sur le r�seau, un WRITE/belong sur la ligne 2, et un WRITE sur la ligne 8
		*/
		class MessagesRight : public Right
		{

		};
	}
}

#endif
