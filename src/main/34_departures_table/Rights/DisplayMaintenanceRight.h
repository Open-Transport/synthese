
#ifndef SYNTHESE_DISPLAY_MAINTENANCE_RIGHT_H
#define SYNTHESE_DISPLAY_MAINTENANCE_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la maintenance des afficheurs.
			@ingroup m34

			Les niveaux de droit utilisés sont les suivants :
				- READ : Permet de consulter les états de maintenance des afficheurs
				- WRITE : Permet de modifier les états de maintenance des afficheurs

			Définition du périmètre :
				- TransportNetwork : Autorisation portant sur les afficheurs situés sur un point de diffusion desservi par une ligne du réseau spécifié
				- Line : Autorisation portant sur les afficheurs situés sur un arrêt desservi par la ligne spécifiée
				- Place : Autorisation portant sur les afficheurs situés sur un arrêt appartenant au lieu logique spécifié
				- BroadcastPoint : Autorisation portant sur les afficheurs situés sur le point de diffusion spécifié
				- NB : la notion de situation d'un afficheur est définie de deux manières :
					- si le point de diffusion est un arrêt physique, alors il doit être desservi par la ligne
					- si le point de diffusion n'est pas un arrêt physique, alors au moins un arrêt physique du lieu auquel appartient le point de diffusion doit être desservi par la ligne
		*/
		class DisplayMaintenanceRight : public Right
		{

		};
	}
}

#endif