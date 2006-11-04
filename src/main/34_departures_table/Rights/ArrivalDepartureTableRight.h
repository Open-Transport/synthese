
#ifndef SYNTHESE_ARRIVAL_DEPARTURE_TABLE_RIGHT_H
#define SYNTHESE_ARRIVAL_DEPARTURE_TABLE_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur le téléaffichage.
			@ingroup m34

			Les niveaux de droit utilisés sont les suivants :
				- USE : Parmet de visualiser un tableau de départ ou d'arrivée
				- READ : Permet de consulter les propriétés des objets relatifs au téléaffichage
				- WRITE : Permet de modifier les propriétés des objets relatifs au téléaffichage
				- DELETE : Permet de supprimer des objets relatifs au téléaffichage

			Définition du périmètre :
				- TransportNetwork : Autorisation portant sur les afficheurs situés sur un point de diffusion desservi par une ligne du réseau spécifié
				- Line : Autorisation portant sur les afficheurs situés sur un arrêt desservi par la ligne spécifiée
				- Place : Autorisation portant sur les afficheurs situés sur un arrêt appartenant au lieu logique spécifié
				- BroadcastPoint : Autorisation portant sur les afficheurs situés sur le point de diffusion spécifié
				- NB : la notion de situation d'un afficheur est définie de deux manières :
					- si le point de diffusion est un arrêt physique, alors il doit être desservi par la ligne
					- si le point de diffusion n'est pas un arrêt physique, alors au moins un arrêt physique du lieu auquel appartient le point de diffusion doit être desservi par la ligne
		*/
		class ArrivalDepartureTableRight : public Right
		{

		};
	}
}

#endif