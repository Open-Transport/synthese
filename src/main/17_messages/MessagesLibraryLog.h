
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

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace messages
	{
		class AlarmTemplate;
		class ScenarioTemplate;

		/** Journal concernant la bibliothèque de messages.

			Les colonnes additionnelles du journal de sécurité sont :
				- Action : décrit l'action effectuée (ex : création de modèle de message prioritaire)
				- UID objet concerné : lien vers le message

			@ingroup m17Logs refLogs
		*/
		class MessagesLibraryLog : public util::FactorableTemplate<dblog::DBLog, MessagesLibraryLog>
		{
		public:
			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;
			static void	addUpdateEntry(
				const AlarmTemplate* alarm
				, const std::string& text
				, const security::User* user
				);
			static void	addUpdateEntry(
				const ScenarioTemplate* scenario
				, const std::string& text
				, const security::User* user
				);
			static void addDeleteEntry(
				const ScenarioTemplate* scenario
				, const security::User* user
				);
			std::string getObjectName(uid id) const;
		};
	}
}

#endif

