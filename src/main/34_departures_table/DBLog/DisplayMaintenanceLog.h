#ifndef SYNTHESE_DISPLAY_MAINTENANCE_LOG
#define SYNTHESE_DISPLAY_MAINTENANCE_LOG

#include "01_util/DBLog.h"

namespace synthese
{
	namespace util
	{
		/** Journal relatif à la maintenance des afficheurs.

			Ce journal contient deux types d'entrée :
				- Les entrées relatives aux manipulations effectuées sur la console d'administration
				- Les entrées issues de remontées d'alarmes par la fonction SendLogEntry issues des clients de supervision des afficheurs.

			Le brassage des deux informations permet d'exploiter directement le journal pour connaître l'état d'un afficheur en observant la dernière entrée de type contrôle.

			Colonnes :
				- UID de l'afficheur concerné
				- Type d'entrée : 
					- contrôle : l'entrée est issue d'une requête de contrôle de bon fonctionnement (une requete sur la dernière entrée connue de ce type donne l'état de l'afficheur, et l'absence d'entrée de ce type depuis un certain délai peut également être interprétée comme une absence de fonctionnement)
					- admin : l'entrée trace une manipulation faite dans la console d'administration
					- state : l'entrée trace un basculement manuel en service / hors service (une requete sur la dernière entrée connue de ce type donne l'état de l'afficheur)
				- Description de l'entrée
				
		*/
		class DisplayMaintenanceLog : public DBLog
		{
		};
	}
}

#endif
