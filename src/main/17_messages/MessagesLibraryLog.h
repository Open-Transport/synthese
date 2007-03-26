
/** MessagesLibraryLog class header.
	@file MessagesLibraryLog.h

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

#ifndef SYNTHESE_MESSAGES_LIBRARY_LOG
#define SYNTHESE_MESSAGES_LIBRARY_LOG

#include "13_dblog/DBLog.h"

namespace synthese
{
	namespace messages
	{
		/** Journal concernant la biblioth�que de messages.

			Les colonnes additionnelles du journal de s�curit� sont :
				- Action : d�crit l'action effectu�e (ex : cr�ation de mod�le de message prioritaire)
				- UID objet concern� : lien vers le message
		*/
		class MessagesLibraryLog : public dblog::DBLog
		{
		public:
			MessagesLibraryLog();
			DBLog::ColumnsVector getColumnNames() const;
		};
	}
}

#endif

