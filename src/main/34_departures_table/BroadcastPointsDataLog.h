
/** TransportNetworkDataLog class header.
	@file TransportNetworkDataLog.h

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

#ifndef SYNTHESE_TRANSPORT_NETWORK_DATA_LOG
#define SYNTHESE_TRANSPORT_NETWORK_DATA_LOG

#include "13_dblog/DBLog.h"

namespace synthese
{
	namespace departurestable
	{
		/** Journal des modifications de la base transport.
			@ingroup m34

			Les entrées du journal de modification de la base transport sont toutes les modifications effectuées à chaud sur les objets de description du réseau (places, lines, etc.)
				
			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de profil)
				- UID objet concerné
				- Classe objet concerné
				- Description de l'action

		*/
		class BroadcastPointsDataLog : public dblog::DBLog
		{
		public:
			BroadcastPointsDataLog();
			DBLog::ColumnsNameVector getColumnNames() const;
		};
	}
}

#endif

