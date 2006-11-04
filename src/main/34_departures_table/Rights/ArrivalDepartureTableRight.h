
#ifndef SYNTHESE_ARRIVAL_DEPARTURE_TABLE_RIGHT_H
#define SYNTHESE_ARRIVAL_DEPARTURE_TABLE_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur le t�l�affichage.
			@ingroup m34

			Les niveaux de droit utilis�s sont les suivants :
				- USE : Parmet de visualiser un tableau de d�part ou d'arriv�e
				- READ : Permet de consulter les propri�t�s des objets relatifs au t�l�affichage
				- WRITE : Permet de modifier les propri�t�s des objets relatifs au t�l�affichage
				- DELETE : Permet de supprimer des objets relatifs au t�l�affichage

			D�finition du p�rim�tre :
				- TransportNetwork : Autorisation portant sur les afficheurs situ�s sur un point de diffusion desservi par une ligne du r�seau sp�cifi�
				- Line : Autorisation portant sur les afficheurs situ�s sur un arr�t desservi par la ligne sp�cifi�e
				- Place : Autorisation portant sur les afficheurs situ�s sur un arr�t appartenant au lieu logique sp�cifi�
				- BroadcastPoint : Autorisation portant sur les afficheurs situ�s sur le point de diffusion sp�cifi�
				- NB : la notion de situation d'un afficheur est d�finie de deux mani�res :
					- si le point de diffusion est un arr�t physique, alors il doit �tre desservi par la ligne
					- si le point de diffusion n'est pas un arr�t physique, alors au moins un arr�t physique du lieu auquel appartient le point de diffusion doit �tre desservi par la ligne
		*/
		class ArrivalDepartureTableRight : public Right
		{

		};
	}
}

#endif