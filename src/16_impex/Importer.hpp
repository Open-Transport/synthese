
/** Importer class header.
	@file Importer.hpp

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

#ifndef SYNTHESE_impex_Importer_hpp__
#define SYNTHESE_impex_Importer_hpp__

#include "Env.h"
#include "ParametersMap.h"
#include "DBTransaction.hpp"
#include "AdminInterfaceElement.h"

#include <ostream>
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;

		/** Importer class.
			@ingroup m16
		*/
		class Importer
		{
		public:

			class Logger
			{
			public:
				typedef enum
				{
					ALL = 0,
					DEBG = 10,
					LOAD = 20,
					CREA = 25,
					INFO = 30,
					WARN = 40,
					NOTI = 50,
					ERROR = 60,
					NOLOG = 99
				} Level;

				struct Entry
				{
					Level level;
					std::string content;
					Entry(Level level_, const std::string& content_):
					level(level_), content(content_) {}
				};

				typedef std::vector<Entry> Entries;

			private:
				Entries _entries;
				const Level _minLevel;
				Level _maxLoggedLevel;

			public:
				Logger(
					Level minLevel
				);

				void log(
					Level level,
					const std::string& content
				);

				void output(
					std::ostream& stream
				) const;

				Level getMaxLoggedLevel() const { return _maxLoggedLevel; }
			};

			Importer(
				util::Env& env,
				const DataSource& dataSource,
				Logger::Level minLevel = Logger::ALL
			):	_env(env),
				_dataSource(dataSource),
				_logger(minLevel)
			{}

		protected:
			util::Env&						_env;
			const DataSource&						_dataSource;
			boost::optional<boost::filesystem::path> _logPath;
			mutable Logger							_logger;

			virtual db::DBTransaction _save() const = 0;

		public:
			//! @name Getters
			//@{
				const DataSource& getDataSource() const { return _dataSource; }
				boost::optional<boost::filesystem::path> getLogPath() const { return _logPath; }
				const Logger& getLogger() const { return _logger; }
			//@}

			//! @name Setters
			//@{
				void setLogPath(boost::optional<boost::filesystem::path> value){ _logPath = value; }
			//@}

			virtual bool beforeParsing() { return true; }
			virtual bool afterParsing() { return true; }

			//////////////////////////////////////////////////////////////////////////
			/// Purge the obsolete data imported by the source
			/// @param firstDayToKeep the first day to keep
			virtual void cleanObsoleteData(const boost::gregorian::date& firstDayToKeep) const {}

			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// @param map Parameters map to interpret
			/// @param doImport tests if the parameters are valid for an import
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			virtual void setFromParametersMap(
				const util::ParametersMap& map,
				bool doImport
			) = 0;

			virtual util::ParametersMap getParametersMap() const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Launches the parsing of the files, with output on an admin page.
			/// @param os output stream
			/// @param request admin
			/// @return true if it is allowed to save the data
			virtual bool parseFiles(
				std::ostream& os,
				boost::optional<const server::Request&> request
			) const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Interface for the save method.
			/// @return transaction to run
			db::DBTransaction save() const
			{
				db::DBTransaction result(_save());
				_env.clear();
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Import screen to include in the administration console.
			/// @param os stream to write the result on
			/// @param request request for display of the administration console
			/// @since 3.2.0
			/// @date 2010
			virtual void displayAdmin(
				std::ostream& os,
				const server::Request& request
			) const = 0;
		};
}	}

#endif // SYNTHESE_impex_Importer_hpp__
