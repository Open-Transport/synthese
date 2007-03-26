
/** MessagesLog class header.
	@file MessagesLog.h

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

#ifndef SYNTHESE_MESSAGES_LOG
#define SYNTHESE_MESSAGES_LOG

#include "13_dblog/DBLog.h"

namespace synthese
{
	namespace messages
	{
		/** Journal des envois de messages.

			Les entrées du journal de messages sont :
				- envois de messages
				- envois selon scénarios de diffusion
				
			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de profil)
				- UID objet concerné : 
					- lien vers le message si envoi de message unique
					- lien vers le scénario si envoi selon un scénario de diffusion

		*/
		class MessagesLog : public dblog::DBLog
		{
		public:
			MessagesLog();
			DBLog::ColumnsVector getColumnNames() const;
		};
	}
}

#endif

