
/** DBLog class header.
	@file DBLog.h

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

#ifndef SYNTHESE_UTIL_DBLOG_H
#define SYNTHESE_UTIL_DBLOG_H

#include <string>
#include <vector>

#include "FactoryBase.h"
#include "DBLogEntry.h"
#include "Registry.h"
#include "SecurityTypes.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace security
	{
		class User;
		class Profile;
	}

	namespace server
	{
		class Request;
	}

	namespace dblog
	{
		/** Journal d'événements stocké en base de données (abstraite).
			@ingroup m13

			Un journal est un compte-rendu d'activité de SYNTHESE.
			Plusieurs entrées sont consignées dans la base de données sous formes d'entrées de journal.
			Le journal lui-même est le composant d'administration dédié à leur consultation.

			Le stockage des entrées de journal s'effectue dans la base de données.

			Les éléments de journal contiennent les données suivantes :
				- date de l'événement
				- nom du journal (clé texte identique au nom d'enregistrement de la classe)
				- utilisateur à l'origine de l'événement
				- niveau de l'entrée (INFO, WARNING, ERROR)
				- texte de l'entrée (formalisme selon module et rubrique, spécifié par les sous-classes)

			Les différents journaux sont enregistrés dans l'instance de fabrique Factory<DBLog>.
		*/
		class DBLog
		:	public util::FactoryBase<DBLog>
		{
		public:
			typedef std::vector<std::string> ColumnsVector;

		protected:

			/** Adds an entry to a log (generic method).
				@param logKey key of the DBLog to write
				@param level level of the entry (@see DBLogEntry::Level)
				@param content serialized content of the entry
				@param user user of the entry
				@param objectId id of the referring object
				@return id of the created entry
				@author Hugues Romain
				@date 2008

				This method is intended to be used by subclasses to do managed entry creations.
			*/
			static util::RegistryKeyType _addEntry(
				const std::string& logKey
				, Level level
				, const DBLogEntry::Content& content
				, const security::User* user
				, util::RegistryKeyType objectId = 0
				, util::RegistryKeyType objectId2 = 0
			);



			/** Reads the last entry of a log.
				@param logKey key of the DBLog to write
				@param objectId id of the referring object (can be undefined)
				@return The last log entry of the specified log, referring the specified object if any
			*/
			static boost::shared_ptr<DBLogEntry> _getLastEntry(
				const std::string& logKey,
				boost::optional<util::RegistryKeyType> objectId = boost::optional<util::RegistryKeyType>()
			);

		public:

			virtual std::string getName() const = 0;
			virtual ColumnsVector getColumnNames() const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Generates the display of the log specific columns.
			/// @param entry to parse
			/// @searchRequest request which has generated the display : can be reused
			///		to generate other requests at the display
			/// @return all columns content to display
			virtual ColumnsVector parse(
				const DBLogEntry& entry,
				const server::Request& searchRequest
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Object column name getter.
			/// If empty result, then the column is not displayed.
			/// Virtual method : the default implementation returns "Objet"
			virtual std::string getObjectColumnName() const;


			virtual std::string getObjectName(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Object column name getter.
			/// If empty result, then the column is not displayed.
			/// Virtual method : the default implementation returns empty string
			virtual std::string getObject2ColumnName() const;



			virtual std::string getObject2Name(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;


			//////////////////////////////////////////////////////////////////////////
			/** Authorization tester.
			 * Each subclass of DBLog must implement an authorization method depending on the request.
			 * @param request the request which generated the display of the log
			 * @param level needed level
			 * @return true if the log can be displayed
			 */
			virtual bool isAuthorized(
 				const security::Profile& profile,
 				const security::RightLevel& level
 			) = 0;



			static util::RegistryKeyType AddSimpleEntry(
				const std::string& logKey,
				Level level,
				const std::string& content,
				const security::User* user,
				util::RegistryKeyType objectId = 0,
				util::RegistryKeyType objectId2 = 0
			);

		};
	}
}

#endif
