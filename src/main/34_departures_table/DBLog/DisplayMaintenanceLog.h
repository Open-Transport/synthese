#ifndef SYNTHESE_DISPLAY_MAINTENANCE_LOG
#define SYNTHESE_DISPLAY_MAINTENANCE_LOG

#include "01_util/DBLog.h"

namespace synthese
{
	namespace util
	{
		/** Journal relatif � la maintenance des afficheurs.

			Ce journal contient deux types d'entr�e :
				- Les entr�es relatives aux manipulations effectu�es sur la console d'administration
				- Les entr�es issues de remont�es d'alarmes par la fonction SendLogEntry issues des clients de supervision des afficheurs.

			Le brassage des deux informations permet d'exploiter directement le journal pour conna�tre l'�tat d'un afficheur en observant la derni�re entr�e de type contr�le.

			Colonnes :
				- UID de l'afficheur concern�
				- Type d'entr�e : 
					- contr�le : l'entr�e est issue d'une requ�te de contr�le de bon fonctionnement (une requete sur la derni�re entr�e connue de ce type donne l'�tat de l'afficheur, et l'absence d'entr�e de ce type depuis un certain d�lai peut �galement �tre interpr�t�e comme une absence de fonctionnement)
					- admin : l'entr�e trace une manipulation faite dans la console d'administration
					- state : l'entr�e trace un basculement manuel en service / hors service (une requete sur la derni�re entr�e connue de ce type donne l'�tat de l'afficheur)
				- Description de l'entr�e
				
		*/
		class DisplayMaintenanceLog : public DBLog
		{
		};
	}
}

#endif
