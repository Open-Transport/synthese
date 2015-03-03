
/** MessagesRight class header.
	@file MessagesRight.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_MESSAGES_RIGHT_H
#define SYNTHESE_MESSAGES_RIGHT_H

#include "12_security/RightTemplate.h"

namespace synthese
{
	namespace messages
	{
		/** Habilitation portant sur la gestion de messages.
			@ingroup m17

			Les niveaux de droit privés utilisés sont les suivants :
				- READ : Utilisation de la console d'administration des messages en mode lecture, recherche possible. Seuls les messages créés par l'utilisateur sont affichés
				- WRITE : Permet de déclarer de nouveaux messages et de les modifier par la suite. La modification ainsi que la lecture des messages existants n'est permise que pour les messages créés par l'utilisateur.

			Les niveaux de droit public utilisés sont les suivants :
				- READ : Utilisation de la console d'administration des messages en mode lecture, recherche possible.
				- WRITE : Permet d'éditer des messages


			Définition du périmètre :
				- MessagesSection : Autorisation portant sur les messages activés pour une section en particulier
				- TransportNetwork : Autorisation portant sur les messages diffusés sur un point de diffusion desservi par une ligne du réseau spécifié
				- JourneyPattern : Autorisation portant sur les messages diffusés sur un arrêt desservi par la ligne spécifiée
				- Place : Autorisation portant sur les messages diffusés sur un arrêt appartenant au lieu logique spécifié
				- BroadcastPoint : Autorisation portant sur les messages diffusés sur le point de diffusion spécifié
				- NB : le périmètre est construit par ajout : si un message est diffusé par un arrêt desservi par une ligne autorisée alors il entre dans le champ de l'habilitation, même s'il est diffusé sur un autre arrêt dont la ligne n'y figure pas.
				- NB : la notion de desserte d'un point de diffusion par une ligne est définie de deux manières :
					- si le point de diffusion est un arrêt physique, alors il doit être desservi par la ligne
					- si le point de diffusion n'est pas un arrêt physique, alors au moins un arrêt physique du lieu auquel appartient le point de diffusion doit être desservi par la ligne

			Cas d'utilisation :
				- Permission de lire tous les messages, d'en créer, mais interdiction de modifier ceux créés par les autres utilisateurs : créer une READ et une WRITE/belong
				- Permission de lire tous les messages diffusés sur un arrêt desservi par les lignes 12 et 14, permission de créer un message sur la 12 uniquement : créer une WRITE/belong sur la ligne 12, READ sur la ligne 12, READ sur la ligne 14
				- Permission de lire tous les messages diffusés sur un réseau, permission de créer un message sur la ligne 2, permission de créer un message et d'éditer ceux des autres utilisateurs sur la ligne 8 : créer un READ sur le réseau, un WRITE/belong sur la ligne 2, et un WRITE sur la ligne 8
		*/
		class MessagesRight : public security::RightTemplate<MessagesRight>
		{
		public:
			static const std::string MESSAGES_SECTION_FACTORY_KEY;



			std::string displayParameter(
				util::Env& env = util::Env::GetOfficialEnv()
			) const;
			bool perimeterIncludes(
				const std::string& perimeter,
				util::Env& env = util::Env::GetOfficialEnv()
			) const;
		};
	}
}

#endif
