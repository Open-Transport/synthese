
/** ArrivalDepartureTableRight class header.
	@file ArrivalDepartureTableRight.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_ARRIVAL_DEPARTURE_TABLE_RIGHT_H
#define SYNTHESE_ARRIVAL_DEPARTURE_TABLE_RIGHT_H

#include "12_security/Right.h"

namespace synthese
{
	namespace departurestable
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
		class ArrivalDepartureTableRight : public security::Right
		{
		public:
			ArrivalDepartureTableRight();
			std::string displayParameter() const;
			ParameterLabelsVector	getParametersLabels()	const;
			bool perimeterIncludes(const std::string& perimeter) const;
		};
	}
}

#endif
