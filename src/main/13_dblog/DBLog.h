
/** DBLog class header.
	@file DBLog.h

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

#ifndef SYNTHESE_UTIL_DBLOG_H
#define SYNTHESE_UTIL_DBLOG_H

#include <string>
#include <vector>

#include "01_util/Factorable.h"

#include "13_dblog/DBLogEntry.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace dblog
	{
		/** Journal d'événements stocké en base de données (abstraite).
			@ingroup m01
		
			Un journal est un compte-rendu d'activité de SYNTHESE. 
			Plusieurs entrées sont consignées dans la base de données sous formes d'entrées de journal. 
			Le journal lui-même est le composant d'administration dédié à leur consultation.

			Le stockage des entrées de journal s'effectue dans la base de données SQLite.

			Les éléments de journal contiennent les données suivantes :
				- date de l'événement
				- nom du journal (clé texte identique au nom d'enregistrement de la classe)
				- utilisateur à l'origine de l'événement
				- niveau de l'entrée (INFO, WARNING, ERROR)
				- texte de l'entrée (formalisme selon module et rubrique, spécifié par les sous-classes)

			Les différents journaux sont enregistrés dans l'instance de fabrique Factory<DBLog>.
		*/
		class DBLog : public util::Factorable
		{
		public:
			typedef std::vector<std::string> ColumnsNameVector;
			

		protected:
			void	write(DBLogEntry::Level level, const DBLogEntry::Content& content, security::User* user = NULL);

		private:
			const std::string _name;
				
		public:
			DBLog(const std::string& name);

			//static std::string getModuleName() = 0;
			const std::string& getName() const;
			virtual ColumnsNameVector getColumnNames() const = 0;
		};
	}
}

#endif
