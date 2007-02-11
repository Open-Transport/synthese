
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
		/** Journal d'�v�nements stock� en base de donn�es (abstraite).
			@ingroup m01
		
			Un journal est un compte-rendu d'activit� de SYNTHESE. 
			Plusieurs entr�es sont consign�es dans la base de donn�es sous formes d'entr�es de journal. 
			Le journal lui-m�me est le composant d'administration d�di� � leur consultation.

			Le stockage des entr�es de journal s'effectue dans la base de donn�es SQLite.

			Les �l�ments de journal contiennent les donn�es suivantes :
				- date de l'�v�nement
				- nom du journal (cl� texte identique au nom d'enregistrement de la classe)
				- utilisateur � l'origine de l'�v�nement
				- niveau de l'entr�e (INFO, WARNING, ERROR)
				- texte de l'entr�e (formalisme selon module et rubrique, sp�cifi� par les sous-classes)

			Les diff�rents journaux sont enregistr�s dans l'instance de fabrique Factory<DBLog>.
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
